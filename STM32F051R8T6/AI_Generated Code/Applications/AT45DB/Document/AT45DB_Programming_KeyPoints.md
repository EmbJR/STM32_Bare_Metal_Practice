# AT45DB041D / AT45DB041E (DataFlash) — programming key points

Sources: `AT45DB041D.PDF` and `REN_DS-AT45DB041E-8783Q-052023_DST_20230524.pdf`.

This note summarizes the practical, firmware-oriented items you need when reading/writing/erasing these parts over SPI.

---

## 1) SPI / bus fundamentals (applies to both)

- Interface is SPI-compatible (RapidS). Supports SPI Mode 0 and Mode 3.
- A valid command frame:
  - Starts on CS falling edge.
  - Then 8-bit opcode (MSB first).
  - Often followed by 3 address bytes and sometimes dummy bytes.
  - Ends on CS rising edge.
- Data in (MOSI/SI) is latched on SCK rising edge.
- Data out (MISO/SO) changes on SCK falling edge.
- Keep CS, SCK, SI edges clean (ringing/noise can be interpreted as extra clocks).

Power-up sequencing:
- Don’t access the device until VCC is valid and the required power-up delays have elapsed.
- Renesas AT45DB041E additionally recommends a ~10 kΩ pull-up on CS to ensure CS rises with VCC.

---

## 2) Memory organization & the “page size” trap

Main memory array:
- Organized as 2048 pages.
- Page size can be either:
  - Standard DataFlash page: 264 bytes (default on ship for both families)
  - “Power-of-2” page: 256 bytes

Why this matters:
- Address bit mapping and “byte-in-page” interpretation differ between 264 vs 256 mode.
- Your firmware must know the configured page size before forming addresses.

How to detect current page size:
- Read Status Register; PAGE SIZE bit indicates 256 vs 264.

Configuration differences:
- AT45DB041D:
  - One-time programmable switch to 256-byte pages (cannot go back).
  - Requires power cycle after programming configuration.
- AT45DB041E:
  - Page size configuration is reversible (256 <-> 264) via command sequences.
  - Stored in a nonvolatile register (has a finite cycle rating; don’t flip constantly).
  - No power cycle required after configuration.

---

## 3) Addressing model you must implement

The device uses 3 address bytes, but many bits are “dummy/don’t care” depending on command and page size.

Terminology used in the datasheets:
- Standard page size (264):
  - PA10..PA0 = page address (11 bits)
  - BA8..BA0  = byte address within page (9 bits)
  - Buffer address also uses 9 bits (BFA8..BFA0)
- Binary page size (256):
  - A18..A8 = page address (11 bits)
  - A7..A0  = byte address within page (8 bits)
  - Buffer address uses 8 bits (BFA7..BFA0)

Practical implication:
- For portability, build helpers that:
  1) Read Status Register at init to decide 256 vs 264.
  2) Encode address fields accordingly for every command.

---

## 4) Reading data (common patterns)

### Continuous Array Read
Use when you want to stream across page boundaries.

- Opcodes (both families):
  - 0x0B (high frequency) — requires 1 dummy byte after the 3 address bytes.
  - 0x03 (low frequency) — no dummy byte.
  - 0xE8 legacy continuous read exists but is “not recommended for new designs”.
- Behavior: internal address counter increments continuously; wraps to next page automatically.
- End by deasserting CS.

AT45DB041E adds:
- 0x1B (very high frequency read) — requires 2 dummy bytes.
- 0x01 (low-power read) — up to lower clock rate, no dummy bytes.

### Main Memory Page Read
Use when you want to read within a single page.

- Opcode: 0xD2
- Requires 4 dummy bytes after address.
- Behavior: when end-of-page is reached, it wraps to the beginning of the *same page* (not the next page).

### Buffer Read
Read SRAM buffer contents directly.

- Buffer 1 read: 0xD4 (hi) or 0xD1 (lo)
- Buffer 2 read: 0xD6 (hi) or 0xD3 (lo)
- Hi-frequency variants typically require a dummy byte.
- When end of buffer is reached, it wraps back to the beginning of the buffer.

---

## 5) Writing/programming data: recommended approaches

The device has two independent SRAM buffers (Buffer 1 and Buffer 2). Efficient firmware uses them to pipeline streaming writes.

### A) Two-step page program (classic DataFlash flow)
1) Load SRAM buffer with your payload
   - Buffer 1 Write: 0x84
   - Buffer 2 Write: 0x87
   - Address selects the start offset in the buffer.
   - Data wraps if you keep clocking past end of buffer.
2) Program buffer into main memory page
   - With built-in erase:
     - Buffer 1 -> Main: 0x83
     - Buffer 2 -> Main: 0x86
   - Without built-in erase (page must be pre-erased):
     - Buffer 1 -> Main: 0x88
     - Buffer 2 -> Main: 0x89
3) Poll Status Register RDY/BUSY until ready.

When to use which:
- Use “with built-in erase” for general page updates (simpler, safe).
- Use “without built-in erase” only if you have already erased the target region and want to minimize wear/time.

### B) One-command “program through buffer” (simplifies firmware)
- Main memory page program through buffer with built-in erase:
  - Buffer 1: 0x82
  - Buffer 2: 0x85
- You provide opcode + address + data stream; on CS high, device performs erase+program internally.

### C) Partial byte/page programming (AT45DB041E feature)
AT45DB041E supports programming 1..page_size bytes to pre-erased locations:
- Main Memory Byte/Page Program through Buffer 1 without built-in erase: 0x02
- Only bytes clocked in are programmed; other bytes in the page remain unchanged.
- CS must be released on a byte boundary; otherwise the operation aborts.

### D) Read-modify-write / EEPROM emulation style updates
- AT45DB041E provides an explicit Read-Modify-Write operation using opcodes 0x58 (Buffer 1) / 0x59 (Buffer 2):
  - It internally transfers page -> buffer, applies your byte stream, then programs page back.
  - If you send *no data bytes* after the address, 0x58/0x59 instead performs “Auto Page Rewrite”.

AT45DB041D has “Auto Page Rewrite” (0x58/0x59) described as a maintenance/refresh algorithm; its datasheet emphasizes page refresh requirements during heavy random updates.

---

## 6) Erasing (granularity & opcodes)

Erase granularity:
- Page erase (one page): 0x81
- Block erase (8 pages ≈ 2 kB): 0x50
- Sector erase (64 kB): 0x7C
- Chip erase (entire array): command sequence 0xC7, 0x94, 0x80, 0x9A

Important note for AT45DB041D:
- Chip erase has an errata; it may fail on some units. Workaround: do not use Chip Erase; use Block Erase instead.

---

## 7) Status Register polling (don’t guess timing)

### AT45DB041D
- Status Read: 0xD7 (1-byte status)
- Key bits:
  - Bit7 RDY/BUSY: 1=ready, 0=busy
  - Bit6 COMP: compare result
  - Bit1 PROTECT: sector protection enabled
  - Bit0 PAGE SIZE: 1=256, 0=264

### AT45DB041E
- Status Read: 0xD7 (2-byte status stream)
- Byte 1 has RDY/BUSY, COMP, density code, PROTECT, PAGE SIZE.
- Byte 2 includes:
  - EPE (erase/program error)
  - SLE (sector lockdown enabled)
  - PS1/PS2/ES (program/erase suspend state)

Polling rule:
- Keep clocking status bytes until RDY/BUSY indicates ready. Don’t rely only on worst-case delays.

---

## 8) Protection & security (what can break your writes)

- WP pin (hardware protection) can cause program/erase commands to be ignored.
- Sector Protection:
  - Enable: 0x3D 0x2A 0x7F 0xA9
  - Disable: 0x3D 0x2A 0x7F 0x9A
  - Sector Protection Register must be erased then programmed to change which sectors are protected.
- Sector Lockdown:
  - Permanently makes a sector read-only.
  - Command sequence includes 0x3D 0x2A 0x7F 0x30 followed by an address in the target sector.
- Security Register:
  - 128 bytes total; 64 bytes user OTP + 64 bytes factory unique ID.
  - Program: 0x9B 0x00 0x00 0x00 then 64 bytes
  - Read: 0x77 + 3 dummy bytes then read out

AT45DB041E extras:
- Freeze Sector Lockdown: 0x34 0x55 0xAA 0x40 (permanently disables further lockdown changes).

---

## 9) Power management commands

Deep Power-Down (both):
- Enter: 0xB9
- Resume: 0xAB
- While in deep power-down, all commands are ignored except resume.

AT45DB041E adds Ultra-Deep Power-Down:
- Enter: 0x79
- Exit: pulse CS low then high; wait the specified exit time.
- SRAM buffer contents are lost in ultra-deep power-down.

---

## 10) “Command concurrency” rules (avoid undefined behavior)

Both datasheets describe command grouping rules (A/B/C/D) — the intent is:
- Don’t start a new command while an earlier command frame is still active.
- During internal self-timed program/erase, you can often still read status (and in some cases read from buffers).
- If you pipeline writes, use Buffer 1 and Buffer 2 alternately so that while one buffer’s content is being programmed into flash, you can fill/read the other buffer.

AT45DB041E further supports Program/Erase Suspend (0xB0) and Resume (0xD0) for some operations, with restrictions.

---

## 11) Practical firmware checklist (STM32-style)

1) Hardware init
   - Configure SPI mode 0 or 3.
   - Respect max clock rate (and start slower for bring-up).
   - Ensure CS is a GPIO and you control its edges cleanly.

2) Device init
   - Read JEDEC ID (0x9F) to confirm device is present.
   - Read Status Register and cache PAGE SIZE mode.

3) Read implementation
   - Prefer 0x0B continuous read for general streaming (1 dummy byte).
   - Use 0xD2 page read if you truly want page-local wrap behavior.

4) Write implementation
   - Default safe method: Program Through Buffer with built-in erase (0x82/0x85).
   - For streaming logs: alternate Buffer 1/2, using 0x84/0x87 then 0x83/0x86.
   - Always poll RDY/BUSY after program/erase.

5) Erase strategy
   - Prefer Block Erase (0x50) or Sector Erase (0x7C) for bulk erase.
   - Avoid Chip Erase on AT45DB041D (errata).

6) Robustness
   - On AT45DB041E, check EPE after erase/program to detect failures.
   - Consider page compare (0x60/0x61) after programming if integrity is critical.

---

## 12) Quick opcode map (common ones)

Read:
- 0x9F: JEDEC Manufacturer/Device ID
- 0xD7: Status Register Read
- 0x0B / 0x03: Continuous Array Read
- 0xD2: Main Memory Page Read

Buffer:
- 0x84 / 0x87: Buffer 1/2 Write
- 0xD4 / 0xD6: Buffer 1/2 Read (hi freq)

Program:
- 0x82 / 0x85: Main memory program through buffer with built-in erase (Buf1/Buf2)
- 0x83 / 0x86: Buffer 1/2 -> Main memory page program with built-in erase
- 0x88 / 0x89: Buffer 1/2 -> Main memory page program without built-in erase

Erase:
- 0x81: Page Erase
- 0x50: Block Erase
- 0x7C: Sector Erase
- 0xC7 0x94 0x80 0x9A: Chip Erase

Power:
- 0xB9 / 0xAB: Deep power-down / resume
- 0x79: Ultra-deep power-down (AT45DB041E)

