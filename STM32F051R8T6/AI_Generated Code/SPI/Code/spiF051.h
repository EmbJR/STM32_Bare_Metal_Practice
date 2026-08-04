/**
 * @file    spiF051.h
 * @brief   SPI Driver for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-03-02
 */

#ifndef SPIF051_H
#define SPIF051_H

#include <stdint.h>
#include <stdbool.h>

/*============================================================================
 * SPI Base Addresses
 *============================================================================*/
#define SPI1_BASE    (0x40013000UL)  /*!< SPI1 base address (APB2) */
#define SPI2_BASE    (0x40003800UL)  /*!< SPI2 base address (APB1) */

/*============================================================================
 * SPI Register Structure
 *============================================================================*/
typedef struct {
    volatile uint32_t CR1;       /*!< Control Register 1,         Address offset: 0x00 */
    volatile uint32_t CR2;       /*!< Control Register 2,         Address offset: 0x04 */
    volatile uint32_t SR;        /*!< Status Register,            Address offset: 0x08 */
    volatile uint32_t DR;        /*!< Data Register,              Address offset: 0x0C */
    volatile uint32_t CRCPR;     /*!< CRC Polynomial Register,    Address offset: 0x10 */
    volatile uint32_t RXCRCR;    /*!< RX CRC Register,            Address offset: 0x14 */
    volatile uint32_t TXCRCR;    /*!< TX CRC Register,            Address offset: 0x18 */
    volatile uint32_t I2SCFGR;    /*!< I2S Configuration Register, Address offset: 0x1C */
} SPI_TypeDef;

/* SPI Base Address Macros */
#define SPI1    ((SPI_TypeDef *)SPI1_BASE)
#define SPI2    ((SPI_TypeDef *)SPI2_BASE)

/*============================================================================
 * SPI Register Bit Definitions
 *============================================================================*/

/* CR1 Register Bits */
#define SPI_CR1_CPHA       (0x0001UL)  /*!< Clock Phase */
#define SPI_CR1_CPOL       (0x0002UL)  /*!< Clock Polarity */
#define SPI_CR1_MSTR       (0x0004UL)  /*!< Master Selection */
#define SPI_CR1_BR         (0x0038UL)  /*!< Baud Rate Control [2:0] */
#define SPI_CR1_BR_0       (0x0008UL)  /*!< Bit 0 */
#define SPI_CR1_BR_1       (0x0010UL)  /*!< Bit 1 */
#define SPI_CR1_BR_2       (0x0020UL)  /*!< Bit 2 */
#define SPI_CR1_SPE        (0x0040UL)  /*!< SPI Enable */
#define SPI_CR1_LSBFIRST   (0x0080UL)  /*!< Frame Format */
#define SPI_CR1_SSI        (0x0100UL)  /*!< Internal Slave Select */
#define SPI_CR1_SSM        (0x0200UL)  /*!< Software Slave Management */
#define SPI_CR1_RXONLY     (0x0400UL)  /*!< Receive Only */
#define SPI_CR1_DFF        (0x0800UL)  /*!< Data Frame Format (0=8bit, 1=16bit) - Not in F0 */
#define SPI_CR1_CRCNEXT    (0x1000UL)  /*!< CRC Transfer Next */
#define SPI_CR1_CRCEN      (0x2000UL)  /*!< Hardware CRC Enable */
#define SPI_CR1_BIDIMODE   (0x4000UL)  /*!< Bidirectional Data Mode */
#define SPI_CR1_BIDIOE     (0x8000UL)  /*!< Bidirectional Output Enable */

/* CR2 Register Bits */
#define SPI_CR2_RXDMAEN    (0x0001UL)  /*!< RX Buffer DMA Enable */
#define SPI_CR2_TXDMAEN    (0x0002UL)  /*!< TX Buffer DMA Enable */
#define SPI_CR2_SSOE       (0x0004UL)  /*!< SS Output Enable */
#define SPI_CR2_ERRIE      (0x0020UL)  /*!< Error Interrupt Enable */
#define SPI_CR2_RXNEIE     (0x0040UL)  /*!< RX Buffer Not Empty Interrupt Enable */
#define SPI_CR2_TXEIE      (0x0080UL)  /*!< TX Buffer Empty Interrupt Enable */
#define SPI_CR2_DS         (0x0F00UL)  /*!< Data Size [3:0] */
#define SPI_CR2_DS_0       (0x0100UL)  /*!< Bit 0 */
#define SPI_CR2_DS_1       (0x0200UL)  /*!< Bit 1 */
#define SPI_CR2_DS_2       (0x0400UL)  /*!< Bit 2 */
#define SPI_CR2_DS_3       (0x0800UL)  /*!< Bit 3 */
#define SPI_CR2_FRXTH      (0x1000UL)  /*!< FIFO Reception Threshold */
#define SPI_CR2_LDMA_RX    (0x2000UL)  /*!< Last DMA Transfer for Reception */
#define SPI_CR2_LDMA_TX    (0x4000UL)  /*!< Last DMA Transfer for Transmission */
#define SPI_CR2_NSSP       (0x8000UL)  /*!< NSS Pulse Management */
#define SPI_CR2_FRFT       (0x1000UL)  /*!< FIFO Reception Threshold (alternative) */
#define SPI_CR2_TXFIFO      (0x0000UL)  /*!< TX FIFO */
#define SPI_CR2_RXFIFO      (0x0000UL)  /*!< RX FIFO */

/* SR Register Bits */
#define SPI_SR_RXNE        (0x0001UL)  /*!< Receive Buffer Not Empty */
#define SPI_SR_TXE         (0x0002UL)  /*!< Transmit Buffer Empty */
#define SPI_SR_CHSIDE      (0x0004UL)  /*!< Channel Side - Not used in SPI mode */
#define SPI_SR_UDR         (0x0008UL)  /*!< Underrun Flag - Not in F0 */
#define SPI_SR_CRCERR      (0x0010UL)  /*!< CRC Error Flag */
#define SPI_SR_MODF        (0x0020UL)  /*!< Mode Fault */
#define SPI_SR_OVR         (0x0040UL)  /*!< Overrun Flag */
#define SPI_SR_BSY         (0x0080UL)  /*!< Busy Flag */
#define SPI_SR_FRE         (0x0100UL)  /*!< Frame Format Error - Not in F0 */
#define SPI_SR_FRLVL       (0x0600UL)  /*!< FIFO Reception Level [1:0] */
#define SPI_SR_FTLVL       (0x1800UL)  /*!< FIFO Transmission Level [1:0] */

/*============================================================================
 * SPI Configuration Enumerations
 *============================================================================*/

/* SPI Mode */
typedef enum {
    SPI_MODE_SLAVE = 0,
    SPI_MODE_MASTER = 1
} SPI_ModeTypeDef;

/* Clock Polarity */
typedef enum {
    SPI_CPOL_LOW = 0,    /*!< Clock polarity low when idle */
    SPI_CPOL_HIGH = 1    /*!< Clock polarity high when idle */
} SPI_CPOLTypeDef;

/* Clock Phase */
typedef enum {
    SPI_CPHA_1EDGE = 0,  /*!< First clock transition is first data capture edge */
    SPI_CPHA_2EDGE = 1   /*!< Second clock transition is first data capture edge */
} SPI_CPHATypeDef;

/* Data Direction */
typedef enum {
    SPI_DIRECTION_2LINES_FULLDUPLEX = 0x0000UL,
    SPI_DIRECTION_2LINES_RXONLY = 0x0400UL,
    SPI_DIRECTION_1LINE_TX = 0x4000UL,
    SPI_DIRECTION_1LINE_RX = 0xC000UL
} SPI_DirectionTypeDef;

/* Data Size (DS bits in CR2) */
typedef enum {
    SPI_DATASIZE_4BIT  = 0x0300UL,
    SPI_DATASIZE_5BIT  = 0x0400UL,
    SPI_DATASIZE_6BIT  = 0x0500UL,
    SPI_DATASIZE_7BIT  = 0x0600UL,
    SPI_DATASIZE_8BIT  = 0x0700UL,
    SPI_DATASIZE_9BIT  = 0x0800UL,
    SPI_DATASIZE_10BIT = 0x0900UL,
    SPI_DATASIZE_11BIT = 0x0A00UL,
    SPI_DATASIZE_12BIT = 0x0B00UL,
    SPI_DATASIZE_13BIT = 0x0C00UL,
    SPI_DATASIZE_14BIT = 0x0D00UL,
    SPI_DATASIZE_15BIT = 0x0E00UL,
    SPI_DATASIZE_16BIT = 0x0F00UL
} SPI_DataSizeTypeDef;

/* Baud Rate Prescaler */
typedef enum {
    SPI_BAUDRATEPRESCALER_2   = 0x0000UL,
    SPI_BAUDRATEPRESCALER_4   = 0x0008UL,
    SPI_BAUDRATEPRESCALER_8   = 0x0010UL,
    SPI_BAUDRATEPRESCALER_16  = 0x0018UL,
    SPI_BAUDRATEPRESCALER_32  = 0x0020UL,
    SPI_BAUDRATEPRESCALER_64  = 0x0028UL,
    SPI_BAUDRATEPRESCALER_128 = 0x0030UL,
    SPI_BAUDRATEPRESCALER_256 = 0x0038UL
} SPI_BaudRatePrescalerTypeDef;

/* NSS Pin Management */
typedef enum {
    SPI_NSS_SOFT = 0x0200UL,          /*!< Software NSS management */
    SPI_NSS_HARD_OUTPUT = 0x0004UL,    /*!< Hardware NSS output enable */
    SPI_NSS_HARD_INPUT = 0x0000UL     /*!< Hardware NSS input */
} SPI_NSSModeTypeDef;

/* Frame Format */
typedef enum {
    SPI_FRAME_FORMAT_MSBFIRST = 0x0000UL,
    SPI_FRAME_FORMAT_LSBFIRST = 0x0080UL
} SPI_FrameFormatTypeDef;

/* FIFO Threshold */
typedef enum {
    SPI_FIFO_THRESHOLD_4DATA = 0x0000UL,
    SPI_FIFO_THRESHOLD_8DATA = 0x1000UL
} SPI_FIFOThresholdTypeDef;

/* Communication Mode */
typedef enum {
    SPI_COMM_MODE_POLING = 0,
    SPI_COMM_MODE_INTERRUPT = 1,
    SPI_COMM_MODE_DMA = 2
} SPI_CommModeTypeDef;

/*============================================================================
 * SPI Init Structure Definition
 *============================================================================*/
typedef struct {
    SPI_ModeTypeDef Mode;                /*!< Specifies the SPI operating mode */
    SPI_BaudRatePrescalerTypeDef BaudRate; /*!< Specifies the SPI baud rate prescaler */
    SPI_CPOLTypeDef ClockPolarity;       /*!< Specifies the clock polarity */
    SPI_CPHATypeDef ClockPhase;          /*!< Specifies the clock phase */
    SPI_DataSizeTypeDef DataSize;        /*!< Specifies the data size */
    SPI_NSSModeTypeDef NSS;              /*!< Specifies the NSS mode */
    SPI_FrameFormatTypeDef FrameFormat;  /*!< Specifies the frame format */
    bool CRC_Enable;                     /*!< Enable or disable CRC */
    uint8_t CRC_Polynomial;              /*!< CRC polynomial */
    bool FIFOThreshold;                   /*!< FIFO threshold level */
} SPI_InitTypeDef;

/*============================================================================
 * DMA Configuration Structures (for STM32F0)
 *============================================================================*/

/* DMA Base Addresses */
#define DMA1_BASE        (0x40020000UL)
#define DMA_ISR          (*(volatile uint32_t *)(DMA1_BASE + 0x00))
#define DMA_IFCR         (*(volatile uint32_t *)(DMA1_BASE + 0x04))
#define DMA_CCR1         (*(volatile uint32_t *)(DMA1_BASE + 0x08 + 0x00))
#define DMA_CCR2         (*(volatile uint32_t *)(DMA1_BASE + 0x08 + 0x0C))
#define DMA_CCR3         (*(volatile uint32_t *)(DMA1_BASE + 0x08 + 0x18))
#define DMA_CCR4         (*(volatile uint32_t *)(DMA1_BASE + 0x08 + 0x24))
#define DMA_CCR5         (*(volatile uint32_t *)(DMA1_BASE + 0x08 + 0x30))
#define DMA_CCR6         (*(volatile uint32_t *)(DMA1_BASE + 0x08 + 0x3C))
#define DMA_CCR7         (*(volatile uint32_t *)(DMA1_BASE + 0x08 + 0x48))
#define DMA_CPAR1        (*(volatile uint32_t *)(DMA1_BASE + 0x0C + 0x00))
#define DMA_CPAR2        (*(volatile uint32_t *)(DMA1_BASE + 0x0C + 0x0C))
#define DMA_CPAR3        (*(volatile uint32_t *)(DMA1_BASE + 0x0C + 0x18))
#define DMA_CPAR4        (*(volatile uint32_t *)(DMA1_BASE + 0x0C + 0x24))
#define DMA_CPAR5        (*(volatile uint32_t *)(DMA1_BASE + 0x0C + 0x30))
#define DMA_CPAR6        (*(volatile uint32_t *)(DMA1_BASE + 0x0C + 0x3C))
#define DMA_CPAR7        (*(volatile uint32_t *)(DMA1_BASE + 0x0C + 0x48))
#define DMA_CMAR1        (*(volatile uint32_t *)(DMA1_BASE + 0x10 + 0x00))
#define DMA_CMAR2        (*(volatile uint32_t *)(DMA1_BASE + 0x10 + 0x0C))
#define DMA_CMAR3        (*(volatile uint32_t *)(DMA1_BASE + 0x10 + 0x18))
#define DMA_CMAR4        (*(volatile uint32_t *)(DMA1_BASE + 0x10 + 0x24))
#define DMA_CMAR5        (*(volatile uint32_t *)(DMA1_BASE + 0x10 + 0x30))
#define DMA_CMAR6        (*(volatile uint32_t *)(DMA1_BASE + 0x10 + 0x3C))
#define DMA_CMAR7        (*(volatile uint32_t *)(DMA1_BASE + 0x10 + 0x48))
#define DMA_CNDTR1       (*(volatile uint32_t *)(DMA1_BASE + 0x14 + 0x00))
#define DMA_CNDTR2       (*(volatile uint32_t *)(DMA1_BASE + 0x14 + 0x0C))
#define DMA_CNDTR3       (*(volatile uint32_t *)(DMA1_BASE + 0x14 + 0x18))
#define DMA_CNDTR4       (*(volatile uint32_t *)(DMA1_BASE + 0x14 + 0x24))
#define DMA_CNDTR5       (*(volatile uint32_t *)(DMA1_BASE + 0x14 + 0x30))
#define DMA_CNDTR6       (*(volatile uint32_t *)(DMA1_BASE + 0x14 + 0x3C))
#define DMA_CNDTR7       (*(volatile uint32_t *)(DMA1_BASE + 0x14 + 0x48))

/* DMA Channel/Stream Selections for SPI */
#define DMA_CHANNEL_SPI1_TX  0x0000UL  /*!< DMA Channel 0 for SPI1 TX */
#define DMA_CHANNEL_SPI1_RX  0x0000UL  /*!< DMA Channel 0 for SPI1 RX */
#define DMA_CHANNEL_SPI2_TX  0x0001UL  /*!< DMA Channel 1 for SPI2 TX */
#define DMA_CHANNEL_SPI2_RX  0x0001UL  /*!< DMA Channel 1 for SPI2 RX */

/* DMA Stream Defines */
#define DMA_STREAM_SPI1_TX  3  /*!< DMA1 Stream 3 for SPI1 TX */
#define DMA_STREAM_SPI1_RX  2  /*!< DMA1 Stream 2 for SPI1 RX */
#define DMA_STREAM_SPI2_TX  5  /*!< DMA1 Stream 5 for SPI2 TX */
#define DMA_STREAM_SPI2_RX  4  /*!< DMA1 Stream 4 for SPI2 RX */

/* DMA Channel Register Offsets */
#define DMA_CCR_OFFSET    0x08
#define DMA_CPAR_OFFSET   0x0C
#define DMA_CMAR_OFFSET   0x10
#define DMA_CNDTR_OFFSET  0x14

/* DMA CR Register Bits */
#define DMA_CR_EN         (0x0001UL)  /*!< Channel Enable */
#define DMA_CR_TCIE       (0x0002UL)  /*!< Transfer Complete Interrupt Enable */
#define DMA_CR_HTIE       (0x0004UL)  /*!< Half Transfer Interrupt Enable */
#define DMA_CR_TEIE       (0x0008UL)  /*!< Transfer Error Interrupt Enable */
#define DMA_CR_DIR        (0x0010UL)  /*!< Data Transfer Direction */
#define DMA_CR_CIRC       (0x0020UL)  /*!< Circular Mode */
#define DMA_CR_PINC       (0x0040UL)  /*!< Peripheral Increment Mode */
#define DMA_CR_MINC       (0x0080UL)  /*!< Memory Increment Mode */
#define DMA_CR_PSIZE      (0x0300UL)  /*!< Peripheral Size [1:0] */
#define DMA_CR_PSIZE_0   (0x0100UL)  /*!< Bit 0 */
#define DMA_CR_PSIZE_1   (0x0200UL)  /*!< Bit 1 */
#define DMA_CR_MSIZE      (0x0C00UL)  /*!< Memory Size [1:0] */
#define DMA_CR_MSIZE_0   (0x0400UL)  /*!< Bit 0 */
#define DMA_CR_MSIZE_1   (0x0800UL)  /*!< Bit 1 */
#define DMA_CR_PL         (0x3000UL)  /*!< Priority Level [1:0] */

/* DMA Flags */
#define DMA_FLAG_TCIF     (0x00000020UL)  /*!< Transfer Complete Flag */
#define DMA_FLAG_HTIF     (0x00000010UL)  /*!< Half Transfer Flag */
#define DMA_FLAG_TEIF     (0x00000008UL)  /*!< Transfer Error Flag */

/*============================================================================
 * NVIC Interrupt Numbers for SPI
 *============================================================================*/
#define SPI1_IRQn         35   /*!< SPI1 Global Interrupt */
#define SPI2_IRQn         36   /*!< SPI2 Global Interrupt */

/*============================================================================
 * SPI Driver API - Initialization and Configuration
 *============================================================================*/

/**
 * @brief  Initialize SPI peripheral
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  SPI_Init: pointer to SPI_InitTypeDef structure
 */
void SPI_Init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_Init);

/**
 * @brief  De-Initialize SPI peripheral
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 */
void SPI_DeInit(SPI_TypeDef *SPIx);

/**
 * @brief  Fill initialization structure with default values
 * @param  SPI_InitStruct: pointer to SPI_InitTypeDef structure
 */
void SPI_StructInit(SPI_InitTypeDef *SPI_Init);

/**
 * @brief  Enable SPI peripheral
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 */
void SPI_Enable(SPI_TypeDef *SPIx);

/**
 * @brief  Disable SPI peripheral
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 */
void SPI_Disable(SPI_TypeDef *SPIx);

/*============================================================================
 * SPI Driver API - Data Transfer (Polling)
 *============================================================================*/

/**
 * @brief  Transmit and Receive a data byte (Full Duplex - Polling)
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  txData: data to transmit
 * @return received data
 */
uint8_t SPI_TransmitReceive(SPI_TypeDef *SPIx, uint8_t txData);

/**
 * @brief  Transmit a data byte (Polling)
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  txData: data to transmit
 */
void SPI_SendData(SPI_TypeDef *SPIx, uint8_t txData);

/**
 * @brief  Receive a data byte (Polling)
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @return received data
 */
uint8_t SPI_ReceiveData(SPI_TypeDef *SPIx);

/**
 * @brief  Transmit and Receive multiple data bytes (Full Duplex - Polling)
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  txBuffer: pointer to transmit buffer
 * @param  rxBuffer: pointer to receive buffer
 * @param  length: number of bytes to transfer
 */
bool SPI_TransmitReceiveBuffer(SPI_TypeDef *SPIx, uint8_t *txBuffer, uint8_t *rxBuffer, uint16_t length);

/**
 * @brief  Transmit multiple data bytes (Polling)
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  txBuffer: pointer to transmit buffer
 * @param  length: number of bytes to transmit
 */
void SPI_SendBuffer(SPI_TypeDef *SPIx, uint8_t *txBuffer, uint16_t length);

/**
 * @brief  Receive multiple data bytes (Polling)
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  rxBuffer: pointer to receive buffer
 * @param  length: number of bytes to receive
 */
void SPI_ReceiveBuffer(SPI_TypeDef *SPIx, uint8_t *rxBuffer, uint16_t length);

/*============================================================================
 * SPI Driver API - Interrupt Functions
 *============================================================================*/

/**
 * @brief  Enable SPI interrupts
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  interrupt: interrupt source to enable (TXE, RXNE, ERR)
 */
void SPI_EnableInterrupt(SPI_TypeDef *SPIx, uint32_t interrupt);

/**
 * @brief  Disable SPI interrupts
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  interrupt: interrupt source to disable
 */
void SPI_DisableInterrupt(SPI_TypeDef *SPIx, uint32_t interrupt);

/**
 * @brief  Get SPI interrupt status
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  interrupt: interrupt source to check
 * @return interrupt status (SET or RESET)
 */
uint32_t SPI_GetInterruptStatus(SPI_TypeDef *SPIx, uint32_t interrupt);

/**
 * @brief  Clear SPI interrupt flag
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  flag: flag to clear
 */
void SPI_ClearFlag(SPI_TypeDef *SPIx, uint32_t flag);

/*============================================================================
 * SPI Driver API - Status Flags
 *============================================================================*/

/**
 * @brief  Get SPI status flags
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @return SPI status register value
 */
uint32_t SPI_GetStatus(SPI_TypeDef *SPIx);

/**
 * @brief  Check if TX buffer is empty
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @return true if TX buffer is empty
 */
bool SPI_IsTXEmpty(SPI_TypeDef *SPIx);

/**
 * @brief  Check if RX buffer is not empty
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @return true if RX buffer has data
 */
bool SPI_IsRXNotEmpty(SPI_TypeDef *SPIx);

/**
 * @brief  Check if SPI is busy
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @return true if SPI is busy
 */
bool SPI_IsBusy(SPI_TypeDef *SPIx);

/*============================================================================
 * SPI Driver API - CRC Functions
 *============================================================================*/

/**
 * @brief  Enable CRC
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 */
void SPI_EnableCRC(SPI_TypeDef *SPIx);

/**
 * @brief  Disable CRC
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 */
void SPI_DisableCRC(SPI_TypeDef *SPIx);

/**
 * @brief  Reset CRC
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 */
void SPI_ResetCRC(SPI_TypeDef *SPIx);

/**
 * @brief  Get transmitted CRC value
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @return transmitted CRC value
 */
uint16_t SPI_GetTransmitCRC(SPI_TypeDef *SPIx);

/**
 * @brief  Get received CRC value
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @return received CRC value
 */
uint16_t SPI_GetReceiveCRC(SPI_TypeDef *SPIx);

/**
 * @brief  Check if CRC error occurred
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @return true if CRC error occurred
 */
bool SPI_IsCRCError(SPI_TypeDef *SPIx);

/**
 * @brief  Clear CRC error flag
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 */
void SPI_ClearCRCError(SPI_TypeDef *SPIx);

/*============================================================================
 * SPI Driver API - DMA Functions
 *============================================================================*/

/**
 * @brief  Enable SPI DMA
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  direction: DMA direction (TX, RX, or TX_RX)
 */
void SPI_EnableDMA(SPI_TypeDef *SPIx, uint8_t direction);

/**
 * @brief  Disable SPI DMA
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  direction: DMA direction (TX, RX, or TX_RX)
 */
void SPI_DisableDMA(SPI_TypeDef *SPIx, uint8_t direction);

/*============================================================================
 * DMA Driver API
 *============================================================================*/

/**
 * @brief  Configure DMA channel for SPI
 * @param  channel: DMA channel number (1-7)
 * @param  peripheralAddr: peripheral address (SPI data register)
 * @param  memoryAddr: memory address
 * @param  dataSize: number of data to transfer
 * @param  direction: transfer direction (0=read from peripheral, 1=read from memory)
 * @param  memoryIncrement: enable memory increment
 */
void DMA_ConfigChannel(uint8_t channel, uint32_t peripheralAddr, uint32_t memoryAddr, 
                       uint16_t dataSize, uint8_t direction, bool memoryIncrement);

/**
 * @brief  Enable DMA channel
 * @param  channel: DMA channel number (1-7)
 */
void DMA_EnableChannel(uint8_t channel);

/**
 * @brief  Disable DMA channel
 * @param  channel: DMA channel number (1-7)
 */
void DMA_DisableChannel(uint8_t channel);

/**
 * @brief  Get DMA channel status flags
 * @param  channel: DMA channel number (1-7)
 * @return status flags
 */
uint32_t DMA_GetStatusFlags(uint8_t channel);

/**
 * @brief  Clear DMA channel flags
 * @param  channel: DMA channel number (1-7)
 * @param  flags: flags to clear
 */
void DMA_ClearFlags(uint8_t channel, uint32_t flags);

/**
 * @brief  Check if DMA transfer is complete
 * @param  channel: DMA channel number (1-7)
 * @return true if transfer is complete
 */
bool DMA_IsTransferComplete(uint8_t channel);

/*============================================================================
 * NVIC Functions for SPI
 *============================================================================*/

/**
 * @brief  Enable SPI interrupt in NVIC
 * @param  IRQn: interrupt number
 */
void NVIC_EnableSPI(uint8_t IRQn);

/**
 * @brief  Disable SPI interrupt in NVIC
 * @param  IRQn: interrupt number
 */
void NVIC_DisableSPI(uint8_t IRQn);

/**
 * @brief  Set SPI interrupt priority
 * @param  IRQn: interrupt number
 * @param  priority: priority (0-3)
 */
void NVIC_SetSPIPriority(uint8_t IRQn, uint8_t priority);

void SPI_ConfigGPIO(SPI_TypeDef *SPIx);

/*============================================================================
 * Helper Macros
 *============================================================================*/

/* SPI Interrupt Enable/Disable Macros */
#define SPI_IT_TXE        SPI_CR2_TXEIE    /*!< TX Buffer Empty Interrupt */
#define SPI_IT_RXNE       SPI_CR2_RXNEIE   /*!< RX Buffer Not Empty Interrupt */
#define SPI_IT_ERR        SPI_CR2_ERRIE    /*!< Error Interrupt */

/* DMA Direction */
#define SPI_DMA_DIR_TX    0x01  /*!< DMA TX Direction */
#define SPI_DMA_DIR_RX    0x02  /*!< DMA RX Direction */
#define SPI_DMA_DIR_TX_RX 0x03  /*!< DMA TX and RX Direction */

/* Flag definitions */
#define SPI_FLAG_RXNE     SPI_SR_RXNE    /*!< Receive buffer not empty flag */
#define SPI_FLAG_TXE      SPI_SR_TXE     /*!< Transmit buffer empty flag */
#define SPI_FLAG_BSY      SPI_SR_BSY     /*!< Busy flag */
#define SPI_FLAG_OVR      SPI_SR_OVR     /*!< Overrun flag */
#define SPI_FLAG_MODF     SPI_SR_MODF    /*!< Mode fault flag */
#define SPI_FLAG_CRCERR   SPI_SR_CRCERR  /*!< CRC error flag */

#endif /* SPIF051_H */
