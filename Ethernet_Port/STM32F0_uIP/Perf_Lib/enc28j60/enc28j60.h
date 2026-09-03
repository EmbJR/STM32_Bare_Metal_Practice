/**
 * @file    enc28j60.h
 * @brief   ENC28J60 Ethernet Controller Driver
 * @author  AI Generated (corrected per datasheet)
 * @date    2026-08-04
 *
 * @note    Driver for Microchip ENC28J60 Stand-Alone Ethernet Controller
 *          with SPI Interface, targeted for STM32F051R8.
 */

#ifndef ENC28J60_H
#define ENC28J60_H

#include <stdint.h>
#include <stdbool.h>
#include "spiF051.h"
#include "gpio.h"

/*============================================================================
 * SPI Configuration - using SPI2
 *============================================================================*/
#define ENC28J60_SPI                SPI2
#define ENC28J60_SPI_CLOCK_PRESCALER SPI_BAUDRATEPRESCALER_16

/*============================================================================
 * GPIO Configuration
 * SCK:  PB13
 * MISO: PB14
 * MOSI: PB15
 * CS:   PB12
 *============================================================================*/
#define ENC28J60_GPIO_PORT          GPIOB
#define ENC28J60_CS_PIN             GPIO_PIN_12
#define ENC28J60_SCK_PIN            GPIO_PIN_13
#define ENC28J60_MISO_PIN           GPIO_PIN_14
#define ENC28J60_MOSI_PIN           GPIO_PIN_15

/*============================================================================
 * ENC28J60 SPI Instruction Set (Opcodes)
 *============================================================================*/
#define ENC28J60_READ_CTRL_REG      0x00
#define ENC28J60_READ_BUF_MEM       0x3A
#define ENC28J60_WRITE_CTRL_REG     0x40
#define ENC28J60_WRITE_BUF_MEM      0x7A
#define ENC28J60_BIT_FIELD_SET      0x80
#define ENC28J60_BIT_FIELD_CLR      0xA0
#define ENC28J60_SYSTEM_RESET_CMD   0xFF

/*============================================================================
 * Bank Select Macros
 *============================================================================*/
#define BANK0   0
#define BANK1   1
#define BANK2   2
#define BANK3   3

#define BANK_SHIFT          5
#define ADDR_MASK           0x1F
#define BANK_MASK           0x60

/*============================================================================
 * ENC28J60 Register Map (Banked Memory Map)
 *============================================================================*/

/* Bank 0 Registers */
#define ERDPTL      ((BANK0 << BANK_SHIFT) | 0x00)
#define ERDPTH      ((BANK0 << BANK_SHIFT) | 0x01)
#define EWRPTL      ((BANK0 << BANK_SHIFT) | 0x02)
#define EWRPTH      ((BANK0 << BANK_SHIFT) | 0x03)
#define ETXSTL      ((BANK0 << BANK_SHIFT) | 0x04)
#define ETXSTH      ((BANK0 << BANK_SHIFT) | 0x05)
#define ETXNDL      ((BANK0 << BANK_SHIFT) | 0x06)
#define ETXNDH      ((BANK0 << BANK_SHIFT) | 0x07)
#define ERXSTL      ((BANK0 << BANK_SHIFT) | 0x08)
#define ERXSTH      ((BANK0 << BANK_SHIFT) | 0x09)
#define ERXNDL      ((BANK0 << BANK_SHIFT) | 0x0A)
#define ERXNDH      ((BANK0 << BANK_SHIFT) | 0x0B)
#define ERXRDPTL    ((BANK0 << BANK_SHIFT) | 0x0C)
#define ERXRDPTH    ((BANK0 << BANK_SHIFT) | 0x0D)
#define ERXWRPTL    ((BANK0 << BANK_SHIFT) | 0x0E)
#define ERXWRPTH    ((BANK0 << BANK_SHIFT) | 0x0F)
#define EDMASTL     ((BANK0 << BANK_SHIFT) | 0x10)
#define EDMASTH     ((BANK0 << BANK_SHIFT) | 0x11)
#define EDMANDL     ((BANK0 << BANK_SHIFT) | 0x12)
#define EDMANDH     ((BANK0 << BANK_SHIFT) | 0x13)
#define EDMADSTL    ((BANK0 << BANK_SHIFT) | 0x14)
#define EDMADSTH    ((BANK0 << BANK_SHIFT) | 0x15)
#define EDMACSL     ((BANK0 << BANK_SHIFT) | 0x16)
#define EDMACSH     ((BANK0 << BANK_SHIFT) | 0x17)
/* Bank 1 Registers */
#define EHT0        ((BANK1 << BANK_SHIFT) | 0x00)
#define EHT1        ((BANK1 << BANK_SHIFT) | 0x01)
#define EHT2        ((BANK1 << BANK_SHIFT) | 0x02)
#define EHT3        ((BANK1 << BANK_SHIFT) | 0x03)
#define EHT4        ((BANK1 << BANK_SHIFT) | 0x04)
#define EHT5        ((BANK1 << BANK_SHIFT) | 0x05)
#define EHT6        ((BANK1 << BANK_SHIFT) | 0x06)
#define EHT7        ((BANK1 << BANK_SHIFT) | 0x07)
#define EPMM0       ((BANK1 << BANK_SHIFT) | 0x08)
#define EPMM1       ((BANK1 << BANK_SHIFT) | 0x09)
#define EPMM2       ((BANK1 << BANK_SHIFT) | 0x0A)
#define EPMM3       ((BANK1 << BANK_SHIFT) | 0x0B)
#define EPMM4       ((BANK1 << BANK_SHIFT) | 0x0C)
#define EPMM5       ((BANK1 << BANK_SHIFT) | 0x0D)
#define EPMM6       ((BANK1 << BANK_SHIFT) | 0x0E)
#define EPMM7       ((BANK1 << BANK_SHIFT) | 0x0F)
#define EPMCSL      ((BANK1 << BANK_SHIFT) | 0x10)
#define EPMCSH      ((BANK1 << BANK_SHIFT) | 0x11)
#define EPMOL       ((BANK1 << BANK_SHIFT) | 0x14)
#define EPMOH       ((BANK1 << BANK_SHIFT) | 0x15)
#define ERXFCON     ((BANK1 << BANK_SHIFT) | 0x18)
#define EPKTCNT     ((BANK1 << BANK_SHIFT) | 0x19)


/* Bank 1 Registers */
//#define EPMM8       ((BANK1 << BANK_SHIFT) | 0x08)
//#define EPMM9       ((BANK1 << BANK_SHIFT) | 0x09)
//#define EPMM10      ((BANK1 << BANK_SHIFT) | 0x0A)
//#define EPMM11      ((BANK1 << BANK_SHIFT) | 0x0B)
//#define EPMM12      ((BANK1 << BANK_SHIFT) | 0x0C)
//#define EPMM13      ((BANK1 << BANK_SHIFT) | 0x0D)
//#define EPMM14      ((BANK1 << BANK_SHIFT) | 0x0E)
//#define EPMM15      ((BANK1 << BANK_SHIFT) | 0x0F)
//#define EPMCS2L     ((BANK1 << BANK_SHIFT) | 0x10)
//#define EPMCS2H     ((BANK1 << BANK_SHIFT) | 0x11)
//#define EPMO2L      ((BANK1 << BANK_SHIFT) | 0x12)
//#define EPMO2H      ((BANK1 << BANK_SHIFT) | 0x13)
//#define ERXFCON2    ((BANK1 << BANK_SHIFT) | 0x14)
//#define ERXFCON3    ((BANK1 << BANK_SHIFT) | 0x15)
//#define ERXFCON4    ((BANK1 << BANK_SHIFT) | 0x16)

#define EIE         ((BANK1 << BANK_SHIFT) | 0x1B)
#define EIR         ((BANK1 << BANK_SHIFT) | 0x1C)
//#define ESTATCLR    ((BANK1 << BANK_SHIFT) | 0x1A)
#define ESTAT       ((BANK1 << BANK_SHIFT) | 0x1D)
#define ECON2       ((BANK1 << BANK_SHIFT) | 0x1E)
#define ECON1       ((BANK1 << BANK_SHIFT) | 0x1F)

/* Bank 2 Registers */
#define MACON1      ((BANK2 << BANK_SHIFT) | 0x00)
#define MACON3      ((BANK2 << BANK_SHIFT) | 0x02)   // Note: MACON2 is reserved; MACON3 is at 0x02
#define MACON4      ((BANK2 << BANK_SHIFT) | 0x03)
#define MABBIPG     ((BANK2 << BANK_SHIFT) | 0x04)
#define MAIPGL      ((BANK2 << BANK_SHIFT) | 0x06)
#define MAIPGH      ((BANK2 << BANK_SHIFT) | 0x07)
#define MACLCON1    ((BANK2 << BANK_SHIFT) | 0x08)   // Retransmission Maximum
#define MACLCON2    ((BANK2 << BANK_SHIFT) | 0x09)   // Collision Window
#define MAMXFLL     ((BANK2 << BANK_SHIFT) | 0x0A)
#define MAMXFLH     ((BANK2 << BANK_SHIFT) | 0x0B)
#define MICMD       ((BANK2 << BANK_SHIFT) | 0x12)   // MII Command (correct address)
#define MIREGADR    ((BANK2 << BANK_SHIFT) | 0x14)   // MII Register Address
#define MIWRL       ((BANK2 << BANK_SHIFT) | 0x16)   // MII Write Data Low
#define MIWRH       ((BANK2 << BANK_SHIFT) | 0x17)   // MII Write Data High
#define MIRDL       ((BANK2 << BANK_SHIFT) | 0x18)   // MII Read Data Low
#define MIRDH       ((BANK2 << BANK_SHIFT) | 0x19)   // MII Read Data High

/* Bank 3 Registers (MAC address and others) */
#define MAADR5      ((BANK3 << BANK_SHIFT) | 0x00)   // MAC Address Byte 5 (MSB?)
#define MAADR6      ((BANK3 << BANK_SHIFT) | 0x01)   // MAC Address Byte 6 (LSB)
#define MAADR3      ((BANK3 << BANK_SHIFT) | 0x02)
#define MAADR4      ((BANK3 << BANK_SHIFT) | 0x03)
#define MAADR1      ((BANK3 << BANK_SHIFT) | 0x04)   // MAC Address Byte 1 (MSB)
#define MAADR2      ((BANK3 << BANK_SHIFT) | 0x05)
#define EBSTSD      ((BANK3 << BANK_SHIFT) | 0x06)
#define EBSTCON     ((BANK3 << BANK_SHIFT) | 0x07)
#define EBSTCSL     ((BANK3 << BANK_SHIFT) | 0x08)
#define EBSTCSH     ((BANK3 << BANK_SHIFT) | 0x09)
#define MISTAT      ((BANK3 << BANK_SHIFT) | 0x0A)
#define MIRESULT    ((BANK3 << BANK_SHIFT) | 0x0C)
#define EREVID      ((BANK3 << BANK_SHIFT) | 0x12)   // Revision ID
#define EPAUSL      ((BANK3 << BANK_SHIFT) | 0x18)
#define EPAUSH      ((BANK3 << BANK_SHIFT) | 0x19)

/*============================================================================
 * ECON1 Register Bits (Bank 1 - 0x1F) – corrected bank select bits
 *============================================================================*/
#define ECON1_TXRST         (1 << 7)
#define ECON1_RXRST         (1 << 6)
#define ECON1_CSUMEN         (1 << 4)
#define ECON1_RXEN          (1 << 2)
#define ECON1_TXRTS         (1 << 3)   /* same as BSEL1 */
#define ECON1_BSEL0         (1 << 0)
#define ECON1_BSEL1         (1 << 1)

/* Bank select values for ECON1 bits (1:0) */
#define ECON1_BSEL_BANK0    0x00
#define ECON1_BSEL_BANK1    0x01
#define ECON1_BSEL_BANK2    0x02
#define ECON1_BSEL_BANK3    0x03

/*============================================================================
 * ECON2 Register Bits (Bank 1 - 0x1E)
 *============================================================================*/
#define ECON2_AUTOINC       (1 << 7)
#define ECON2_PKTDEC        (1 << 6)
#define ECON2_PWRSV         (1 << 5)
#define ECON2_VRPS          (1 << 3)

/*============================================================================
 * EIE Register Bits (Bank 1 - 0x17)
 *============================================================================*/
#define EIE_INTIE           (1 << 7)
#define EIE_PKTIE           (1 << 6)
#define EIE_DMAIE           (1 << 5)
#define EIE_LINKIE          (1 << 4)
#define EIE_TXIE            (1 << 3)
#define EIE_WOLIE           (1 << 2)
#define EIE_TXERIE          (1 << 1)
#define EIE_RXERIE          (1 << 0)

/*============================================================================
 * EIR Register Bits (Bank 1 - 0x1B)
 *============================================================================*/
#define EIR_PKTIF           (1 << 6)
#define EIR_DMAIF           (1 << 5)
#define EIR_LINKIF          (1 << 4)
#define EIR_TXIF            (1 << 3)
#define EIR_WOLIF           (1 << 2)
#define EIR_TXERIF          (1 << 1)
#define EIR_RXERIF          (1 << 0)

/*============================================================================
 * ESTAT Register Bits (Bank 1 - 0x1D)
 *============================================================================*/
#define ESTAT_INT           (1 << 7)
#define ESTAT_BUFER         (1 << 6)
#define ESTAT_LATECOL       (1 << 4)
#define ESTAT_RXBUSY        (1 << 2)
#define ESTAT_TXABRT        (1 << 1)
#define ESTAT_CLKRDY        (1 << 0)

/*============================================================================
 * MACON1 Register Bits (Bank 2 - 0x00)
 *============================================================================*/
#define MACON1_TXPAUS       (1 << 3)
#define MACON1_RXPAUS       (1 << 2)
#define MACON1_PASSALL      (1 << 1)
#define MACON1_MARXEN       (1 << 0)

/*============================================================================
 * MACON2 Register Bits (Bank 2 - 0x01) – only these two exist
 *============================================================================*/
#define MACON2_MARST        (1 << 7)
#define MACON2_RNDRST       (1 << 6)

/*============================================================================
 * MACON3 Register Bits (Bank 2 - 0x02) – all other configuration bits
 *============================================================================*/
/* PADCFG2 is bit 5? Actually bits 5:7? Let's define clearly:
   PADCFG0 = bit0, PADCFG1 = bit1, PADCFG2 = bit5? No, according to datasheet,
   PADCFG2:0 are bits 7:5? Wait, register MACON3 has:
   bit 7:5 = PADCFG2:0, bit 4 = TXCRCEN, bit 3 = PHDREN, bit 2 = HFRMEN, bit 1 = FRMLNEN, bit 0 = FULDPX.
   Actually the datasheet says: bit7 = FULDPX, bit6 = FRMLNEN? Let's check the datasheet.
   From Register 6-2 (page 35 of PDF, page 37 of document): 
   MACON3: 
   bit 7-5: PADCFG2:0 (but they list them as PADCFG2, PADCFG1, PADCFG0)
   bit 4: TXCRCEN
   bit 3: PHDREN
   bit 2: HFRMEN
   bit 1: FRMLNEN
   bit 0: FULDPX
   So we need to define accordingly.
*/
#define MACON3_PADCFG2      (1 << 7)  // but that conflicts with FULDPX? Actually no, FULDPX is bit0.
// Let's re-do:
#define MACON3_FULDPX       (1 << 0)
#define MACON3_FRMLNEN      (1 << 1)
#define MACON3_HFRMEN       (1 << 2)
#define MACON3_PHDREN       (1 << 3)
#define MACON3_TXCRCEN      (1 << 4)
#define MACON3_PADCFG0      (1 << 5)  // Wait, bits 5-7 are PADCFG, but they are bits 5,6,7?
// Actually the datasheet says: bit 7-5 = PADCFG2:0, so PADCFG0 is bit5, PADCFG1 is bit6, PADCFG2 is bit7.
#define MACON3_PADCFG1      (1 << 6)
#define MACON3_PADCFG2      (1 << 7)

/*============================================================================
 * MACON4 Register Bits (Bank 2 - 0x03)
 *============================================================================*/
#define MACON4_DEFER        (1 << 6)
#define MACON4_BPEN         (1 << 5)
#define MACON4_NOBKOFF      (1 << 4)
#define MACON4_LONGPRE      (1 << 2)
#define MACON4_PUREPRE      (1 << 1)

/*============================================================================
 * MISTAT Register Bits (Bank 3 - 0x0A)
 *============================================================================*/
#define MISTAT_BUSY         (1 << 0)
#define MISTAT_SCAN         (1 << 1)
#define MISTAT_NVALID       (1 << 2)

/*============================================================================
 * MICMD Register Bits (Bank 2 - 0x12)
 *============================================================================*/
#define MICMD_MIIRD         (1 << 0)
#define MICMD_MIISCAN       (1 << 1)

/*============================================================================
 * ERXFCON Register Bits (Bank 0 - 0x2C)
 *============================================================================*/
#define ERXFCON_BCEN        (1 << 0)
#define ERXFCON_MCEN        (1 << 1)
#define ERXFCON_HTEN        (1 << 2)
#define ERXFCON_MPEN        (1 << 3)   // also called HSEN
#define ERXFCON_PMEN        (1 << 4)
#define ERXFCON_CRCEN       (1 << 5)
#define ERXFCON_ANDOR       (1 << 6)
#define ERXFCON_UCEN        (1 << 7)

/*============================================================================
 * PHY Registers
 *============================================================================*/
#define PHCON1              0x00
#define PHSTAT1             0x01
#define PHANA               0x02
#define PHANLPA             0x03
#define PHANE               0x04
#define PHCON2              0x10
#define PHSTAT2             0x11
#define PHSTAT3             0x1B
#define PHLCON           0x14

/* PHCON1 Bits */
#define PHCON1_PRST         (1 << 15)
#define PHCON1_PLOOPBK      (1 << 14)
#define PHCON1_PPWRSV       (1 << 11)
#define PHCON1_PDPXMD       (1 << 8)

/* PHCON2 Bits */
#define PHCON2_FRCLNK       (1 << 14)
#define PHCON2_TXDIS        (1 << 13)
#define PHCON2_JABBER       (1 << 10)
#define PHCON2_HDLDIS       (1 << 8)

/* PHSTAT1 Bits */
#define PHSTAT1_FULLDPLX    (1 << 2)
#define PHSTAT1_HALFDPLX    (1 << 3)
#define PHSTAT1_LLSTAT      (1 << 1)
#define PHSTAT1_JBSTAT      (1 << 0)
#define PHSTAT1_PFDPX       (1 << 12)

/* PHSTAT2 Bits */
#define PHSTAT2_LSTAT       (1 << 10)
#define PHSTAT2_COLSTAT     (1 << 9)
#define PHSTAT2_RXSTAT      (1 << 8)
#define PHSTAT2_TXSTAT      (1 << 3)
#define PHSTAT2_DPXSTAT     (1 << 5)
#define PHSTAT2_SPEED       (1 << 4)

/*============================================================================
 * Buffer Memory Layout
 *============================================================================*/
#define ENC28J60_RAM_SIZE           8192
#define ENC28J60_RX_BUFFER_START    0x0000  // start of RX buffer, (must be zero, Rev. B4 Errata point 5)
#define ENC28J60_RX_BUFFER_END      0x0BFF  // end of RX buffer, room for 2 packets
#define ENC28J60_TX_BUFFER_START    0x0C00  // start of TX buffer, room for 1 packet
#define ENC28J60_TX_BUFFER_END      0x1FB0  // end of TX buffer
#define ENC28J60_MAX_FRAMELEN       420
#define ENC28J60_TX_BUFFER_SIZE     0x0600

/*============================================================================
 * Packet Structure Definitions
 *============================================================================*/
#define ENC28J60_RX_HEADER_SIZE     6

/* RX Status Vector Offsets */
#define RX_STATUS_NEXT_PACKET_LOW   0
#define RX_STATUS_NEXT_PACKET_HIGH  1
#define RX_STATUS_LENGTH_LOW        2
#define RX_STATUS_LENGTH_HIGH       3
#define RX_STATUS_LENGTH_MASK       0x07FF
#define RX_STATUS_STATUS_LOW        4
#define RX_STATUS_STATUS_HIGH       5

/* RX Status Bits (Status Low Byte) */
#define RX_STAT_BCAST               (1 << 0)
#define RX_STAT_MCAST               (1 << 1)
#define RX_STAT_UCAST               (1 << 3)
#define RX_STAT_RANGE_ERR           (1 << 4)
#define RX_STAT_LENGTH_CHK_ERROR    (1 << 5)
#define RX_STAT_LONG_EVENT          (1 << 6)
#define RX_STAT_CRC_ERROR           (1 << 7)

/* RX Status Bits (Status High Byte) */
#define RX_STAT_RUNT                (1 << 0)
#define RX_STAT_HASH_MATCH          (1 << 1)
#define RX_STAT_HASH_TABLE_MATCH    (1 << 3)
#define RX_STAT_MAGIC_PACKET        (1 << 4)
#define RX_STAT_PAUSE_CONTROL_FRAME (1 << 5)
#define RX_STAT_RX_OK               (1 << 7)

/*============================================================================
 * TX Status Vector Elements
 *============================================================================*/
#define TSV_TX_BYTE_COUNT_L         0
#define TSV_TX_BYTE_COUNT_H         1
#define TSV_TX_COLLISION_COUNT_L    2
#define TSV_TX_COLLISION_COUNT_H    3
#define TSV_TX_CRC_BYTE_L           4
#define TSV_TX_CRC_BYTE_H           5

/*============================================================================
 * ENC28J60 Device Configuration Structure
 *============================================================================*/
typedef struct {
    uint8_t  mac_addr[6];
    bool     full_duplex;
    bool     auto_negotiation;
    uint16_t rx_buffer_start;
    uint16_t rx_buffer_end;
    uint16_t tx_buffer_start;
    uint16_t tx_buffer_end;
} ENC28J60_ConfigTypeDef;

/*============================================================================
 * ENC28J60 Handle Structure
 *============================================================================*/
typedef struct {
    SPI_TypeDef      *SPIx;
    GPIO_TypeDef     *CS_Port;
    uint16_t          CS_Pin;
    uint8_t           current_bank;
    uint8_t           mac_addr[6];
    // uint16_t          rx_buffer_start;
    // uint16_t          rx_buffer_end;
    // uint16_t          tx_buffer_start;
    // uint16_t          tx_buffer_end;
    // uint16_t          next_packet_ptr;
    // uint8_t           remaining_packets;
    bool              initialized;
} ENC28J60_HandleTypeDef;

typedef struct {
    uint16_t byte_count;         // Total bytes transmitted on wire
    uint8_t  collision_count:4;
    uint8_t  crc_error:1;
    uint8_t  length_check_error:1;
    uint8_t  length_out_of_range:1;
    uint8_t  done:1;
    uint8_t  multicast:1;
    uint8_t  broadcast:1;
    uint8_t  packet_defer:1;
    uint8_t  excessive_defer:1;
    uint8_t  excessive_collision:1;
    uint8_t  late_collision:1;
    uint8_t  giant:1;
    uint8_t  underrun:1;
    uint16_t bytes_on_wire;
} enc28j60_tsv_t;

/*============================================================================
 * Global Handle Declaration
 *============================================================================*/
extern ENC28J60_HandleTypeDef enc28j60_handle;

/*============================================================================
 * API - Initialization
 *============================================================================*/
void ENC28J60_Init(ENC28J60_ConfigTypeDef *config);
uint8_t ENC28J60_verify(void);
void ENC28J60_DeInit(void);
bool ENC28J60_verifyRead(uint8_t reg, uint8_t verifybyte);

/*============================================================================
 * API - Register Access (Internal use)
 *============================================================================*/
uint8_t  ENC28J60_ReadReg(uint8_t reg_addr);
void     ENC28J60_WriteReg(uint8_t reg_addr, uint8_t value);
void     ENC28J60_SetBitField(uint8_t reg_addr, uint8_t mask);
void     ENC28J60_ClearBitField(uint8_t reg_addr, uint8_t mask);
void     ENC28J60_ReadBuffer(uint8_t *buffer, uint16_t length);
void     ENC28J60_WriteBuffer(uint8_t *buffer, uint16_t length);

/*============================================================================
 * API - PHY Access
 *============================================================================*/
uint16_t ENC28J60_ReadPHY(uint8_t phy_reg);
void     ENC28J60_WritePHY(uint8_t phy_reg, uint16_t value);

/*============================================================================
 * API - MAC Address
 *============================================================================*/
void     ENC28J60_SetMACAddress(uint8_t *mac_addr);
void     ENC28J60_GetMACAddress(uint8_t *mac_addr);

/*============================================================================
 * API - Packet Transmit / Receive
 *============================================================================*/
void     ENC28J60_SendPacket(uint8_t *data, uint16_t length);
uint16_t ENC28J60_ReceivePacket(uint8_t *buffer, uint16_t max_length);
void     ENC28J60_DiscardRxPacket(void);
uint8_t  ENC28J60_GetPendingPacketCount(void);

struct pbuf* ENC28J60_packet_receive(void);

/*============================================================================
 * API - Interrupt Handling
 *============================================================================*/
void     ENC28J60_EnableInterrupts(uint8_t mask);
void     ENC28J60_DisableInterrupts(uint8_t mask);
uint8_t  ENC28J60_GetInterruptFlags(void);
void     ENC28J60_ClearInterruptFlags(uint8_t mask);

/*============================================================================
 * API - Link Status
 *============================================================================*/
bool     ENC28J60_IsLinkUp(void);
void     ENC28J60_GetPHYStatus(bool *link_up, bool *full_duplex);

/*============================================================================
 * API - Power / Reset
 *============================================================================*/
void     ENC28J60_WaitClk(void);
uint8_t  ENC28J60_GetRevision(void);

uint16_t getPackatLen(void);


#endif /* ENC28J60_H */
