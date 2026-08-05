/**
 * @file    enc28j60.c
 * @brief   ENC28J60 Ethernet Controller Driver Implementation
 * @author  AI Generated
 * @date    2026-08-04
 *
 * @note    Driver for Microchip ENC28J60 Stand-Alone Ethernet Controller
 *          with SPI Interface. Uses the SPI_TransmitReceiveBuffer API
 *          for all SPI operations.
 */

#include <stdint.h>
#include <stdbool.h>
#include "enc28j60.h"
#include "spiF051.h"
#include "rcc.h"
#include "gpio.h"
#include "main.h"

/*============================================================================
 * Local Function Prototypes
 *============================================================================*/
static void ENC28J60_SelectBank(uint8_t bank);
static void ENC28J60_SPI_Init(void);
static void ENC28J60_GPIO_Init(void);
static void ENC28J60_CS_Low(void);
static void ENC28J60_CS_High(void);
static void ENC28J60_InitMAC(ENC28J60_ConfigTypeDef *config);
static void ENC28J60_InitPHY(ENC28J60_ConfigTypeDef *config);
static void ENC28J60_InitBuffers(void);

/*============================================================================
 * Global Handle Instance
 *============================================================================*/
ENC28J60_HandleTypeDef enc28j60_handle;

/*============================================================================
 * ENC28J60 SPI Initialization
 *============================================================================*/
static void ENC28J60_SPI_Init(void)
{
    SPI_InitTypeDef SPI_InitStruct;

    enc28j60_handle.SPIx = ENC28J60_SPI;
    enc28j60_handle.CS_Port = ENC28J60_GPIO_PORT;
    enc28j60_handle.CS_Pin = ENC28J60_CS_PIN;

    ENC28J60_GPIO_Init();

    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.Mode          = SPI_MODE_MASTER;
    SPI_InitStruct.BaudRate      = ENC28J60_SPI_CLOCK_PRESCALER;
    SPI_InitStruct.ClockPolarity = SPI_CPOL_LOW;
    SPI_InitStruct.ClockPhase    = SPI_CPHA_1EDGE;
    SPI_InitStruct.DataSize      = SPI_DATASIZE_8BIT;
    SPI_InitStruct.NSS           = SPI_NSS_SOFT;
    SPI_InitStruct.FrameFormat   = SPI_FRAME_FORMAT_MSBFIRST;
    SPI_InitStruct.CRC_Enable    = false;
    SPI_InitStruct.FIFOThreshold = true;

    RCC_EnablePeripheralClock(PERIPH_SPI2, 0);
    SPI_Init(enc28j60_handle.SPIx, &SPI_InitStruct);
    SPI_Enable(enc28j60_handle.SPIx);
}

/*============================================================================
 * ENC28J60 GPIO Initialization
 *============================================================================*/
static void ENC28J60_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_EnableClock(ENC28J60_GPIO_PORT);

    GPIO_InitStruct.Pin   = ENC28J60_CS_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Ot    = GPIO_OTYPE_PP;
    GPIO_InitStruct.Pull  = GPIO_PULL_NO;
    GPIO_Init(ENC28J60_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin   = ENC28J60_SCK_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Ot    = GPIO_OTYPE_PP;
    GPIO_InitStruct.Pull  = GPIO_PULL_NO;
    GPIO_InitStruct.AF    = GPIO_AF0;
    GPIO_Init(ENC28J60_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin   = ENC28J60_MISO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Ot    = GPIO_OTYPE_PP;
    GPIO_InitStruct.Pull  = GPIO_PULL_NO;
    GPIO_InitStruct.AF    = GPIO_AF0;
    GPIO_Init(ENC28J60_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin   = ENC28J60_MOSI_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Ot    = GPIO_OTYPE_PP;
    GPIO_InitStruct.Pull  = GPIO_PULL_NO;
    GPIO_InitStruct.AF    = GPIO_AF0;
    GPIO_Init(ENC28J60_GPIO_PORT, &GPIO_InitStruct);

    ENC28J60_CS_High();
}

/*============================================================================
 * Chip Select Control
 *============================================================================*/
static void ENC28J60_CS_Low(void)
{
    GPIO_ResetPin(enc28j60_handle.CS_Port, enc28j60_handle.CS_Pin);
}

static void ENC28J60_CS_High(void)
{
    GPIO_SetPin(enc28j60_handle.CS_Port, enc28j60_handle.CS_Pin);
}

/*============================================================================
 * SPI Command Dispatch
 *
 * Uses SPI_TransmitReceiveBuffer for every command sequence.
 *============================================================================*/

/**
 * @brief  Read ENC28J60 register via SPI, fixed to given bank.
 *         Does NOT perform bank switching - caller must ensure correct bank.
 */
static uint8_t ENC28J60_ReadRegRaw(uint8_t reg_addr)
{
    uint8_t opcode = (reg_addr & ADDR_MASK) | ENC28J60_READ_CTRL_REG;
    uint8_t result;

    ENC28J60_CS_Low();
    {
        uint8_t tx[3] = { opcode, 0x00, 0x00 };
        uint8_t rx[3];
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 3);
        result = rx[2];
    }
    ENC28J60_CS_High();
    return result;
}

/**
 * @brief  Write ENC28J60 register via SPI, fixed to given bank.
 *         Does NOT perform bank switching - caller must ensure correct bank.
 */
static void ENC28J60_WriteRegRaw(uint8_t reg_addr, uint8_t value)
{
    uint8_t opcode = (reg_addr & ADDR_MASK) | ENC28J60_WRITE_CTRL_REG;

    ENC28J60_CS_Low();
    {
        uint8_t tx[3] = { opcode, value, 0x00 };
        uint8_t rx[3];
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 3);
    }
    ENC28J60_CS_High();
}

/**
 * @brief  Execute a system reset command.
 *         The ENC28J60 datasheet notes that after SRC (0xFF),
 *         a dummy byte must be read back before de-asserting CS.
 */
static void ENC28J60_SPI_SoftResetCmd(void)
{
    uint8_t tx[3];
    uint8_t rx[3];

    ENC28J60_CS_Low();
    tx[0] = ENC28J60_SYSTEM_RESET_CMD;
    tx[1] = 0x00;
    tx[2] = 0x00;
    SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 3);
    ENC28J60_CS_High();
}

/*============================================================================
 * Register Read (with bank switching)
 *============================================================================*/
uint8_t ENC28J60_ReadReg(uint8_t reg_addr)
{
    uint8_t bank = (reg_addr & BANK_MASK) >> BANK_SHIFT;
    ENC28J60_SelectBank(bank);
    return ENC28J60_ReadRegRaw(reg_addr);
}

/*============================================================================
 * Register Write (with bank switching)
 *============================================================================*/
void ENC28J60_WriteReg(uint8_t reg_addr, uint8_t value)
{
    uint8_t bank = (reg_addr & BANK_MASK) >> BANK_SHIFT;
    ENC28J60_SelectBank(bank);
    ENC28J60_WriteRegRaw(reg_addr, value);
}

/*============================================================================
 * Bit Field Set (with bank switching)
 *============================================================================*/
void ENC28J60_SetBitField(uint8_t reg_addr, uint8_t mask)
{
    uint8_t bank = (reg_addr & BANK_MASK) >> BANK_SHIFT;
    ENC28J60_SelectBank(bank);

    {
        uint8_t opcode = (reg_addr & ADDR_MASK) | ENC28J60_BIT_FIELD_SET;
        ENC28J60_CS_Low();
        {
            uint8_t tx[3] = { opcode, mask, 0x00 };
            uint8_t rx[3];
            SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 3);
        }
        ENC28J60_CS_High();
    }
}

/*============================================================================
 * Bit Field Clear (with bank switching)
 *============================================================================*/
void ENC28J60_ClearBitField(uint8_t reg_addr, uint8_t mask)
{
    uint8_t bank = (reg_addr & BANK_MASK) >> BANK_SHIFT;
    ENC28J60_SelectBank(bank);

    {
        uint8_t opcode = (reg_addr & ADDR_MASK) | ENC28J60_BIT_FIELD_CLR;
        ENC28J60_CS_Low();
        {
            uint8_t tx[3] = { opcode, mask, 0x00 };
            uint8_t rx[3];
            SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 3);
        }
        ENC28J60_CS_High();
    }
}

/*============================================================================
 * Read Buffer Memory
 *============================================================================*/
void ENC28J60_ReadBuffer(uint8_t *buffer, uint16_t length)
{
    uint16_t i;

    if (length == 0) return;

    ENC28J60_CS_Low();

    {
        uint8_t tx[2];
        uint8_t rx[2];
        tx[0] = ENC28J60_READ_BUF_MEM;
        tx[1] = 0x00;
        rx[0] = 0x00; rx[1] = 0x00;
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
    }

    for (i = 0; i < length; i++)
    {
        uint8_t tx[2] = { 0x00, 0x00 };
        uint8_t rx[2] = { 0x00, 0x00 };
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
        buffer[i] = rx[1];
    }

    ENC28J60_CS_High();
}

/*============================================================================
 * Write Buffer Memory
 *============================================================================*/
void ENC28J60_WriteBuffer(uint8_t *buffer, uint16_t length)
{
    uint16_t i;

    if (length == 0) return;

    ENC28J60_CS_Low();

    {
        uint8_t tx[2];
        uint8_t rx[2];
        tx[0] = ENC28J60_WRITE_BUF_MEM;
        tx[1] = 0x00;
        rx[0] = 0x00; rx[1] = 0x00;
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
    }

    for (i = 0; i < length; i++)
    {
        uint8_t tx[2] = { buffer[i], 0x00 };
        uint8_t rx[2] = { 0x00, 0x00 };
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
    }

    ENC28J60_CS_High();
}

/*============================================================================
 * Bank Selection
 *
 * The ENC28J60 banks are selected via bits[1:0] and bit[7] of ECON1.
 * Switches only when the bank differs from current.
 *============================================================================*/
static void ENC28J60_SelectBank(uint8_t bank)
{
    uint8_t current_econ1;

    if (bank == enc28j60_handle.current_bank) return;

    current_econ1 = ENC28J60_ReadRegRaw(ECON1);

    current_econ1 &= ~(ECON1_BSEL0 | ECON1_BSEL1);

    switch (bank)
    {
    case BANK0:
        current_econ1 |= ECON1_BSEL_BANK0;
        break;
    case BANK1:
        current_econ1 |= ECON1_BSEL_BANK1;
        break;
    case BANK2:
        current_econ1 |= ECON1_BSEL_BANK2;
        break;
    case BANK3:
        current_econ1 |= ECON1_BSEL_BANK3;
        break;
    default:
        return;
    }

    ENC28J60_WriteRegRaw(ECON1, current_econ1);
    enc28j60_handle.current_bank = bank;
}

/*============================================================================
 * PHY Register Read
 *
 * Per ENC28J60 Datasheet:
 *   1. Write PHY register address to MIREGADR (Bank 2)
 *   2. Set MICMD.MIIRD bit (BUSY becomes set, Bank 2)
 *   3. Wait 10.24 us, then poll MISTAT.BUSY until cleared (Bank 3)
 *   4. Clear MICMD.MIIRD bit (Bank 2)
 *   5. Read result from MIRDL/MIRDH (Bank 2)
 *
 * NOTE: Do not read MIRDL/MIRDH before BUSY clears.
 *============================================================================*/
uint16_t ENC28J60_ReadPHY(uint8_t phy_reg)
{
    uint16_t value;

    ENC28J60_WriteReg(MIREGADR, phy_reg);
    ENC28J60_WriteReg(MICMD, MICMD_MIIRD);

    {
        volatile uint32_t timeout = 100000UL;
        uint8_t mistat;
        do {
            mistat = ENC28J60_ReadReg(MISTAT);
            timeout--;
        } while ((mistat & MISTAT_BUSY) && (timeout > 0));
    }

    ENC28J60_WriteReg(MICMD, 0x00);

    value  = (uint16_t)ENC28J60_ReadReg(MIRDL);
    value |= (uint16_t)ENC28J60_ReadReg(MIRDH) << 8;

    return value;
}

/*============================================================================
 * PHY Register Write
 *
 * MII Management Interface per ENC28J60 Datasheet:
 *   - Write PHY register address to MIREGADR (Bank 2)
 *   - Write low byte to MIWRL, high byte to MIWRH
 *   - Wait for MISTAT.BUSY to clear (polling Bank 3)
 *============================================================================*/
void ENC28J60_WritePHY(uint8_t phy_reg, uint16_t value)
{
    ENC28J60_WriteReg(MIREGADR, phy_reg);
    ENC28J60_WriteReg(MIWRL, (uint8_t)(value & 0xFF));
    ENC28J60_WriteReg(MIWRH, (uint8_t)((value >> 8) & 0xFF));

    {
        volatile uint32_t timeout = 100000UL;
        uint8_t mistat;
        do {
            mistat = ENC28J60_ReadReg(MISTAT);
            timeout--;
        } while ((mistat & MISTAT_BUSY) && (timeout > 0));
    }
}

/*============================================================================
 * Soft Reset
 *
 * Issues the system reset command and waits for CLKRDY.
 *============================================================================*/
void ENC28J60_SoftReset(void)
{
    ENC28J60_SPI_SoftResetCmd();

    {
        volatile uint32_t timeout = 100000UL;
        uint8_t estat;
        do {
            estat = ENC28J60_ReadReg(ESTAT);
            timeout--;
        } while (!(estat & ESTAT_CLKRDY) && (timeout > 0));
    }

    enc28j60_handle.current_bank = 0;
}

/*============================================================================
 * Get Silicon Revision
 *============================================================================*/
uint8_t ENC28J60_GetRevision(void)
{
    return ENC28J60_ReadReg(EREVID);
}

/*============================================================================
 * MAC Initialization
 *
 * Configures MACON1-4, MAMXFL, MABBIPG, MAIPG based on duplex setting.
 *============================================================================*/
static void ENC28J60_InitMAC(ENC28J60_ConfigTypeDef *config)
{
    ENC28J60_SelectBank(BANK2);

    if (config->full_duplex)
    {
        ENC28J60_WriteReg(MACON1, MACON1_TXPAUS | MACON1_RXPAUS | MACON1_MARXEN);
        ENC28J60_WriteReg(MACON3, MACON3_PADCFG_0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);
        ENC28J60_WriteReg(MACON4, MACON4_DEFER);
        ENC28J60_WriteReg(MABBIPG, 0x15);
    }
    else
    {
        ENC28J60_WriteReg(MACON1, MACON1_MARXEN);
        ENC28J60_WriteReg(MACON3, MACON3_PADCFG_0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
        ENC28J60_WriteReg(MACON4, MACON4_DEFER);
        ENC28J60_WriteReg(MABBIPG, 0x12);
    }

    ENC28J60_WriteReg(MAMXFLL, (uint8_t)(ENC28J60_MAX_FRAMELEN & 0xFF));
    ENC28J60_WriteReg(MAMXFLH, (uint8_t)((ENC28J60_MAX_FRAMELEN >> 8) & 0xFF));

    ENC28J60_WriteReg(MAIPGL, 0x12);
    ENC28J60_WriteReg(MAIPGH, 0x0C);

    if (config->full_duplex)
    {
        ENC28J60_WriteReg(MAIPGL, 0x12);   /* 4.1.2: IPG = 0x0C12 for FD */
    }
    else
    {
        ENC28J60_WriteReg(MAIPGL, 0x12);   /* HD: same base; B2B IPG handled by MABBIPG */
    }

    /* Back-to-back inter-packet gap */
    ENC28J60_WriteReg(MABBIPG, config->full_duplex ? 0x15 : 0x12);
}

/*============================================================================
 * PHY Initialization
 *
 * Performs auto-negotiation or forces link parameters.
 *============================================================================*/
static void ENC28J60_InitPHY(ENC28J60_ConfigTypeDef *config)
{
    uint16_t phy_val;
    volatile uint32_t timeout;

    if (config->full_duplex)
    {
        ENC28J60_WritePHY(PHCON1, PHCON1_PDPXMD);
    }
    else
    {
        ENC28J60_WritePHY(PHCON1, 0x0000);
    }

    ENC28J60_WritePHY(PHCON2, PHCON2_HDLDIS);

    if (config->auto_negotiation)
    {
        ENC28J60_WritePHY(PHCON1, ENC28J60_ReadPHY(PHCON1) | PHCON1_PRST);

        timeout = 100000UL;
        do {
            phy_val = ENC28J60_ReadPHY(PHSTAT2);
            timeout--;
        } while (!(phy_val & PHSTAT2_LSTAT) && (timeout > 0));
    }
    else
    {
        ENC28J60_WritePHY(PHCON1, ENC28J60_ReadPHY(PHCON1) & ~PHCON1_PRST);
    }

    if (!config->auto_negotiation)
    {
        ENC28J60_WritePHY(PHCON2, ENC28J60_ReadPHY(PHCON2) | PHCON2_FRCLNK);
    }
}

/*============================================================================
 * Buffer Memory Initialization
 *
 * Sets ERXST, ERXND, ERXRDPT, ETXST, ETXND, and enables RX.
 *============================================================================*/
static void ENC28J60_InitBuffers(void)
{
    ENC28J60_SelectBank(BANK0);

    ENC28J60_WriteReg(ERXSTL,  (uint8_t)(enc28j60_handle.rx_buffer_start & 0xFF));
    ENC28J60_WriteReg(ERXSTH,  (uint8_t)((enc28j60_handle.rx_buffer_start >> 8) & 0xFF));
    ENC28J60_WriteReg(ERXNDL,  (uint8_t)(enc28j60_handle.rx_buffer_end & 0xFF));
    ENC28J60_WriteReg(ERXNDH,  (uint8_t)((enc28j60_handle.rx_buffer_end >> 8) & 0xFF));
    ENC28J60_WriteReg(ERXRDPTL,(uint8_t)(enc28j60_handle.rx_buffer_start & 0xFF));
    ENC28J60_WriteReg(ERXRDPTH,(uint8_t)((enc28j60_handle.rx_buffer_start >> 8) & 0xFF));

    ENC28J60_WriteReg(ETXSTL,  (uint8_t)(enc28j60_handle.tx_buffer_start & 0xFF));
    ENC28J60_WriteReg(ETXSTH,  (uint8_t)((enc28j60_handle.tx_buffer_start >> 8) & 0xFF));
    ENC28J60_WriteReg(ETXNDL,  (uint8_t)(enc28j60_handle.tx_buffer_start & 0xFF));
    ENC28J60_WriteReg(ETXNDH,  (uint8_t)((enc28j60_handle.tx_buffer_start >> 8) & 0xFF));

    /* Set read pointer */
    ENC28J60_WriteReg(ERXRDPTL, (uint8_t)(enc28j60_handle.rx_buffer_start & 0xFF));
    ENC28J60_WriteReg(ERXRDPTH, (uint8_t)((enc28j60_handle.rx_buffer_start >> 8) & 0xFF));

    /* Enable RX */
    ENC28J60_SetBitField(ECON1, ECON1_RXEN);
}

/*============================================================================
 * Set MAC Address
 *
 * Writes the 6-byte MAC into MAADR0-5 (non-banked, access from Bank 2).
 * Also stores into the handle.
 *============================================================================*/
void ENC28J60_SetMACAddress(uint8_t *mac_addr)
{
    uint8_t i;
    const uint8_t ma_addr_regs[6] = { MAADR0, MAADR1, MAADR2, MAADR3, MAADR4, MAADR5 };

    for (i = 0; i < 6; i++)
    {
        enc28j60_handle.mac_addr[i] = mac_addr[i];
    }

    ENC28J60_SelectBank(BANK2);
    for (i = 0; i < 6; i++)
    {
        ENC28J60_WriteReg(ma_addr_regs[i], mac_addr[i]);
    }
}

/*============================================================================
 * Get MAC Address
 *============================================================================*/
void ENC28J60_GetMACAddress(uint8_t *mac_addr)
{
    uint8_t i;
    for (i = 0; i < 6; i++)
    {
        mac_addr[i] = enc28j60_handle.mac_addr[i];
    }
}

/*============================================================================
 * Send Packet
 *
 * Per ENC28J60 Datasheet:
 *   1. Set EWRPT to TX buffer start
 *   2. Write per-packet control byte (0x00) + frame data via WBM
 *   3. Set ETXST and ETXND to define the TX range
 *   4. Set ECON1.TXRTS to start transmission
 *   5. Wait for TXRTS to clear or EIR.TXIF / EIR.TXERIF
 *   6. Optionally read TSV at ETXND + 1 (7 bytes)
 *============================================================================*/
void ENC28J60_SendPacket(uint8_t *data, uint16_t length)
{
    uint16_t tx_end;
    uint16_t i;
    uint8_t  estat_val;

    if (length > ENC28J60_MAX_FRAMELEN) return;

    ENC28J60_SelectBank(BANK0);

    tx_end = enc28j60_handle.tx_buffer_start + length;

    /* Set write pointer to TX start */
    ENC28J60_WriteReg(EWRPTL, (uint8_t)(enc28j60_handle.tx_buffer_start & 0xFF));
    ENC28J60_WriteReg(EWRPTH, (uint8_t)((enc28j60_handle.tx_buffer_start >> 8) & 0xFF));

    /* Set TX start and end boundaries */
    ENC28J60_WriteReg(ETXSTL, (uint8_t)(enc28j60_handle.tx_buffer_start & 0xFF));
    ENC28J60_WriteReg(ETXSTH, (uint8_t)((enc28j60_handle.tx_buffer_start >> 8) & 0xFF));
    ENC28J60_WriteReg(ETXNDL, (uint8_t)(tx_end & 0xFF));
    ENC28J60_WriteReg(ETXNDH, (uint8_t)((tx_end >> 8) & 0xFF));

    /* Single CS assertion: WBM opcode, per-packet control byte, then data */
    ENC28J60_CS_Low();

    /* WBM opcode */
    {
        uint8_t tx[2] = { ENC28J60_WRITE_BUF_MEM, 0x00 };
        uint8_t rx[2];
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
    }

    /* Per-packet control byte: 0x00 = use MACON3 settings */
    {
        uint8_t tx[2] = { 0x00, 0x00 };
        uint8_t rx[2];
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
    }

    /* Frame data */
    for (i = 0; i < length; i++)
    {
        uint8_t tx[2] = { data[i], 0x00 };
        uint8_t rx[2];
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
    }

    /* Dummy byte to ensure last data byte is committed */
    {
        uint8_t tx[2] = { 0x00, 0x00 };
        uint8_t rx[2];
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
    }

    ENC28J60_CS_High();

    /* Trigger transmission */
    ENC28J60_SelectBank(BANK1);
    ENC28J60_SetBitField(ECON1, ECON1_TXRTS);

    /* Wait for completion: TXRTS self-clears when done, or check TXIF/TXERIF */
    {
        volatile uint32_t timeout = 100000000UL;
        uint8_t econ1_val;
        uint8_t eir_val;
        do {
            econ1_val = ENC28J60_ReadReg(ECON1);
            if (!(econ1_val & ECON1_TXRTS)) break;
            eir_val = ENC28J60_ReadReg(EIR);
            if (eir_val & (EIR_TXIF | EIR_TXERIF)) break;
            timeout--;
        } while (timeout > 0);
    }

    /* Clear TX interrupt flags */
    ENC28J60_ClearInterruptFlags(EIR_TXIF | EIR_TXERIF);
}

/*============================================================================
 * Receive Packet
 *
 * Reads the next pending packet from the RX buffer.
 * Returns the number of bytes read (data payload only).
 * The caller must provide a buffer large enough for max_length bytes.
 *
 * Returns 0 if no valid packet was found.
 *============================================================================*/
uint16_t ENC28J60_ReceivePacket(uint8_t *buffer, uint16_t max_length)
{
    uint8_t  rx_header[ENC28J60_RX_HEADER_SIZE];
    uint16_t next_packet;
    uint16_t packet_length;
    uint8_t  rx_status_high;
    uint16_t read_ptr;
    uint8_t  pkt_count;

    ENC28J60_SelectBank(BANK1);
    pkt_count = ENC28J60_ReadReg(EPKTCNT);

    if (pkt_count == 0) return 0;

    ENC28J60_SelectBank(BANK0);

    /* Get the read pointer */
    read_ptr  = (uint16_t)ENC28J60_ReadReg(ERXRDPTL);
    read_ptr |= (uint16_t)ENC28J60_ReadReg(ERXRDPTH) << 8;

    /* Set read pointer to start of RX packet data */
    ENC28J60_WriteReg(ERDPTL, (uint8_t)(read_ptr & 0xFF));
    ENC28J60_WriteReg(ERDPTH, (uint8_t)((read_ptr >> 8) & 0xFF));

    /* Read the 6-byte RX status vector */
    ENC28J60_ReadBuffer(rx_header, ENC28J60_RX_HEADER_SIZE);

    next_packet   = (uint16_t)rx_header[RX_STATUS_NEXT_PACKET_LOW];
    next_packet  |= (uint16_t)rx_header[RX_STATUS_NEXT_PACKET_HIGH] << 8;

    packet_length = (uint16_t)rx_header[RX_STATUS_LENGTH_LOW];
    packet_length |= (uint16_t)(rx_header[RX_STATUS_LENGTH_HIGH] & RX_STATUS_LENGTH_MASK) << 8;

    rx_status_high = rx_header[RX_STATUS_STATUS_HIGH];

    /* Validate RX status */
    if ((rx_status_high & RX_STAT_RX_OK) == 0)
    {
        ENC28J60_WriteReg(ERXRDPTL, (uint8_t)(next_packet & 0xFF));
        ENC28J60_WriteReg(ERXRDPTH, (uint8_t)((next_packet >> 8) & 0xFF));
        ENC28J60_SetBitField(ECON2, ECON2_PKTDEC);
        return 0;
    }

    if (packet_length > max_length)
    {
        packet_length = max_length;
    }

    if (packet_length > 0)
    {
        ENC28J60_ReadBuffer(buffer, packet_length);
    }

    /* Advance RX read pointer (handle wrap-around) */
    if (next_packet == 0)
    {
        next_packet = enc28j60_handle.rx_buffer_end;
    }

    ENC28J60_WriteReg(ERXRDPTL, (uint8_t)(next_packet & 0xFF));
    ENC28J60_WriteReg(ERXRDPTH, (uint8_t)((next_packet >> 8) & 0xFF));

    ENC28J60_SetBitField(ECON2, ECON2_PKTDEC);

    return packet_length;
}

/*============================================================================
 * Discard Current RX Packet
 *
 * Frees the current packet without reading its payload.
 * Advances ERXRDPT to the next packet and decrements PKTCNT.
 *============================================================================*/
void ENC28J60_DiscardRxPacket(void)
{
    uint8_t header[ENC28J60_RX_HEADER_SIZE];
    uint16_t next_packet;
    uint16_t read_ptr;
    uint8_t  pkt_count;

    ENC28J60_SelectBank(BANK1);
    pkt_count = ENC28J60_ReadReg(EPKTCNT);
    if (pkt_count == 0) return;

    ENC28J60_SelectBank(BANK0);

    read_ptr  = (uint16_t)ENC28J60_ReadReg(ERXRDPTL);
    read_ptr |= (uint16_t)ENC28J60_ReadReg(ERXRDPTH) << 8;

    ENC28J60_WriteReg(ERDPTL, (uint8_t)(read_ptr & 0xFF));
    ENC28J60_WriteReg(ERDPTH, (uint8_t)((read_ptr >> 8) & 0xFF));

    ENC28J60_ReadBuffer(header, 2);

    next_packet  = (uint16_t)header[0];
    next_packet |= (uint16_t)header[1] << 8;

    if (next_packet == 0)
    {
        next_packet = enc28j60_handle.rx_buffer_end;
    }

    ENC28J60_WriteReg(ERXRDPTL, (uint8_t)(next_packet & 0xFF));
    ENC28J60_WriteReg(ERXRDPTH, (uint8_t)((next_packet >> 8) & 0xFF));

    ENC28J60_SetBitField(ECON2, ECON2_PKTDEC);
}

/*============================================================================
 * Get Pending Packet Count
 *============================================================================*/
uint8_t ENC28J60_GetPendingPacketCount(void)
{
    ENC28J60_SelectBank(BANK1);
    return ENC28J60_ReadReg(EPKTCNT);
}

/*============================================================================
 * Interrupt Enable
 *============================================================================*/
void ENC28J60_EnableInterrupts(uint8_t mask)
{
    ENC28J60_SetBitField(EIE, mask);
}

/*============================================================================
 * Interrupt Disable
 *============================================================================*/
void ENC28J60_DisableInterrupts(uint8_t mask)
{
    ENC28J60_ClearBitField(EIE, mask);
}

/*============================================================================
 * Get Interrupt Flags
 *============================================================================*/
uint8_t ENC28J60_GetInterruptFlags(void)
{
    ENC28J60_SelectBank(BANK1);
    return ENC28J60_ReadReg(EIR);
}

/*============================================================================
 * Clear Interrupt Flags
 *============================================================================*/
void ENC28J60_ClearInterruptFlags(uint8_t mask)
{
    ENC28J60_SelectBank(BANK1);
    ENC28J60_ClearBitField(EIR, mask);
}

/*============================================================================
 * Check Link Status
 *============================================================================*/
bool ENC28J60_IsLinkUp(void)
{
    uint16_t phstat2 = ENC28J60_ReadPHY(PHSTAT2);
    return (phstat2 & PHSTAT2_LSTAT) ? true : false;
}

/*============================================================================
 * Get PHY Status (Link + Duplex)
 *============================================================================*/
void ENC28J60_GetPHYStatus(bool *link_up, bool *full_duplex)
{
    uint16_t phstat1 = ENC28J60_ReadPHY(PHSTAT1);
    uint16_t phstat2 = ENC28J60_ReadPHY(PHSTAT2);

    *link_up     = (phstat2 & PHSTAT2_LSTAT) ? true : false;
    *full_duplex = (phstat1 & PHSTAT1_PFDPX) ? true : false;
}

/*============================================================================
 * De-Initialize
 *============================================================================*/
void ENC28J60_DeInit(void)
{
    SPI_Disable(enc28j60_handle.SPIx);
    SPI_DeInit(enc28j60_handle.SPIx);
    GPIO_DeInit(enc28j60_handle.CS_Port);
    enc28j60_handle.initialized = false;
}

/*============================================================================
 * High-Level Initialization
 *
 *  1. SPI + GPIO init.
 *  2. Soft-reset ENC28J60.
 *  3. Configure RX buffer filter.
 *  4. Set MAC address.
 *  5. Initialize PHY.
 *  6. Initialize MAC.
 *  7. Initialize buffer pointers.
 *  8. Enable RX.
 *============================================================================*/
void ENC28J60_Init(ENC28J60_ConfigTypeDef *config)
{
    ENC28J60_SPI_Init();

    ENC28J60_SoftReset();
    Delay_ms(10);

    enc28j60_handle.current_bank = 0;

    ENC28J60_SelectBank(BANK0);

#if 1
    /* RX filter: accept broadcast and CRC-valid frames */
    /* Unicast frames matching local MAC are automatically accepted */
    ENC28J60_WriteReg(ERXFCON, ERXFCON_BCEN | ERXFCON_CRCEN);

    ENC28J60_WriteReg(EPMM0,  0x3F);
    ENC28J60_WriteReg(EPMM1,  0x30);

    ENC28J60_WriteReg(EPMCSL, 0xF9);
    ENC28J60_WriteReg(EPMCSH, 0xBE);

    ENC28J60_WriteReg(EPMOL,  0x00);
    ENC28J60_WriteReg(EPMOH,  0x40);

    /* Store config values */
    enc28j60_handle.rx_buffer_start = ENC28J60_RX_BUFFER_START;
    enc28j60_handle.rx_buffer_end   = ENC28J60_RX_BUFFER_END;
    enc28j60_handle.tx_buffer_start = ENC28J60_TX_BUFFER_START;
    enc28j60_handle.tx_buffer_end   = ENC28J60_TX_BUFFER_END;

    if (config->rx_buffer_start != 0)
        enc28j60_handle.rx_buffer_start = config->rx_buffer_start;
    if (config->rx_buffer_end != 0)
        enc28j60_handle.rx_buffer_end   = config->rx_buffer_end;
    if (config->tx_buffer_start != 0)
        enc28j60_handle.tx_buffer_start = config->tx_buffer_start;
    if (config->tx_buffer_end != 0)
        enc28j60_handle.tx_buffer_end   = config->tx_buffer_end;

    ENC28J60_SetMACAddress(config->mac_addr);

    ENC28J60_InitPHY(config);
    ENC28J60_InitMAC(config);
    ENC28J60_InitBuffers();

    ENC28J60_SelectBank(BANK1);
    ENC28J60_EnableInterrupts(EIE_PKTIE);
#endif
    enc28j60_handle.initialized = true;
}
