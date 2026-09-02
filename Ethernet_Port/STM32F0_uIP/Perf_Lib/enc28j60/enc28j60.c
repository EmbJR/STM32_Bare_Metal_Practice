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
//#include "lwip/pbuf.h"
//#include "lwip/netif.h"

//#define DEBUG_LOOPBACK

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
static uint16_t NextPacketPtr;
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
    SPI_InitStruct.BaudRate      = SPI_BAUDRATEPRESCALER_4;
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
    for(int i = 0; i < 10; i++);
    GPIO_ResetPin(enc28j60_handle.CS_Port, enc28j60_handle.CS_Pin);
    for(int i = 0; i < 2; i++);
}

static void ENC28J60_CS_High(void)
{
	for(int i = 0; i < 2; i++);
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
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
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
    SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 1);
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
            SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
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
            SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
        }
        ENC28J60_CS_High();
    }
}

/*============================================================================
 * Read Buffer Memory
 *============================================================================*/
void ENC28J60_ReadBuffer(uint8_t *buffer, uint16_t length)
{
    ENC28J60_CS_Low();

    // Send the READ command (1 byte). Ignore the status byte returned.
    uint8_t cmd = ENC28J60_READ_BUF_MEM;
    uint8_t dummy_rx;
    SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, &cmd, &dummy_rx, 1);

    // Now read 'length' bytes. Each SPI transaction clocks exactly 1 byte.
    for (uint16_t i = 0; i < length; i++)
    {
        uint8_t tx_dummy = 0x00;
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, &tx_dummy, &buffer[i], 1);
    }

    ENC28J60_CS_High();
}

/*============================================================================
 * Write Buffer Memory
 *============================================================================*/
void ENC28J60_WriteBuffer(uint8_t *buffer, uint16_t length)
{
    ENC28J60_CS_Low();

    // Send the READ command (1 byte). Ignore the status byte returned.
    uint8_t cmd[2] = {ENC28J60_WRITE_BUF_MEM, 0};
    uint8_t dummy_rx[2] = {0};
    SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, cmd, &dummy_rx, 2);

    // Now read 'length' bytes. Each SPI transaction clocks exactly 1 byte.
    for (uint16_t i = 0; i < length; )
    {
        uint8_t rx_dummy[2] = {0x00, 0x00};
        if(SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, &buffer[i], rx_dummy, 1))
        {
            i++;
        }
        else
        {

        }
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
    {
		uint8_t opcode = (ECON1 & ADDR_MASK) | ENC28J60_BIT_FIELD_CLR;
		ENC28J60_CS_Low();
		{
			uint8_t tx[3] = { opcode, (0x03), 0x00 };
			uint8_t rx[3];
			SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
		}
		ENC28J60_CS_High();
	}
    {
		uint8_t opcode = (ECON1 & ADDR_MASK) | ENC28J60_BIT_FIELD_SET;
		ENC28J60_CS_Low();
		{
			uint8_t tx[3] = { opcode, bank, 0x00 };
			uint8_t rx[3];
			SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, tx, rx, 2);
		}
		ENC28J60_CS_High();
	}
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
void ENC28J60_WaitClk(void)
{
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
    //ENC28J60_SelectBank(BANK2);

    if (config->full_duplex)
    {
        ENC28J60_WriteReg(MACON1, MACON1_TXPAUS | MACON1_RXPAUS | MACON1_MARXEN);
        ENC28J60_WriteReg(MACON3, MACON3_PADCFG0 | MACON3_FRMLNEN | MACON3_FULDPX);
        ENC28J60_WriteReg(MACON4, MACON4_DEFER);
        ENC28J60_WriteReg(MABBIPG, 0x15);
    }
    else
    {
        ENC28J60_WriteReg(MACON1, MACON1_MARXEN | MACON1_RXPAUS | MACON1_TXPAUS);
        //ENC28J60_WriteReg(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
        ENC28J60_WriteReg(MACON3, MACON3_PADCFG0 | MACON3_FRMLNEN);
        ENC28J60_WriteReg(MACON4, MACON4_DEFER);
        ENC28J60_WriteReg(MABBIPG, 0x12);
    }

    ENC28J60_WriteReg(MAMXFLL, (uint8_t)(ENC28J60_MAX_FRAMELEN & 0xFF));
    ENC28J60_WriteReg(MAMXFLH, (uint8_t)((ENC28J60_MAX_FRAMELEN >> 8) & 0xFF));

    ENC28J60_WriteReg(MAIPGL, 0x12);
    ENC28J60_WriteReg(MAIPGH, 0x0C);

    uint8_t mactest = ENC28J60_ReadReg(MACON1);

    mactest = ENC28J60_ReadReg(MACON3);
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

#ifdef DEBUG_LOOPBACK
    ENC28J60_WritePHY(PHCON1, (ENC28J60_ReadPHY(PHCON1) | PHCON1_PLOOPBK));  // enable loopback
    ENC28J60_WritePHY(PHCON2, ENC28J60_ReadPHY(PHCON1) & ~PHCON2_HDLDIS);
#endif


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
        //ENC28J60_WritePHY(PHCON2, ENC28J60_ReadPHY(PHCON2) | PHCON2_FRCLNK);
    }
    ENC28J60_WritePHY(PHLCON, (uint16_t)((1<<1)|(1<<4)|(1<<9))); // led status
}

/*============================================================================
 * Buffer Memory Initialization
 *
 * Sets ERXST, ERXND, ERXRDPT, ETXST, ETXND, and enables RX.
 *============================================================================*/
static void ENC28J60_InitBuffers(void)
{

    //ENC28J60_SelectBank(BANK0);

    ENC28J60_WriteReg(ERXSTL,  (uint8_t)(encdevice.rx_buffer_start & 0xFF));
    ENC28J60_WriteReg(ERXSTH,  (uint8_t)((encdevice.rx_buffer_start >> 8) & 0xFF));

    ENC28J60_WriteReg(ERXNDL,  (uint8_t)(encdevice.rx_buffer_end & 0xFF));
    ENC28J60_WriteReg(ERXNDH,  (uint8_t)((encdevice.rx_buffer_end >> 8) & 0xFF));

    ENC28J60_WriteReg(ETXSTL,  (uint8_t)(encdevice.tx_buffer_start & 0xFF));
    ENC28J60_WriteReg(ETXSTH,  (uint8_t)((encdevice.tx_buffer_start >> 8) & 0xFF));

    ENC28J60_WriteReg(ETXNDL,  (uint8_t)(encdevice.tx_buffer_end & 0xFF));
    ENC28J60_WriteReg(ETXNDH,  (uint8_t)((encdevice.tx_buffer_end >> 8) & 0xFF));

    ENC28J60_WriteReg(ERDPTL, (uint8_t)(NextPacketPtr & 0xFF));
    ENC28J60_WriteReg(ERDPTH, (uint8_t)((NextPacketPtr >> 8) & 0xFF));

        /* Set write pointer to TX start */
    ENC28J60_WriteReg(EWRPTL, (uint8_t)(encdevice.tx_buffer_start & 0xFF));
    ENC28J60_WriteReg(EWRPTH, (uint8_t)((encdevice.tx_buffer_start >> 8) & 0xFF));
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
    const uint8_t ma_addr_regs[6] = { MAADR1, MAADR2, MAADR3, MAADR4, MAADR5, MAADR6 };

    for (i = 0; i < 6; i++)
    {
        encdevice.mac_addr[i] = mac_addr[i];
    }

    //ENC28J60_SelectBank(BANK3);
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

 static inline void ENC28J60_TxErrata_Fix(void) {
    // 1. Read ECON1 to check if TXRTS is stuck high
#if 1
//	  if (ENC28J60_ReadReg(EIR) & EIR_TXERIF) {
//         // 2. Errata fix: Force reset the transmit logic state machine
//         ENC28J60_SetBitField(ECON1, ECON1_TXRST);
//         ENC28J60_ClearBitField(ECON1, ECON1_TXRST);
//     }
// 1. Read ESTAT to check if an abort happened
    {
        // 2. Clear TXRTS bit just in case it's still latched
    	ENC28J60_ClearBitField(ECON1, ECON1_TXRTS);

        // 3. Reset the internal TX logic state machine
        ENC28J60_SetBitField(ECON1, ECON1_TXRST);
        ENC28J60_ClearBitField(ECON1, ECON1_TXRST);

        // 4. Clear the Transmit Abort and Error Flags
        ENC28J60_ClearBitField(ESTAT, ESTAT_TXABRT);
        ENC28J60_ClearBitField(EIR, EIR_TXERIF | EIR_TXIF);
    }
#endif
}

enc28j60_tsv_t tsvdata;
#if 1
void ENC28J60_SendPacket(uint8_t *data, uint16_t length)
{
	// 1. Wait for any previous transmission to clear
    while (ENC28J60_ReadReg(ECON1) & ECON1_TXRTS);

    // 2. Reset Write Pointer to start of TX Buffer (e.g., 0x1200)
    ENC28J60_WriteReg(EWRPTL, (uint8_t)(encdevice.tx_buffer_start & 0xFF));
    ENC28J60_WriteReg(EWRPTH, (uint8_t)((encdevice.tx_buffer_start >> 8) & 0xFF));

    // 3. Set TX Start Pointer
    ENC28J60_WriteReg(ETXSTL, (uint8_t)(encdevice.tx_buffer_start & 0xFF));
    ENC28J60_WriteReg(ETXSTH, (uint8_t)((encdevice.tx_buffer_start >> 8) & 0xFF));

    // 4. Set TX End Pointer (TXSTART + length of payload)
    // Note: Do NOT add extra offsets here!
    uint16_t tx_end = encdevice.tx_buffer_start + length;
    ENC28J60_WriteReg(ETXNDL, tx_end & 0xFF);
    ENC28J60_WriteReg(ETXNDH, tx_end >> 8);

//	    // 5. Write 1-byte Per-Packet Control Byte (0x00)
//	    uint8_t cb = 0x00;
//	    enc28j60_write_buf(1, &cb);

	if(length > 0)
    {
        //UART_SendStringIT(USART1, "\r\n---- Tx Data st----\r\n");

//        for(uint16_t cnt = 0; cnt < length; cnt++)
//        {
//            UART_SendDataIT(USART1, data[cnt]);
//            for(int i=0; i< 5000; i++);
//        }
//        //UART_SendStringIT(USART1, "\r\n--- Tx Data end---\r\n");
//        UART_SendDataIT(USART1, '\r');
//        UART_SendDataIT(USART1, '\n');

    }
    // 6. Write packet payload over SPI
    ENC28J60_WriteBuffer(data, length);

    // 7. Reset TX Logic (Silicon Errata B7 Fix)
    ENC28J60_SetBitField(ECON1, ECON1_TXRST);
    ENC28J60_ClearBitField(ECON1, ECON1_TXRST);
    ENC28J60_ClearBitField(EIR, EIR_TXERIF | EIR_TXIF);

    // 8. Start Transmission
    ENC28J60_SetBitField(ECON1, ECON1_TXRTS);

    Delay_ms(1);

    /* checking the transmit status */
    	// Set the read pointer to the start of the received packet
    ENC28J60_WriteReg(ERDPTL,(unsigned char)(tx_end + 1));
    ENC28J60_WriteReg(ERDPTH,(unsigned char)((tx_end + 1)>>8));

    /* Read the 6-byte RX status vector */
    ENC28J60_ReadBuffer((uint8_t *)&tsvdata, 7);

    if(tsvdata.bytes_on_wire < 1500)
    {
    	UART_SendStringIT(USART1, "\r\n- |) Tx success-\r\n");
    }
}
#endif

static uint16_t next_packet;
static uint8_t  pkt_count;
uint16_t getPackatLen(void)
{
    unsigned int rxstat;
    uint16_t packet_length;


    uint8_t  rx_header[ENC28J60_RX_HEADER_SIZE];
	pkt_count = ENC28J60_ReadReg(EPKTCNT);

	if (pkt_count == 0) return 0;

    ENC28J60_ClearBitField(ECON1, ECON1_RXEN);

	// Set the read pointer to the start of the received packet
	ENC28J60_WriteReg(ERDPTL,(unsigned char)(NextPacketPtr));
	ENC28J60_WriteReg(ERDPTH,(unsigned char)(NextPacketPtr>>8));

	/* Read the 6-byte RX status vector */
	ENC28J60_ReadBuffer((uint8_t *)rx_header, ENC28J60_RX_HEADER_SIZE);

	next_packet   = (uint16_t)rx_header[RX_STATUS_NEXT_PACKET_LOW];
	next_packet  |= (uint16_t)rx_header[RX_STATUS_NEXT_PACKET_HIGH] << 8;

	packet_length = (uint16_t)rx_header[RX_STATUS_LENGTH_LOW];
	packet_length |= (uint16_t)(rx_header[RX_STATUS_LENGTH_HIGH] & RX_STATUS_LENGTH_MASK) << 8;

	rxstat  = (unsigned int)rx_header[RX_STATUS_STATUS_LOW];
	rxstat |= (unsigned int)rx_header[RX_STATUS_STATUS_HIGH] << 8;


	if(((rxstat & 0x80) == 0) || (packet_length > ENC28J60_MAX_FRAMELEN) || (packet_length == 0))
	{
		// invalid
		packet_length=0;
        ENC28J60_DiscardRxPacket();
        ENC28J60_SetBitField(ECON1, ECON1_RXEN);
	}
	else
	{
		packet_length-=4;
	}

	return packet_length;
}

#if 1
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

		// copy the packet from the receive buffer
		if(max_length > 0)
		{
			ENC28J60_ReadBuffer(buffer, max_length);
			//-------------- for debugging -------------
			///UART_SendStringIT(USART1, "\r\n--- Rx Data --\r\n");
//			 for(uint16_t cnt = 0; cnt < packet_length; cnt++)
//			 {
//			 	UART_SendDataIT(USART1, buffer[cnt]);
//			 	for(int i=0; i< 5000; i++);
//			 }
//			 UART_SendDataIT(USART1, '\n');
//			 UART_SendDataIT(USART1, '\r');
			 //UART_SendStringIT(USART1, "\r\n--- Rx Data ends --\r\n");
			//------------------------------------------
		}

    if ((next_packet - 1 < ENC28J60_RX_BUFFER_START)
                || (next_packet -1 > ENC28J60_RX_BUFFER_END)) {
                ENC28J60_WriteReg(ERXRDPTL,  (uint8_t)(ENC28J60_RX_BUFFER_START & 0xFF));
                ENC28J60_WriteReg(ERXRDPTH,  (uint8_t)((ENC28J60_RX_BUFFER_START >> 8) & 0xFF));
                NextPacketPtr = ENC28J60_RX_BUFFER_START;
        } else {
                ENC28J60_WriteReg(ERXRDPTL,  (uint8_t)((next_packet - 1) & 0xFF));
                ENC28J60_WriteReg(ERXRDPTH,  (uint8_t)(((next_packet - 1) >> 8) & 0xFF));
                NextPacketPtr = next_packet;
        }

    ENC28J60_SetBitField(ECON1, ECON1_RXEN);        
    ENC28J60_SetBitField(ECON2, ECON2_PKTDEC);

    return max_length;
}
#endif

#if 0
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
    uint16_t packet_length;
    uint8_t  rx_status_high;
    uint16_t read_ptr;
    uint8_t  pkt_count;
    unsigned int rxstat;
    uint8_t econ1;
    uint8_t  rx_header[ENC28J60_RX_HEADER_SIZE];

    pkt_count = ENC28J60_ReadReg(EPKTCNT);

    if (pkt_count == 0) return 0;

	// Set the read pointer to the start of the received packet
    ENC28J60_WriteReg(ERDPTL,(unsigned char)(NextPacketPtr));
    ENC28J60_WriteReg(ERDPTH,(unsigned char)(NextPacketPtr>>8));

    /* Read the 6-byte RX status vector */
    ENC28J60_ReadBuffer((uint8_t *)rx_header, ENC28J60_RX_HEADER_SIZE);

    next_packet   = (uint16_t)rx_header[RX_STATUS_NEXT_PACKET_LOW];
    next_packet  |= (uint16_t)rx_header[RX_STATUS_NEXT_PACKET_HIGH] << 8;

    packet_length = (uint16_t)rx_header[RX_STATUS_LENGTH_LOW];
    packet_length |= (uint16_t)(rx_header[RX_STATUS_LENGTH_HIGH] & RX_STATUS_LENGTH_MASK) << 8;

    packet_length-=4;

	rxstat  = (unsigned int)rx_header[RX_STATUS_STATUS_LOW];
	rxstat |= (unsigned int)rx_header[RX_STATUS_STATUS_HIGH] << 8;


	if(((rxstat & 0x80) == 0) || (packet_length > ENC28J60_MAX_FRAMELEN))
	{
		// invalid
		packet_length=0;
	}
	else
	{
		// copy the packet from the receive buffer
		if(packet_length > 0)
		{
			ENC28J60_ReadBuffer(buffer, packet_length);
			//-------------- for debugging -------------
			UART_SendStringIT(USART1, "\r\n--- Rx Data --\r\n");
//			 for(uint16_t cnt = 0; cnt < packet_length; cnt++)
//			 {
//			 	UART_SendDataIT(USART1, buffer[cnt]);
//			 	for(int i=0; i< 5000; i++);
//			 }
//			 UART_SendDataIT(USART1, '\n');
//			 UART_SendDataIT(USART1, '\r');
			 UART_SendStringIT(USART1, "\r\n--- Rx Data ends --\r\n");
			//------------------------------------------
		}

	}

    if ((next_packet - 1 < ENC28J60_RX_BUFFER_START)
                || (next_packet -1 > ENC28J60_RX_BUFFER_END)) {
                ENC28J60_WriteReg(ERXRDPTL,  (uint8_t)(ENC28J60_RX_BUFFER_START & 0xFF));
                ENC28J60_WriteReg(ERXRDPTH,  (uint8_t)((ENC28J60_RX_BUFFER_START >> 8) & 0xFF));
                NextPacketPtr = ENC28J60_RX_BUFFER_START;
        } else {
                ENC28J60_WriteReg(ERXRDPTL,  (uint8_t)((next_packet - 1) & 0xFF));
                ENC28J60_WriteReg(ERXRDPTH,  (uint8_t)(((next_packet - 1) >> 8) & 0xFF));
                NextPacketPtr = next_packet;
        }

    ENC28J60_SetBitField(ECON2, ECON2_PKTDEC);


    return packet_length;
}
#endif

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

    //ENC28J60_SelectBank(BANK1);
    pkt_count = ENC28J60_ReadReg(EPKTCNT);
    if (pkt_count == 0) return;

    //ENC28J60_SelectBank(BANK0);

    read_ptr  = (uint16_t)ENC28J60_ReadReg(ERXRDPTL);
    read_ptr |= (uint16_t)ENC28J60_ReadReg(ERXRDPTH) << 8;

    ENC28J60_WriteReg(ERDPTL, (uint8_t)(read_ptr & 0xFF));
    ENC28J60_WriteReg(ERDPTH, (uint8_t)((read_ptr >> 8) & 0xFF));

    ENC28J60_ReadBuffer(header, 2);

    next_packet  = (uint16_t)header[0];
    next_packet |= (uint16_t)header[1] << 8;

    if (next_packet == 0)
    {
        next_packet = encdevice.rx_buffer_end;
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
    return ENC28J60_ReadReg(EIR);
}

/*============================================================================
 * Clear Interrupt Flags
 *============================================================================*/
void ENC28J60_ClearInterruptFlags(uint8_t mask)
{
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
    #ifdef DEBUG_LOOPBACK
    config->full_duplex = false;
    config->auto_negotiation = false;
    #endif

    ENC28J60_SPI_Init();

    Delay_ms(300);
    ENC28J60_SPI_SoftResetCmd();
     Delay_ms(30);
    ENC28J60_WaitClk();

    enc28j60_handle.current_bank = 0;
    ENC28J60_SetBitField(ECON2, ECON2_AUTOINC);

#if 1
    NextPacketPtr = ENC28J60_RX_BUFFER_START;
    uint8_t econ1 = ENC28J60_ReadReg(ECON1);

        /* Store config values */
    encdevice.rx_buffer_start = ENC28J60_RX_BUFFER_START;
    encdevice.rx_buffer_end   = ENC28J60_RX_BUFFER_END;
    encdevice.tx_buffer_start = ENC28J60_TX_BUFFER_START;
    encdevice.tx_buffer_end   = ENC28J60_TX_BUFFER_END;

    ENC28J60_InitBuffers();
            /* RX filter: accept broadcast and CRC-valid frames */
    /* Unicast frames matching local MAC are automatically accepted */

    /*-------- Receive Filters disabled ------*/
    ENC28J60_WriteReg(ERXFCON, ERXFCON_UCEN | ERXFCON_BCEN | ERXFCON_CRCEN | ERXFCON_MPEN);
    /*-------- Receive Filters (Bound to the specific MAC address) ------*/
    //ENC28J60_WriteReg(ERXFCON, ERXFCON_PMEN | ERXFCON_ANDOR | ERXFCON_CRCEN); // | ERXFCON_BCEN);
    // ENC28J60_WriteReg(EPMOL,  0x00);
    // ENC28J60_WriteReg(EPMOH,  0x00);

    // ENC28J60_WriteReg(EPMM0,  0xC0);
    // ENC28J60_WriteReg(EPMM1,  0x0F);
    // ENC28J60_WriteReg(EPMM2,  0x00);
    // ENC28J60_WriteReg(EPMM3,  0x00);
    // ENC28J60_WriteReg(EPMM4,  0x00);
    // ENC28J60_WriteReg(EPMM5,  0x00);
    // ENC28J60_WriteReg(EPMM6,  0x00);
    // ENC28J60_WriteReg(EPMM7,  0x00);    

    // ENC28J60_WriteReg(EPMCSL, 0x9C);
    // ENC28J60_WriteReg(EPMCSH, 0x7b);
    /*-------------------------------------------------------------------*/
    
    //ENC28J60_WriteReg(ERXFCON, 0x00);// | ERXFCON_MCEN);
    #ifdef DEBUG_LOOPBACK
        ENC28J60_WriteReg(ERXFCON, 0x00);
    #endif


    /*  as per microchip stack -------
    ENC28J60_WriteReg(EPMM0,  0x3F);
    ENC28J60_WriteReg(EPMM1,  0x30);

    ENC28J60_WriteReg(EPMCSL, 0xF9);
    ENC28J60_WriteReg(EPMCSH, 0xf7);
    */

    ENC28J60_InitMAC(config);

    ENC28J60_SetMACAddress(config->mac_addr);

    ENC28J60_WriteReg(ECON1, ECON1_RXEN);

    ENC28J60_InitPHY(config);

    // Automatically increment ERDPT and EWRPT when the SPI RBM/WBM command is used

    //ENC28J60_SelectBank(BANK1);
    ENC28J60_EnableInterrupts(EIE_PKTIE);
    ENC28J60_EnableInterrupts(EIE_INTIE);
#endif
    //ENC28J60_SelectBank(ECON1);

	/* pull transmitter and receiver out of reset */
	//ENC28J60_SetBitField(ECON1, ECON1_TXRST | ECON1_RXRST);

    //ENC28J60_ClearBitField(ECON1, ECON1_CSUMEN);
    	/* enable reception */
	
	econ1 = ENC28J60_ReadReg(ECON1);
    enc28j60_handle.initialized = true;

    //ENC28J60_ClearBitField(ESTAT, ESTAT_TXABRT);
}

void printval(uint8_t *strval, uint16_t value1)
{
	memset((char *)printdata, 0x00, sizeof(printdata));
	sprintf((char *)printdata, "%s: 0x%x\r\n", strval, value1);
	UART_SendStringIT(USART1, (const char *)printdata);
}

uint8_t ENC28J60_verify(void)
{
	uint8_t err = 0;
	const uint8_t ma_addr_regs[6] = { MAADR1, MAADR2, MAADR3, MAADR4, MAADR5, MAADR6 };
	uint16_t          rx_buffer_start = 0;
	uint16_t          rx_buffer_end = 0;
	uint16_t          tx_buffer_start = 0;
	uint16_t          tx_buffer_end = 0;
	uint16_t          next_packet_ptr = 0;

	for(uint8_t i=0; i<5; i++)
	{
		enc28j60_handle.mac_addr[i] = ENC28J60_ReadReg(ma_addr_regs[i]);
	}
	UART_SendStringIT(USART1, (const char *)"\n\r----------- Registers --------------\n\r");
	rx_buffer_start = ENC28J60_ReadReg(ERXSTL);
	rx_buffer_start |= (ENC28J60_ReadReg(ERXSTH) << 8);
	printval((uint8_t *)"ERXST", rx_buffer_start);

	rx_buffer_end = ENC28J60_ReadReg(ERXNDL);
	rx_buffer_end |= (ENC28J60_ReadReg(ERXNDH) << 8);
	printval((uint8_t *)"ERXND", rx_buffer_end);

	tx_buffer_start = ENC28J60_ReadReg(ETXSTL);
	tx_buffer_start |= (ENC28J60_ReadReg(ETXSTH) << 8);
	printval((uint8_t *)"ETXST", tx_buffer_start);

	tx_buffer_end = ENC28J60_ReadReg(ETXNDL);
	tx_buffer_end |= (ENC28J60_ReadReg(ETXNDH) << 8);
	printval((uint8_t *)"ETXND", tx_buffer_end);

	next_packet_ptr = ENC28J60_ReadReg(ERXRDPTL);
	next_packet_ptr |= (ENC28J60_ReadReg(ERXRDPTH) << 8);
	printval((uint8_t *)"ERXRDPT", next_packet_ptr);


    //----------------------- mac initialization ------------------------
//
	uint8_t MACON1x = ENC28J60_ReadReg(MACON1);
	printval((uint8_t *)"MACON1", MACON1x);

	uint8_t MACON3x = ENC28J60_ReadReg(MACON3);
	printval((uint8_t *)"MACON3", MACON3x);

	uint8_t MACON4x = ENC28J60_ReadReg(MACON4);
	printval((uint8_t *)"MACON4", MACON4x);

	uint8_t MABBIPGx = ENC28J60_ReadReg(MABBIPG);
	printval((uint8_t *)"MABBIPG", MABBIPGx);

	uint8_t MAMXFLLx = ENC28J60_ReadReg(MAMXFLL);
	printval((uint8_t *)"MAMXFLL", MAMXFLLx);

	uint8_t MAMXFLHx = ENC28J60_ReadReg(MAMXFLH);
	printval((uint8_t *)"MAMXFLH", MAMXFLHx);

	uint8_t MAIPGLx = ENC28J60_ReadReg(MAIPGL);
	printval((uint8_t *)"MAIPGL", MAIPGLx);

	uint8_t MAIPGHx = ENC28J60_ReadReg(MAIPGH);
	printval((uint8_t *)"MAIPGH", MAIPGHx);

    uint16_t phycon1 = ENC28J60_ReadPHY(PHCON1);		// 0x00
    printval((uint8_t *)"PHCON1", phycon1);

    uint16_t phycon2 = ENC28J60_ReadPHY(PHCON2);		// 0x100
    printval((uint8_t *)"PHCON2", phycon2);

    uint16_t phylcon = ENC28J60_ReadPHY(PHLCON);		// 0x212
    printval((uint8_t *)"PHLCON", phylcon);

    uint16_t phystat1 = ENC28J60_ReadPHY(PHSTAT1);		// 0x1800
    printval((uint8_t *)"PHSTAT1", phystat1);

    uint16_t phystat2 = ENC28J60_ReadPHY(PHSTAT2);		// 0x00
    printval((uint8_t *)"PHSTAT2", phystat2);

    uint8_t econ1 = ENC28J60_ReadReg(ECON1);			// 0x5
    printval((uint8_t *)"ECON1", econ1);

    uint8_t econ2 = ENC28J60_ReadReg(ECON2);			// 0x80
    printval((uint8_t *)"ECON2", econ2);

    uint8_t estat = ENC28J60_ReadReg(ESTAT);
    printval((uint8_t *)"ESTAT", estat);

    UART_SendStringIT(USART1, (const char *)"\n\r------------- End Registers ---------------\n\r");

    return 0;

}
