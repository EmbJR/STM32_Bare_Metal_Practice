/**
 * @file spiF051.h
 * @brief SPI Driver for STM32F051R8T6
 * @description This file contains the SPI peripheral driver for STM32F051 series
 *              microcontrollers. It provides functions for configuring and using
 *              the SPI peripheral in various modes (Master/Slave, Full-duplex/Simplex,
 *              8-bit/16-bit data frame, with/without CRC).
 */

#ifndef SPIF051_H
#define SPIF051_H

#include <stdint.h>
#include <stdbool.h>

//============================================================================
// SPI Base Addresses (STM32F051)
//============================================================================
#define SPI1_BASE       0x40013000UL
#define SPI2_BASE       0x40003800UL

/* Interrupt Numbers */
#define SPI1_IRQn       25   /*!< SPI 1 Interrupt */
#define SPI2_IRQn       26   /*!< SPI 2 Interrupt */

//============================================================================
// SPI Register Structure
//============================================================================
typedef struct {
    volatile uint32_t CR1;       // SPI Control Register 1
    volatile uint32_t CR2;       // SPI Control Register 2
    volatile uint32_t SR;        // SPI Status Register
    volatile uint32_t DR;        // SPI Data Register
    volatile uint32_t CRCPR;     // SPI CRC Polynomial Register
    volatile uint32_t RXCRCR;    // SPI RX CRC Register
    volatile uint32_t TXCRCR;    // SPI TX CRC Register
    volatile uint32_t I2SCFGR;   // SPI I2S Configuration Register
} SPI_TypeDef;

// SPI Instances
#define SPI1            ((SPI_TypeDef *)SPI1_BASE)
#define SPI2            ((SPI_TypeDef *)SPI2_BASE)

//============================================================================
// SPI Clock Enable Macros (RCC)
//============================================================================
#define RCC_APB2ENR_SPI1EN     (1U << 12)   // SPI1 clock enable
#define RCC_APB1ENR_SPI2EN     (1U << 14)   // SPI2 clock enable

//============================================================================
// SPI_CR1 Bit Definitions
//============================================================================
#define SPI_CR1_CPHA           (1U << 0)    // Clock Phase
#define SPI_CR1_CPOL           (1U << 1)    // Clock Polarity
#define SPI_CR1_MSTR           (1U << 2)    // Master Selection
#define SPI_CR1_BR             (7U << 3)    // Baud Rate Control
#define SPI_CR1_BR_0           (1U << 3)
#define SPI_CR1_BR_1           (1U << 4)
#define SPI_CR1_BR_2           (1U << 5)
#define SPI_CR1_SPE            (1U << 6)    // SPI Enable
#define SPI_CR1_LSBFIRST       (1U << 7)    // Frame Format (LSB/MSB)
#define SPI_CR1_SSI            (1U << 8)    // Internal Slave Select
#define SPI_CR1_SSM            (1U << 9)    // Software Slave Management
#define SPI_CR1_RXONLY         (1U << 10)   // Receive Only Mode
#define SPI_CR1_DFF            (1U << 11)   // Data Frame Format (8/16-bit)
#define SPI_CR1_CRCNEXT        (1U << 12)   // CRC Transfer Next
#define SPI_CR1_CRCEN          (1U << 13)    // CRC Enable
#define SPI_CR1_BIDIOE         (1U << 14)    // Output Enable in Bidirectional Mode
#define SPI_CR1_BIDIMODE       (1U << 15)   // Bidirectional Data Mode

//============================================================================
// SPI_CR2 Bit Definitions
//============================================================================
#define SPI_CR2_RXDMAEN        (1U << 0)    // Rx Buffer DMA Enable
#define SPI_CR2_TXDMAEN        (1U << 1)    // Tx Buffer DMA Enable
#define SPI_CR2_SSOE           (1U << 2)    // SS Output Enable
#define SPI_CR2_NSSP           (1U << 3)   // NSS Pulse Management
#define SPI_CR2_FRF            (1U << 4)    // Frame Format (Motorola/TI)
#define SPI_CR2_ERRIE          (1U << 5)    // Error Interrupt Enable
#define SPI_CR2_RXNEIE         (1U << 6)    // RX Buffer Not Empty Interrupt Enable
#define SPI_CR2_TXEIE          (1U << 7)    // TX Buffer Empty Interrupt Enable
#define SPI_CR2_DS             (0xFU << 8)  // Data Size
#define SPI_CR2_DS_0           (1U << 8)
#define SPI_CR2_DS_1           (1U << 9)
#define SPI_CR2_DS_2           (1U << 10)
#define SPI_CR2_DS_3           (1U << 11)
#define SPI_CR2_FRXTH          (1U << 12)   // FIFO Reception Threshold
#define SPI_CR2_LDMA_RX        (1U << 13)   // Last DMA Rx Request
#define SPI_CR2_LDMA_TX        (1U << 14)   // Last DMA Tx Request


//============================================================================
// SPI_SR Bit Definitions
//============================================================================
#define SPI_SR_RXNE            (1U << 0)    // Receive Buffer Not Empty
#define SPI_SR_TXE             (1U << 1)    // Transmit Buffer Empty
#define SPI_SR_CHSIDE          (1U << 2)    // Channel Side
#define SPI_SR_UDR             (1U << 3)    // Underrun Flag
#define SPI_SR_CRCERR          (1U << 4)    // CRC Error Flag
#define SPI_SR_MODF            (1U << 5)    // Mode Fault
#define SPI_SR_OVR             (1U << 6)    // Overrun Flag
#define SPI_SR_BSY             (1U << 7)    // Busy Flag
#define SPI_SR_FRE             (1U << 8)    // Frame Error
#define SPI_SR_FTLVL           (3U << 9)    // FIFO Transmission Level
#define SPI_SR_FRLVL           (3U << 11)   // FIFO Reception Level

//============================================================================
// SPI Mode Configuration
//============================================================================
typedef enum {
    SPI_MODE_SLAVE = 0,
    SPI_MODE_MASTER = 1
} SPI_Mode;

typedef enum {
    SPI_CLOCK_POLARITY_LOW = 0,     // CK to 0 when idle
    SPI_CLOCK_POLARITY_HIGH = 1     // CK to 1 when idle
} SPI_ClockPolarity;

typedef enum {
    SPI_CLOCK_PHASE_1ST_EDGE = 0,   // Data captured on first edge
    SPI_CLOCK_PHASE_2ND_EDGE = 1    // Data captured on second edge
} SPI_ClockPhase;

typedef enum {
    SPI_FRAME_FORMAT_MSB_FIRST = 0,
    SPI_FRAME_FORMAT_LSB_FIRST = 1
} SPI_FrameFormat;

typedef enum {
    SPI_DATA_SIZE_8BIT = 7,         // 0x07 = 8-bit
    SPI_DATA_SIZE_9BIT = 8,        // 0x08 = 9-bit
    SPI_DATA_SIZE_10BIT = 9,       // 0x09 = 10-bit
    SPI_DATA_SIZE_11BIT = 10,      // 0x0A = 11-bit
    SPI_DATA_SIZE_12BIT = 11,      // 0x0B = 12-bit
    SPI_DATA_SIZE_13BIT = 12,      // 0x0C = 13-bit
    SPI_DATA_SIZE_14BIT = 13,      // 0x0D = 14-bit
    SPI_DATA_SIZE_15BIT = 14,      // 0x0E = 15-bit
    SPI_DATA_SIZE_16BIT = 15       // 0x0F = 16-bit
} SPI_DataSize;

typedef enum {
    SPI_BAUD_RATE_DIV_2 = 0,
    SPI_BAUD_RATE_DIV_4 = 1,
    SPI_BAUD_RATE_DIV_8 = 2,
    SPI_BAUD_RATE_DIV_16 = 3,
    SPI_BAUD_RATE_DIV_32 = 4,
    SPI_BAUD_RATE_DIV_64 = 5,
    SPI_BAUD_RATE_DIV_128 = 6,
    SPI_BAUD_RATE_DIV_256 = 7
} SPI_BaudRate;

typedef enum {
    SPI_COMM_MODE_FULL_DUPLEX = 0,
    SPI_COMM_MODE_TRANSMIT_ONLY = 1,
    SPI_COMM_MODE_RECEIVE_ONLY = 2,
    SPI_COMM_MODE_HALF_DUPLEX = 3
} SPI_CommMode;

typedef enum {
    SPI_NSS_SOFTWARE = 0,
    SPI_NSS_HARDWARE_OUTPUT = 1,
    SPI_NSS_HARDWARE_INPUT = 2
} SPI_NSSMode;

typedef enum {
    SPI_FRAME_FORMAT_MOTOROLA = 0,
    SPI_FRAME_FORMAT_TI = 1
} SPI_ProtocolFormat;

typedef enum {
    SPI_FIFO_THRESHOLD_1_4 = 0,     // 1/4 FIFO
    SPI_FIFO_THRESHOLD_1_2 = 1      // 1/2 FIFO
} SPI_FIFOThreshold;

//============================================================================
// SPI Configuration Structure
//============================================================================
typedef struct {
    SPI_Mode mode;                   // Master or Slave
    SPI_ClockPolarity clock_polarity;
    SPI_ClockPhase clock_phase;
    SPI_FrameFormat frame_format;
    SPI_DataSize data_size;
    SPI_BaudRate baud_rate;
    SPI_CommMode comm_mode;
    SPI_NSSMode nss_mode;
    SPI_ProtocolFormat protocol;
    SPI_FIFOThreshold rx_fifo_threshold;
    bool crc_enabled;
    uint8_t crc_polynomial;
    bool nss_pulse_enabled;
} SPI_Config;

//============================================================================
// SPI Handle Structure
//============================================================================
typedef struct {
    SPI_TypeDef *Instance;
    SPI_Config Init;
    volatile bool is_busy;
} SPI_HandleTypeDef;

//============================================================================
// SPI Instance Selection
//============================================================================
typedef enum {
    SPI_INSTANCE_1 = 0,
    SPI_INSTANCE_2 = 1
} SPI_Instance;

//============================================================================
// Function Prototypes
//============================================================================

// Initialization and De-initialization
void SPI_Init(SPI_HandleTypeDef *hspi);
void SPI_DeInit(SPI_HandleTypeDef *hspi);
void SPI_EnableClock(SPI_Instance instance);
void SPI_DisableClock(SPI_Instance instance);

// Configuration functions
void SPI_SetBaudRate(SPI_HandleTypeDef *hspi, SPI_BaudRate baud_rate);
void SPI_SetDataSize(SPI_HandleTypeDef *hspi, SPI_DataSize data_size);
void SPI_SetClockPolarity(SPI_HandleTypeDef *hspi, SPI_ClockPolarity polarity);
void SPI_SetClockPhase(SPI_HandleTypeDef *hspi, SPI_ClockPhase phase);
void SPI_SetFrameFormat(SPI_HandleTypeDef *hspi, SPI_FrameFormat format);
void SPI_SetCommMode(SPI_HandleTypeDef *hspi, SPI_CommMode mode);
void SPI_SetNSSMode(SPI_HandleTypeDef *hspi, SPI_NSSMode nss_mode);
void SPI_SetNSSPin(SPI_HandleTypeDef *hspi, bool state);

// Enable/Disable functions
void SPI_Enable(SPI_HandleTypeDef *hspi);
void SPI_Disable(SPI_HandleTypeDef *hspi);
void SPI_EnableCRC(SPI_HandleTypeDef *hspi);
void SPI_DisableCRC(SPI_HandleTypeDef *hspi);

// Data transfer functions
void SPI_SendData(SPI_HandleTypeDef *hspi, uint16_t data);
uint16_t SPI_ReceiveData(SPI_HandleTypeDef *hspi);
void SPI_SendData8(SPI_HandleTypeDef *hspi, uint8_t data);
uint8_t SPI_ReceiveData8(SPI_HandleTypeDef *hspi);
void SPI_SendData32(SPI_HandleTypeDef *hspi, uint32_t data);
uint32_t SPI_ReceiveData32(SPI_HandleTypeDef *hspi);

// Blocking transfer functions
uint16_t SPI_Transfer(SPI_HandleTypeDef *hspi, uint16_t data);
void SPI_TransferBuffer(SPI_HandleTypeDef *hspi, uint16_t *tx_buffer, 
                        uint16_t *rx_buffer, uint16_t size);
void SPI_SendBuffer(SPI_HandleTypeDef *hspi, uint16_t *buffer, uint16_t size);
void SPI_ReceiveBuffer(SPI_HandleTypeDef *hspi, uint16_t *buffer, uint16_t size);

// 8-bit buffer transfer functions (for 8-bit data size)
void SPI_TransferBuffer8(SPI_HandleTypeDef *hspi, uint8_t *tx_buffer, 
                         uint8_t *rx_buffer, uint16_t size);
void SPI_SendBuffer8(SPI_HandleTypeDef *hspi, uint8_t *buffer, uint16_t size);
void SPI_ReceiveBuffer8(SPI_HandleTypeDef *hspi, volatile uint8_t *buffer, uint16_t size);

// CRC functions
void SPI_SetCRCPolynomial(SPI_HandleTypeDef *hspi, uint8_t polynomial);
uint16_t SPI_GetTXCRC(SPI_HandleTypeDef *hspi);
uint16_t SPI_GetRXCRC(SPI_HandleTypeDef *hspi);
void SPI_ResetCRC(SPI_HandleTypeDef *hspi);
bool SPI_CheckCRCError(SPI_HandleTypeDef *hspi);
void SPI_ClearCRCError(SPI_HandleTypeDef *hspi);
void SPI_TransmitCRC(SPI_HandleTypeDef *hspi);

// Status functions
bool SPI_GetFlagStatus(SPI_HandleTypeDef *hspi, uint32_t flag);
bool SPI_IsTXE(SPI_HandleTypeDef *hspi);
bool SPI_IsRXNE(SPI_HandleTypeDef *hspi);
bool SPI_IsBusy(SPI_HandleTypeDef *hspi);
uint8_t SPI_GetFIFOLevel(SPI_HandleTypeDef *hspi);

// Wait functions
void SPI_WaitTXE(SPI_HandleTypeDef *hspi);
void SPI_WaitRXNE(SPI_HandleTypeDef *hspi);
void SPI_WaitNotBusy(SPI_HandleTypeDef *hspi);
void SPI_WaitFIFOEmpty(SPI_HandleTypeDef *hspi);

// Flush functions
void SPI_FlushRX(SPI_HandleTypeDef *hspi);
void SPI_FlushTX(SPI_HandleTypeDef *hspi);

// NSS Pulse Management
void SPI_EnableNSSP(SPI_HandleTypeDef *hspi);
void SPI_DisableNSSP(SPI_HandleTypeDef *hspi);

// TI Protocol Mode
void SPI_EnableTIProtocol(SPI_HandleTypeDef *hspi);
void SPI_DisableTIProtocol(SPI_HandleTypeDef *hspi);

// DMA support (optional)
void SPI_EnableDMA_TX(SPI_HandleTypeDef *hspi);
void SPI_DisableDMA_TX(SPI_HandleTypeDef *hspi);
void SPI_EnableDMA_RX(SPI_HandleTypeDef *hspi);
void SPI_DisableDMA_RX(SPI_HandleTypeDef *hspi);

// Interrupt support (optional)
void SPI_EnableInterrupt(SPI_HandleTypeDef *hspi, uint32_t interrupt);
void SPI_DisableInterrupt(SPI_HandleTypeDef *hspi, uint32_t interrupt);

// Low-level disable procedure
void SPI_DisablePeripheral(SPI_HandleTypeDef *hspi);

//============================================================================
// Macros for common configurations
//============================================================================

// Common SPI mode configurations
#define SPI_MODE_0 (SPI_CLOCK_POLARITY_LOW, SPI_CLOCK_PHASE_1ST_EDGE)
#define SPI_MODE_1 (SPI_CLOCK_POLARITY_LOW, SPI_CLOCK_PHASE_2ND_EDGE)
#define SPI_MODE_2 (SPI_CLOCK_POLARITY_HIGH, SPI_CLOCK_PHASE_1ST_EDGE)
#define SPI_MODE_3 (SPI_CLOCK_POLARITY_HIGH, SPI_CLOCK_PHASE_2ND_EDGE)

// Status flag masks
#define SPI_FLAG_RXNE    SPI_SR_RXNE
#define SPI_FLAG_TXE     SPI_SR_TXE
#define SPI_FLAG_BSY     SPI_SR_BSY
#define SPI_FLAG_OVR     SPI_SR_OVR
#define SPI_FLAG_MODF    SPI_SR_MODF
#define SPI_FLAG_CRCERR  SPI_SR_CRCERR
#define SPI_FLAG_UDR     SPI_SR_UDR
#define SPI_FLAG_FRE     SPI_SR_FRE

// Interrupt masks
#define SPI_IT_TXE       SPI_CR2_TXEIE
#define SPI_IT_RXNE      SPI_CR2_RXNEIE
#define SPI_IT_ERR       SPI_CR2_ERRIE

#endif // SPIF051_H
