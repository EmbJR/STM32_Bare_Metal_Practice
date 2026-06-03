# bl_protocol.py
import serial
import time
from bl_crc import calculate_crc16

class BootloaderProtocol:
    HEADER = 0xA5
    CMD_GET_INFO = 0x02
    CMD_SET_CHUNK_SIZE = 0x03
    CMD_SET_START_ADDR = 0x04
    CMD_FW_DATA = 0x05
    STATUS_SUCCESS = 0x64

    STATUS_MAP = {
        0x64: "Success",
        0x65: "Internal Error",
        0x66: "Unknown Command",
        0x67: "Data Unavailable",
        0x68: "Invalid Length",
        0x69: "Invalid CRC",
        0x6A: "Flash Write Error"
    }

    def __init__(self, port, baudrate=115200, timeout=0.1, response_timeout=1.0):
        self.ser = serial.Serial(port, baudrate, timeout=timeout)
        self.retry_limit = 5
        self.response_timeout = response_timeout

    def make_packet(self, cmd: int, payload: bytes = b"") -> bytes:
        length = 1 + len(payload)
        data_to_crc = bytes([cmd]) + payload
        crc = calculate_crc16(data_to_crc)

        packet = bytearray([self.HEADER])
        packet.extend(crc.to_bytes(2, 'big'))
        packet.extend(length.to_bytes(2, 'big'))
        packet.extend(data_to_crc)
        return bytes(packet)

    def _read_exact(self, size: int, deadline: float) -> bytes:
        buffer = bytearray()
        while len(buffer) < size and time.time() < deadline:
            chunk = self.ser.read(size - len(buffer))
            if chunk:
                buffer.extend(chunk)
        return bytes(buffer)

    def _read_response(self):
        deadline = time.time() + self.response_timeout
        while time.time() < deadline:
            header = self.ser.read(1)
            if not header:
                continue
            if header[0] != self.HEADER:
                continue

            meta = self._read_exact(4, deadline)
            if len(meta) < 4:
                return None, "Incomplete response header"

            resp_crc = int.from_bytes(meta[0:2], 'little')
            payload_length = int.from_bytes(meta[2:4], 'little')
            if payload_length <= 0:
                return None, "Invalid response length"

            payload = self._read_exact(payload_length, deadline)
            if len(payload) < payload_length:
                return None, f"Incomplete payload (expected {payload_length}, got {len(payload)})"

            if calculate_crc16(payload) != resp_crc:
                return None, "Invalid response CRC"

            return payload, None
        return None, "Response timeout"

    def send_and_wait(self, cmd: int, payload: bytes = b""):
        packet = self.make_packet(cmd, payload)

        for attempt in range(1, self.retry_limit + 1):
            self.ser.reset_input_buffer()
            self.ser.write(packet)

            payload_rx, error = self._read_response()
            if payload_rx is not None:
                resp_cmd = payload_rx[0]
                if resp_cmd != cmd:
                    reason = f"Command mismatch (sent {hex(cmd)}, got {hex(resp_cmd)})"
                elif len(payload_rx) < 2:
                    reason = "Malformed response"
                else:
                    status = payload_rx[1]
                    status_text = self.STATUS_MAP.get(status, f"Error {hex(status)}")
                    print(
                        f"[BootloaderProtocol] Received reply for cmd {hex(cmd)}: "
                        f"resp_cmd={hex(resp_cmd)} status={hex(status)} ({status_text}) "
                        f"payload={payload_rx.hex()}"
                    )
                    if status == self.STATUS_SUCCESS:
                        return True
                    reason = status_text
            else:
                print(f"[BootloaderProtocol] No valid reply for cmd {hex(cmd)}: {error}")
                reason = error

            print(f"[BootloaderProtocol] Retry {attempt}/{self.retry_limit} for cmd {hex(cmd)}: {reason}")

        return False

    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()
