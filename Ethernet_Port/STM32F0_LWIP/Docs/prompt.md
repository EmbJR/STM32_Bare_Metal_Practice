
- I am developing the Ethernet controlled data transfer device using STM32F051R8 + ENC25J60 + LWIP stack.
- Please refer to the document first placed at '\Docs\ENC28J60.pdf'.
- I want you to generate the ENC28J60 driver code to be used in the firmware.
- I have generated some basic code for ENC28J60 chip which has been placed at '\Perf_Lib\enc28j60'. Please complete the driver code placed at the path.
- You only need to edit the files which are "enc28j60.c" and "enc28j60.h".
- Please only use the libraries like SPI/GPIO or any required components from path '\Perf_Lib'.
- The code should not have any third party library dependencies like CMCIC/HAL.
- The code should reflect the functionality and configurations support as described in document placed at '\Docs\ENC28J60.pdf'.
- Please don't delete the existing code and reuse/edit it wherever possible.
- Also use the "SPI_TransmitReceiveBuffer" API for all spi related operations if possible.



---------------------------------------------
The ENC28J60 driver is complete. Here's a summary of what was implemented:

enc28j60.h (512 lines) — Complete register map:

All 4 banked register maps (Bank 0–3) with address macros
All SPI opcodes (RCR 0x00, RBM 0x3A, WCR 0x40, WBM 0x7A, BFS 0x80, BFC 0xA0, SRC 0xFF)
All ECON1/ECON2/EIE/EIR/ESTAT register bit definitions
All MACON1–4, MISTAT, ERXFCON configuration bits
Full PHY register map (PHCON1/2, PHSTAT1/2, PHANA, etc.)
RX/TX status vector definitions
ENC28J60_HandleTypeDef and ENC28J60_ConfigTypeDef structures
Public API for init, register/PHY access, MAC address, packet TX/RX, interrupts, link status
enc28j60.c (956 lines) — Complete driver implementation:

SPI+GPIO initialization on SPI2 (PB12=CS, PB13=SCK, PB14=MISO, PB15=MOSI)
SPI ReadRegRaw/WriteRegRaw (no bank switching) + public ReadReg/WriteReg/SetBitField/ClearBitField (with bank switching)
Buffer memory read/write using SPI_TransmitReceiveBuffer for all SPI operations
Bank selection with caching to avoid redundant switches
PHY register read/write via MII management interface with busy-wait polling
Soft reset with CLKRDY wait
MAC initialization (full/half duplex, inter-packet gap)
PHY initialization (auto-negotiation or forced link with configurable speed/duplex)
Packet TX with per-packet control byte, TXRTS trigger, and TSV readback
Packet RX with 6-byte status vector parsing, RX_OK validation, wrap-around handling, and PKTDEC
Discard RX packet, pending packet count, interrupt enable/disable/clear, link status query