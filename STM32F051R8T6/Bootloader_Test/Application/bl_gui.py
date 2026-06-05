# bl_gui.py
import math
import os
import threading
import time
import tkinter as tk
from bl_crc import calculate_crc16
from tkinter import filedialog, messagebox, ttk

from bl_protocol import BootloaderProtocol

class BootloaderGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("STM32 Bootloader GUI")
        self.root.geometry("560x420")
        self.root.resizable(False, False)
        self.root.grid_columnconfigure(1, weight=1)

        tk.Label(root, text="Serial Port:").grid(row=0, column=0, padx=10, pady=8, sticky="w")
        self.port_box = ttk.Combobox(root, width=16, state="normal", values=self._scan_serial_ports())
        self.port_box.set("COM3")
        self.port_box.grid(row=0, column=1, padx=(0, 10), sticky="ew")
        self.refresh_btn = ttk.Button(root, text="Refresh", command=self.refresh_ports)
        self.refresh_btn.grid(row=0, column=2, padx=10, sticky="ew")

        tk.Label(root, text="Baudrate:").grid(row=1, column=0, padx=10, sticky="w")
        self.baud_entry = tk.Entry(root)
        self.baud_entry.insert(0, "115200")
        self.baud_entry.grid(row=1, column=1, padx=(0, 10), sticky="ew")

        tk.Label(root, text="Start Addr (Hex):").grid(row=2, column=0, padx=10, sticky="w")
        self.addr_entry = tk.Entry(root)
        self.addr_entry.insert(0, "0x08000000")
        self.addr_entry.grid(row=2, column=1, padx=(0, 10), sticky="ew")

        tk.Label(root, text="Chunk Size (Bytes):").grid(row=3, column=0, padx=10, sticky="w")
        self.chunk_entry = tk.Entry(root)
        self.chunk_entry.insert(0, "256")
        self.chunk_entry.grid(row=3, column=1, padx=(0, 10), sticky="ew")

        tk.Label(root, text="Delay Between Commands (ms):").grid(row=4, column=0, padx=10, sticky="w")
        self.delay_entry = tk.Entry(root)
        self.delay_entry.insert(0, "0")
        self.delay_entry.grid(row=4, column=1, padx=(0, 10), sticky="ew")

        self.file_btn = ttk.Button(root, text="Select Bin File", command=self.select_file)
        self.file_btn.grid(row=5, column=0, columnspan=3, pady=12, padx=10, sticky="ew")

        self.file_label = tk.Label(root, text="No file selected", anchor="w", fg="#333333")
        self.file_label.grid(row=6, column=0, columnspan=3, padx=10, sticky="ew")

        self.progress = ttk.Progressbar(root, orient="horizontal", length=420, mode="determinate", maximum=100)
        self.progress.grid(row=7, column=0, columnspan=2, pady=14, padx=10, sticky="ew")

        self.progress_percent_var = tk.StringVar(value="0.0 %")
        self.progress_label = tk.Label(root, textvariable=self.progress_percent_var, width=8)
        self.progress_label.grid(row=7, column=2, padx=10, sticky="e")

        self.status_var = tk.StringVar(value="Ready")
        self.status_label = tk.Label(root, textvariable=self.status_var, anchor="w", fg="#1a1a1a")
        self.status_label.grid(row=8, column=0, columnspan=3, padx=10, sticky="ew")

        self.update_btn = ttk.Button(root, text="Start Update", command=self.start_update, state="disabled")
        self.update_btn.grid(row=9, column=0, columnspan=1, pady=12, padx=(10, 5), sticky="ew")

        self.cancel_btn = ttk.Button(root, text="Cancel", command=self.cancel_update, state="disabled")
        self.cancel_btn.grid(row=9, column=1, columnspan=2, pady=12, padx=(5, 10), sticky="ew")

        self.file_path = ""
        self.cancel_event = threading.Event()

    def _scan_serial_ports(self):
        try:
            from serial.tools import list_ports
            return [port.device for port in list_ports.comports()]
        except Exception:
            return []

    def refresh_ports(self):
        ports = self._scan_serial_ports()
        if ports:
            self.port_box.config(values=ports)
            self.port_box.set(ports[0])
            self._set_status(f"Found {len(ports)} port(s)")
        else:
            self._set_status("No serial ports detected")

    def select_file(self):
        self.file_path = filedialog.askopenfilename(filetypes=[("Binary files", "*.bin")])
        if self.file_path:
            file_size = os.path.getsize(self.file_path)
            self.file_label.config(text=f"Selected: {os.path.basename(self.file_path)} ({file_size} bytes)")
            self.update_btn.config(state="normal")
            self._set_status("File selected, ready to flash")

    def start_update(self):
        if not self.file_path:
            messagebox.showwarning("No File", "Please select a .bin file before starting the update.")
            return

        try:
            port = self.port_box.get().strip()
            baudrate = int(self.baud_entry.get().strip())
            start_addr = int(self.addr_entry.get().strip(), 16)
            chunk_size = int(self.chunk_entry.get().strip())
            delay_ms = int(self.delay_entry.get().strip())
            if delay_ms < 0:
                raise ValueError("Delay must be zero or positive")
        except ValueError as exc:
            messagebox.showerror("Invalid Input", f"Please verify serial configuration and numeric fields.\n{exc}")
            return

        if not port:
            messagebox.showerror("Port Required", "Please specify a serial port.")
            return

        self._set_controls_running(True)
        self.cancel_event.clear()

        update_thread = threading.Thread(
            target=self._run_update,
            args=(port, baudrate, start_addr, chunk_size, delay_ms / 1000.0),
            daemon=True,
        )
        update_thread.start()

    def cancel_update(self):
        self.cancel_event.set()
        self._set_status("Cancel requested, stopping after current chunk...")

    def _run_update(self, port, baudrate, start_addr, chunk_size, command_delay):
        try:
            bl = BootloaderProtocol(port, baudrate, command_delay=command_delay)
            file_size = os.path.getsize(self.file_path)
            num_chunks = math.ceil(file_size / chunk_size)

            self._set_status("Setting start address...")
            if not bl.send_and_wait(bl.CMD_SET_START_ADDR, start_addr.to_bytes(4, 'big')):
                raise RuntimeError("Failed to set start address")

            self._set_status("Sending transfer setup...")
            setup_payload = chunk_size.to_bytes(2, 'big') + num_chunks.to_bytes(2, 'big')
            if not bl.send_and_wait(bl.CMD_SET_CHUNK_SIZE, setup_payload):
                raise RuntimeError("Failed to set chunk size")

            self._set_progress(0)
            self._set_status("Uploading firmware...")

            with open(self.file_path, 'rb') as firmware:
                for chunk_index in range(num_chunks):
                    if self.cancel_event.is_set():
                        raise RuntimeError("Firmware update cancelled")

                    data = firmware.read(chunk_size)
                    if not data:
                        break

                    self._set_status(f"Sending chunk {chunk_index + 1}/{num_chunks}...")
                    if not bl.send_and_wait(bl.CMD_FW_DATA, data):
                        raise RuntimeError(f"Failed at chunk {chunk_index + 1}")

                    percent_complete = ((chunk_index + 1) / num_chunks) * 100
                    self._set_progress(percent_complete)

            self._set_status("Calculating firmware CRC...")
            # Re-open the file to calculate the CRC of the entire firmware
            with open(self.file_path, 'rb') as firmware_full:
                full_firmware_data = firmware_full.read()
                firmware_crc = calculate_crc16(full_firmware_data)

            self._set_status("Sending transfer done command with CRC...")
            if not bl.send_and_wait(bl.CMD_TRANSFER_DONE_UPLOAD, firmware_crc.to_bytes(2, 'big')):
                raise RuntimeError("Failed to send transfer done command or CRC mismatch")

            self._finish(True, "Firmware update completed successfully!")
        except Exception as exc:
            self._finish(False, str(exc))
        finally:
            try:
                bl.close()
            except Exception:
                pass

    def _set_controls_running(self, running: bool):
        state = "disabled" if running else "normal"
        self.update_btn.config(state="disabled" if running else ("normal" if self.file_path else "disabled"))
        self.cancel_btn.config(state="normal" if running else "disabled")
        self.file_btn.config(state=state)
        self.refresh_btn.config(state=state)
        self.port_box.config(state="disabled" if running else "normal")
        self.baud_entry.config(state=state)
        self.addr_entry.config(state=state)
        self.chunk_entry.config(state=state)
        self.delay_entry.config(state=state)

    def _set_progress(self, percent: float):
        self.root.after(0, self.progress.configure, value=percent)
        self.root.after(0, self.progress_percent_var.set, f"{percent:5.1f} %")

    def _set_status(self, message: str):
        self.root.after(0, self.status_var.set, message)

    def _finish(self, success: bool, message: str):
        def done():
            self._set_controls_running(False)
            self._set_status("Update completed" if success else "Error occurred")
            if success:
                messagebox.showinfo("Success", message)
            else:
                messagebox.showerror("Error", message)

        self.root.after(0, done)

if __name__ == "__main__":
    root = tk.Tk()
    app = BootloaderGUI(root)
    root.mainloop()
