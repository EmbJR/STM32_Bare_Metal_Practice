/**
 * @file spiF051.c
 * @brief SPI Driver Implementation for STM32F051R8T6
 * @description This file contains the implementation of SPI peripheral driver
 *              for STM32F051 series microcontrollers.
 */

#include "spiF051.h"
#include "rcc.h"

//============================================================================
// RCC Register Definitions for SPI Clock Control
//============================================================================
//#define RCC_BASE        0x40021000UL
//#define RCC_APB2ENR    (*(volatile uint32_t *)(RCC_BASE + 0x24))
//#define RCC_APB1ENR    (*(volatile uint32_t *)(RCC_BASE + 0x28))

//============================================================================
// Static Variables
//============================================================================
static SPI_HandleTypeDef g_spi_handle[SPI_INSTANCE_2 + 1];

//============================================================================
// Initialization and De-initialization Functions
//============================================================================

/**
 * @brief Initialize SPI peripheral with given configuration
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_Init(SPI_HandleTypeDef *hspi)
{
    uint32_t cr1 = 0;
    uint32_t cr2 = 0;
    
    // Enable SPI clock
    if (hspi->Instance == SPI1) {
        SPI_EnableClock(SPI_INSTANCE_1);
    } else if (hspi->Instance == SPI2) {
        SPI_EnableClock(SPI_INSTANCE_2);
    }
    
    // Disable SPI before configuration
    hspi->Instance->CR1 &= ~SPI_CR1_SPE;
    
    // Configure CR1 register
    // Master/Slave selection
    if (hspi->Init.mode == SPI_MODE_MASTER) {
        cr1 |= SPI_CR1_MSTR;
    }
    
    // Clock polarity
    if (hspi->Init.clock_polarity == SPI_CLOCK_POLARITY_HIGH) {
        cr1 |= SPI_CR1_CPOL;
    }
    
    // Clock phase
    if (hspi->Init.clock_phase == SPI_CLOCK_PHASE_2ND_EDGE) {
        cr1 |= SPI_CR1_CPHA;
    }
    
    // Frame format (LSB/MSB first)
    if (hspi->Init.frame_format == SPI_FRAME_FORMAT_LSB_FIRST) {
        cr1 |= SPI_CR1_LSBFIRST;
    }
    
    // Baud rate (only for master)
    if (hspi->Init.mode == SPI_MODE_MASTER) {
        cr1 |= (hspi->Init.baud_rate << 3) & SPI_CR1_BR;
    }
    
    // Software slave management
    if (hspi->Init.nss_mode == SPI_NSS_SOFTWARE) {
        cr1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    }
    
    // Communication mode
    switch (hspi->Init.comm_mode) {
        case SPI_COMM_MODE_FULL_DUPLEX:
            // Default: both RXONLY=0, BIDIMODE=0
            break;
        case SPI_COMM_MODE_TRANSMIT_ONLY:
            cr1 |= SPI_CR1_RXONLY;
            break;
        case SPI_COMM_MODE_RECEIVE_ONLY:
            cr1 |= SPI_CR1_RXONLY;
            break;
        case SPI_COMM_MODE_HALF_DUPLEX:
            cr1 |= SPI_CR1_BIDIMODE;
            break;
    }
    
    // CRC enable
    if (hspi->Init.crc_enabled) {
        cr1 |= SPI_CR1_CRCEN;
    }
    
    // Data frame format (8-bit or 16-bit)
    // Note: DFF bit is not used when DS[3:0] is configured
    // We use DS bits for data size configuration in CR2
    
    // Write to CR1
    hspi->Instance->CR1 = cr1;
    
    // Configure CR2 register
    // Data size
    cr2 |= (hspi->Init.data_size << 8) & SPI_CR2_DS;
    
    // FIFO threshold
    if (hspi->Init.rx_fifo_threshold == SPI_FIFO_THRESHOLD_1_2) {
        cr2 |= SPI_CR2_FRXTH;
    }
    
    // NSS output (only for master)
    if ((hspi->Init.mode == SPI_MODE_MASTER) && 
        (hspi->Init.nss_mode == SPI_NSS_HARDWARE_OUTPUT)) {
        cr2 |= SPI_CR2_SSOE;
    }
    
    // NSS pulse mode
    if (hspi->Init.nss_pulse_enabled) {
        cr2 |= SPI_CR2_NSSP;
    }
    
    // TI protocol
    if (hspi->Init.protocol == SPI_FRAME_FORMAT_TI) {
        cr2 |= SPI_CR2_FRF;
    }
    
    // Write to CR2
    hspi->Instance->CR2 = cr2;
    
    // Configure CRC polynomial if CRC is enabled
    if (hspi->Init.crc_enabled) {
        hspi->Instance->CRCPR = hspi->Init.crc_polynomial;
    }
    
    // Mark as not busy
    hspi->is_busy = false;
}

/**
 * @brief De-initialize SPI peripheral
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_DeInit(SPI_HandleTypeDef *hspi)
{
    // Disable SPI
    hspi->Instance->CR1 &= ~SPI_CR1_SPE;
    
    // Disable clock
    if (hspi->Instance == SPI1) {
        RCC_APB2ENR &= ~RCC_APB2ENR_SPI1EN;
    } else if (hspi->Instance == SPI2) {
        RCC_APB1ENR &= ~RCC_APB1ENR_SPI2EN;
    }
    
    // Reset handle
    hspi->is_busy = false;
}

/**
 * @brief Enable SPI peripheral clock
 * @param instance: SPI instance (SPI_INSTANCE_1 or SPI_INSTANCE_2)
 */
void SPI_EnableClock(SPI_Instance instance)
{
    if (instance == SPI_INSTANCE_1) {
        RCC_APB2ENR |= RCC_APB2ENR_SPI1EN;
    } else if (instance == SPI_INSTANCE_2) {
        RCC_APB1ENR |= RCC_APB1ENR_SPI2EN;
    }
}

/**
 * @brief Disable SPI peripheral clock
 * @param instance: SPI instance (SPI_INSTANCE_1 or SPI_INSTANCE_2)
 */
void SPI_DisableClock(SPI_Instance instance)
{
    if (instance == SPI_INSTANCE_1) {
        RCC_APB2ENR &= ~RCC_APB2ENR_SPI1EN;
    } else if (instance == SPI_INSTANCE_2) {
        RCC_APB1ENR &= ~RCC_APB1ENR_SPI2EN;
    }
}

//============================================================================
// Configuration Functions
//============================================================================

/**
 * @brief Set SPI baud rate
 * @param hspi: Pointer to SPI handle structure
 * @param baud_rate: Baud rate divider
 */
void SPI_SetBaudRate(SPI_HandleTypeDef *hspi, SPI_BaudRate baud_rate)
{
    hspi->Instance->CR1 = (hspi->Instance->CR1 & ~SPI_CR1_BR) | 
                          ((baud_rate << 3) & SPI_CR1_BR);
}

/**
 * @brief Set SPI data size
 * @param hspi: Pointer to SPI handle structure
 * @param data_size: Data frame size
 */
void SPI_SetDataSize(SPI_HandleTypeDef *hspi, SPI_DataSize data_size)
{
    hspi->Instance->CR2 = (hspi->Instance->CR2 & ~SPI_CR2_DS) | 
                          ((data_size << 8) & SPI_CR2_DS);
}

/**
 * @brief Set SPI clock polarity
 * @param hspi: Pointer to SPI handle structure
 * @param polarity: Clock polarity
 */
void SPI_SetClockPolarity(SPI_HandleTypeDef *hspi, SPI_ClockPolarity polarity)
{
    if (polarity == SPI_CLOCK_POLARITY_HIGH) {
        hspi->Instance->CR1 |= SPI_CR1_CPOL;
    } else {
        hspi->Instance->CR1 &= ~SPI_CR1_CPOL;
    }
}

/**
 * @brief Set SPI clock phase
 * @param hspi: Pointer to SPI handle structure
 * @param phase: Clock phase
 */
void SPI_SetClockPhase(SPI_HandleTypeDef *hspi, SPI_ClockPhase phase)
{
    if (phase == SPI_CLOCK_PHASE_2ND_EDGE) {
        hspi->Instance->CR1 |= SPI_CR1_CPHA;
    } else {
        hspi->Instance->CR1 &= ~SPI_CR1_CPHA;
    }
}

/**
 * @brief Set SPI frame format (MSB/LSB first)
 * @param hspi: Pointer to SPI handle structure
 * @param format: Frame format
 */
void SPI_SetFrameFormat(SPI_HandleTypeDef *hspi, SPI_FrameFormat format)
{
    if (format == SPI_FRAME_FORMAT_LSB_FIRST) {
        hspi->Instance->CR1 |= SPI_CR1_LSBFIRST;
    } else {
        hspi->Instance->CR1 &= ~SPI_CR1_LSBFIRST;
    }
}

/**
 * @brief Set SPI communication mode
 * @param hspi: Pointer to SPI handle structure
 * @param mode: Communication mode
 */
void SPI_SetCommMode(SPI_HandleTypeDef *hspi, SPI_CommMode mode)
{
    uint32_t cr1 = hspi->Instance->CR1;
    
    // Clear previous settings
    cr1 &= ~(SPI_CR1_RXONLY | SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE);
    
    switch (mode) {
        case SPI_COMM_MODE_FULL_DUPLEX:
            // Default: both RXONLY=0, BIDIMODE=0
            break;
        case SPI_COMM_MODE_TRANSMIT_ONLY:
            cr1 |= SPI_CR1_RXONLY;
            break;
        case SPI_COMM_MODE_RECEIVE_ONLY:
            cr1 |= SPI_CR1_RXONLY;
            break;
        case SPI_COMM_MODE_HALF_DUPLEX:
            cr1 |= SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE;
            break;
    }
    
    hspi->Instance->CR1 = cr1;
}

/**
 * @brief Set SPI NSS mode
 * @param hspi: Pointer to SPI handle structure
 * @param nss_mode: NSS mode
 */
void SPI_SetNSSMode(SPI_HandleTypeDef *hspi, SPI_NSSMode nss_mode)
{
    uint32_t cr1 = hspi->Instance->CR1;
    uint32_t cr2 = hspi->Instance->CR2;
    
    cr1 &= ~(SPI_CR1_SSM | SPI_CR1_SSI);
    cr2 &= ~SPI_CR2_SSOE;
    
    switch (nss_mode) {
        case SPI_NSS_SOFTWARE:
            cr1 |= SPI_CR1_SSM | SPI_CR1_SSI;
            break;
        case SPI_NSS_HARDWARE_OUTPUT:
            cr2 |= SPI_CR2_SSOE;
            break;
        case SPI_NSS_HARDWARE_INPUT:
            // No additional configuration needed
            break;
    }
    
    hspi->Instance->CR1 = cr1;
    hspi->Instance->CR2 = cr2;
}

/**
 * @brief Set SPI NSS pin state (for software NSS mode)
 * @param hspi: Pointer to SPI handle structure
 * @param state: NSS pin state (true = high, false = low)
 */
void SPI_SetNSSPin(SPI_HandleTypeDef *hspi, bool state)
{
    if (state) {
        hspi->Instance->CR1 |= SPI_CR1_SSI;
    } else {
        hspi->Instance->CR1 &= ~SPI_CR1_SSI;
    }
}

//============================================================================
// Enable/Disable Functions
//============================================================================

/**
 * @brief Enable SPI peripheral
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_Enable(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR1 |= SPI_CR1_SPE;
}

/**
 * @brief Disable SPI peripheral
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_Disable(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR1 &= ~SPI_CR1_SPE;
}

/**
 * @brief Enable CRC calculation
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_EnableCRC(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR1 |= SPI_CR1_CRCEN;
}

/**
 * @brief Disable CRC calculation
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_DisableCRC(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR1 &= ~SPI_CR1_CRCEN;
}

//============================================================================
// Data Transfer Functions
//============================================================================

/**
 * @brief Send data via SPI (16-bit)
 * @param hspi: Pointer to SPI handle structure
 * @param data: Data to send
 */
void SPI_SendData(SPI_HandleTypeDef *hspi, uint16_t data)
{
    hspi->Instance->DR = data;
}

/**
 * @brief Receive data via SPI (16-bit)
 * @param hspi: Pointer to SPI handle structure
 * @return Received data
 */
uint16_t SPI_ReceiveData(SPI_HandleTypeDef *hspi)
{
    return (uint16_t)hspi->Instance->DR;
}

/**
 * @brief Send data via SPI (8-bit)
 * @param hspi: Pointer to SPI handle structure
 * @param data: Data to send (8-bit)
 */
void SPI_SendData8(SPI_HandleTypeDef *hspi, uint8_t data)
{
	*((volatile uint8_t *)&hspi->Instance->DR) = (uint16_t)data;
}

/**
 * @brief Receive data via SPI (8-bit)
 * @param hspi: Pointer to SPI handle structure
 * @return Received data (8-bit)
 */
uint8_t SPI_ReceiveData8(SPI_HandleTypeDef *hspi)
{
    return (uint8_t)hspi->Instance->DR;
}

/**
 * @brief Send data via SPI (32-bit - for 16-bit x 2 packing)
 * @param hspi: Pointer to SPI handle structure
 * @param data: Data to send (32-bit)
 */
void SPI_SendData32(SPI_HandleTypeDef *hspi, uint32_t data)
{
    hspi->Instance->DR = (uint16_t)(data & 0xFFFF);
    // Wait for TXE before sending second half
    SPI_WaitTXE(hspi);
    hspi->Instance->DR = (uint16_t)((data >> 16) & 0xFFFF);
}

/**
 * @brief Receive data via SPI (32-bit - for 16-bit x 2 packing)
 * @param hspi: Pointer to SPI handle structure
 * @return Received data (32-bit)
 */
uint32_t SPI_ReceiveData32(SPI_HandleTypeDef *hspi)
{
    uint32_t data = 0;
    uint16_t temp;
    
    // Read first half
    SPI_WaitRXNE(hspi);
    temp = (uint16_t)hspi->Instance->DR;
    data = temp;
    
    // Read second half
    SPI_WaitRXNE(hspi);
    temp = (uint16_t)hspi->Instance->DR;
    data |= ((uint32_t)temp << 16);
    
    return data;
}

//============================================================================
// Blocking Transfer Functions
//============================================================================

/**
 * @brief Transfer single data word (full-duplex)
 * @param hspi: Pointer to SPI handle structure
 * @param data: Data to send
 * @return Received data
 */
uint16_t SPI_Transfer(SPI_HandleTypeDef *hspi, uint16_t data)
{
    // Wait for TX buffer empty
    SPI_WaitTXE(hspi);
    
    // Send data
    hspi->Instance->DR = data;
    
    // Wait for RX buffer not empty
    SPI_WaitRXNE(hspi);
    
    // Return received data
    return (uint16_t)hspi->Instance->DR;
}

/**
 * @brief Send buffer - 8-bit version (transmit only mode)
 * @param hspi: Pointer to SPI handle structure
 * @param buffer: Data buffer to send (8-bit)
 * @param size: Number of bytes to send
 */
void SPI_SendBuffer8(SPI_HandleTypeDef *hspi, uint8_t *buffer, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++) {
        // Wait for TX buffer empty
        SPI_WaitTXE(hspi);
        
        // Send data (8-bit)
        *((volatile uint16_t *)&hspi->Instance->DR) = buffer[i];
        
        // For full-duplex mode: wait for RXNE and read the dummy data
        // This prevents the extra dummy byte on next transmission
        if (hspi->Init.comm_mode == SPI_COMM_MODE_FULL_DUPLEX) {
            SPI_WaitRXNE(hspi);
            (void)hspi->Instance->DR;
        }
    }
    
    // Wait for last transmission to complete
    SPI_WaitNotBusy(hspi);
    
    // Additional drain: read any remaining data in RX FIFO
    while (SPI_GetFIFOLevel(hspi) > 0) {
        (void)hspi->Instance->DR;
    }
}

/**
 * @brief Receive buffer - 8-bit version (receive only mode)
 * @param hspi: Pointer to SPI handle structure
 * @param buffer: Buffer to store received data
 * @param size: Number of bytes to receive
 */
void SPI_ReceiveBuffer8(SPI_HandleTypeDef *hspi, uint8_t *buffer, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++) {
        // Wait for RX buffer not empty
        SPI_WaitRXNE(hspi);
        
        // Receive data (8-bit)
        buffer[i] = (uint8_t)hspi->Instance->DR;
    }
}

/**
 * @brief Transfer buffer - 8-bit version (full-duplex)
 * @param hspi: Pointer to SPI handle structure
 * @param tx_buffer: Transmit buffer
 * @param rx_buffer: Receive buffer
 * @param size: Number of bytes to transfer
 */
void SPI_TransferBuffer8(SPI_HandleTypeDef *hspi, uint8_t *tx_buffer, 
                         uint8_t *rx_buffer, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++) {
        // Wait for TX buffer empty
        SPI_WaitTXE(hspi);
        
        // Send data
        if (tx_buffer != 0) {
            hspi->Instance->DR = (uint16_t)tx_buffer[i];
        } else {
            hspi->Instance->DR = 0;
        }
        
        // Wait for RX buffer not empty
        SPI_WaitRXNE(hspi);
        
        // Receive data
        if (rx_buffer != 0) {
            rx_buffer[i] = (uint8_t)hspi->Instance->DR;
        } else {
            (void)hspi->Instance->DR;
        }
    }
    
    // Flush any remaining data in RX FIFO to prevent issues on next transmission
    SPI_FlushRX(hspi);
}

/**
 * @brief Transfer buffer (full-duplex)
 * @param hspi: Pointer to SPI handle structure
 * @param tx_buffer: Transmit buffer
 * @param rx_buffer: Receive buffer
 * @param size: Number of data frames
 */
void SPI_TransferBuffer(SPI_HandleTypeDef *hspi, uint16_t *tx_buffer, 
                        uint16_t *rx_buffer, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++) {
        // Wait for TX buffer empty
        SPI_WaitTXE(hspi);
        
        // Send data
        if (tx_buffer != 0) {
            hspi->Instance->DR = tx_buffer[i];
        } else {
            hspi->Instance->DR = 0;
        }
        
        // Wait for RX buffer not empty
        SPI_WaitRXNE(hspi);
        
        // Receive data
        if (rx_buffer != 0) {
            rx_buffer[i] = (uint16_t)hspi->Instance->DR;
        } else {
            (void)hspi->Instance->DR;
        }
    }
    
    // Flush any remaining data in RX FIFO to prevent issues on next transmission
    SPI_FlushRX(hspi);
}

/**
 * @brief Send buffer (transmit only mode)
 * @param hspi: Pointer to SPI handle structure
 * @param buffer: Data buffer to send
 * @param size: Number of data frames
 */
void SPI_SendBuffer(SPI_HandleTypeDef *hspi, uint16_t *buffer, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++) {
        // Wait for TX buffer empty
        SPI_WaitTXE(hspi);
        
        // Send data
        hspi->Instance->DR = buffer[i];
        
        // For full-duplex mode: wait for RXNE and read the dummy data
        // This prevents the extra dummy byte on next transmission
        if (hspi->Init.comm_mode == SPI_COMM_MODE_FULL_DUPLEX) {
            SPI_WaitRXNE(hspi);
            (void)hspi->Instance->DR;
        }
    }
    
    // Wait for last transmission to complete
    SPI_WaitNotBusy(hspi);
    
    // Additional drain: read any remaining data in RX FIFO
    // This ensures no pending data causes issues on next transmission
    while (SPI_GetFIFOLevel(hspi) > 0) {
        (void)hspi->Instance->DR;
    }
}

/**
 * @brief Receive buffer (receive only mode)
 * @param hspi: Pointer to SPI handle structure
 * @param buffer: Buffer to store received data
 * @param size: Number of data frames to receive
 */
void SPI_ReceiveBuffer(SPI_HandleTypeDef *hspi, uint16_t *buffer, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++) {
        // Wait for RX buffer not empty
        SPI_WaitRXNE(hspi);
        
        // Receive data
        buffer[i] = (uint16_t)hspi->Instance->DR;
    }
}

//============================================================================
// CRC Functions
//============================================================================

/**
 * @brief Set CRC polynomial
 * @param hspi: Pointer to SPI handle structure
 * @param polynomial: CRC polynomial (default is 7 for CRC-8)
 */
void SPI_SetCRCPolynomial(SPI_HandleTypeDef *hspi, uint8_t polynomial)
{
    hspi->Instance->CRCPR = polynomial;
}

/**
 * @brief Get TX CRC value
 * @param hspi: Pointer to SPI handle structure
 * @return TX CRC value
 */
uint16_t SPI_GetTXCRC(SPI_HandleTypeDef *hspi)
{
    return hspi->Instance->TXCRCR;
}

/**
 * @brief Get RX CRC value
 * @param hspi: Pointer to SPI handle structure
 * @return RX CRC value
 */
uint16_t SPI_GetRXCRC(SPI_HandleTypeDef *hspi)
{
    return hspi->Instance->RXCRCR;
}

/**
 * @brief Reset CRC values
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_ResetCRC(SPI_HandleTypeDef *hspi)
{
    // Disable CRC
    hspi->Instance->CR1 &= ~SPI_CR1_CRCEN;
    
    // Enable CRC (this resets the CRC)
    hspi->Instance->CR1 |= SPI_CR1_CRCEN;
}

/**
 * @brief Check for CRC error
 * @param hspi: Pointer to SPI handle structure
 * @return true if CRC error occurred
 */
bool SPI_CheckCRCError(SPI_HandleTypeDef *hspi)
{
    return (hspi->Instance->SR & SPI_SR_CRCERR) != 0;
}

/**
 * @brief Clear CRC error flag
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_ClearCRCError(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->SR &= ~SPI_SR_CRCERR;
}

/**
 * @brief Initiate CRC transmission
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_TransmitCRC(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
}

//============================================================================
// Status Functions
//============================================================================

/**
 * @brief Get SPI flag status
 * @param hspi: Pointer to SPI handle structure
 * @param flag: Flag to check
 * @return true if flag is set
 */
bool SPI_GetFlagStatus(SPI_HandleTypeDef *hspi, uint32_t flag)
{
    return (hspi->Instance->SR & flag) != 0;
}

/**
 * @brief Check if TX buffer is empty
 * @param hspi: Pointer to SPI handle structure
 * @return true if TX buffer is empty
 */
bool SPI_IsTXE(SPI_HandleTypeDef *hspi)
{
    return (hspi->Instance->SR & SPI_SR_TXE) != 0;
}

/**
 * @brief Check if RX buffer is not empty
 * @param hspi: Pointer to SPI handle structure
 * @return true if RX buffer has data
 */
bool SPI_IsRXNE(SPI_HandleTypeDef *hspi)
{
    return (hspi->Instance->SR & SPI_SR_RXNE) != 0;
}

/**
 * @brief Check if SPI is busy
 * @param hspi: Pointer to SPI handle structure
 * @return true if SPI is busy
 */
bool SPI_IsBusy(SPI_HandleTypeDef *hspi)
{
    return (hspi->Instance->SR & SPI_SR_BSY) != 0;
}

/**
 * @brief Get FIFO level
 * @param hspi: Pointer to SPI handle structure
 * @return FIFO level (0-3)
 */
uint8_t SPI_GetFIFOLevel(SPI_HandleTypeDef *hspi)
{
    return (uint8_t)((hspi->Instance->SR & SPI_SR_FRLVL) >> 11);
}

//============================================================================
// Wait Functions
//============================================================================

/**
 * @brief Wait for TX buffer to be empty
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_WaitTXE(SPI_HandleTypeDef *hspi)
{
    while (!SPI_IsTXE(hspi));
}

/**
 * @brief Wait for RX buffer to have data
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_WaitRXNE(SPI_HandleTypeDef *hspi)
{
    while (!SPI_IsRXNE(hspi));
}

/**
 * @brief Wait for SPI to be not busy
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_WaitNotBusy(SPI_HandleTypeDef *hspi)
{
    while (SPI_IsBusy(hspi));
}

/**
 * @brief Wait for TX FIFO to be empty
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_WaitFIFOEmpty(SPI_HandleTypeDef *hspi)
{
    while ((hspi->Instance->SR & SPI_SR_FTLVL) != 0);
}

/**
 * @brief Flush RX buffer (drain any pending data)
 * @param hspi: Pointer to SPI handle structure
 * @note This function reads and discards all pending data in the RX FIFO
 *       to prevent dummy byte issues on subsequent transmissions
 */
void SPI_FlushRX(SPI_HandleTypeDef *hspi)
{
    // Read and discard all data in RX FIFO
    while (SPI_GetFIFOLevel(hspi) > 0) {
        (void)hspi->Instance->DR;
    }
    
    // Also clear any pending overrun flag
    hspi->Instance->SR &= ~SPI_SR_OVR;
}

/**
 * @brief Flush TX buffer (drain any pending data)
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_FlushTX(SPI_HandleTypeDef *hspi)
{
    // Wait until TX FIFO is empty
    SPI_WaitFIFOEmpty(hspi);
    
    // Wait for BSY to clear
    SPI_WaitNotBusy(hspi);
}

//============================================================================
// NSS Pulse Management
//============================================================================

/**
 * @brief Enable NSS pulse mode
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_EnableNSSP(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR2 |= SPI_CR2_NSSP;
}

/**
 * @brief Disable NSS pulse mode
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_DisableNSSP(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR2 &= ~SPI_CR2_NSSP;
}

//============================================================================
// TI Protocol Mode
//============================================================================

/**
 * @brief Enable TI protocol mode
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_EnableTIProtocol(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR2 |= SPI_CR2_FRF;
}

/**
 * @brief Disable TI protocol mode
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_DisableTIProtocol(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR2 &= ~SPI_CR2_FRF;
}

//============================================================================
// DMA Support
//============================================================================

/**
 * @brief Enable DMA for TX
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_EnableDMA_TX(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR2 |= SPI_CR2_TXDMAEN;
}

/**
 * @brief Disable DMA for TX
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_DisableDMA_TX(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR2 &= ~SPI_CR2_TXDMAEN;
}

/**
 * @brief Enable DMA for RX
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_EnableDMA_RX(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR2 |= SPI_CR2_RXDMAEN;
}

/**
 * @brief Disable DMA for RX
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_DisableDMA_RX(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->CR2 &= ~SPI_CR2_RXDMAEN;
}

//============================================================================
// Interrupt Support
//============================================================================

/**
 * @brief Enable SPI interrupt
 * @param hspi: Pointer to SPI handle structure
 * @param interrupt: Interrupt to enable
 */
void SPI_EnableInterrupt(SPI_HandleTypeDef *hspi, uint32_t interrupt)
{
    hspi->Instance->CR2 |= interrupt;
}

/**
 * @brief Disable SPI interrupt
 * @param hspi: Pointer to SPI handle structure
 * @param interrupt: Interrupt to disable
 */
void SPI_DisableInterrupt(SPI_HandleTypeDef *hspi, uint32_t interrupt)
{
    hspi->Instance->CR2 &= ~interrupt;
}

//============================================================================
// Low-level Disable Procedure
//============================================================================

/**
 * @brief Proper SPI disable procedure (follows STM32 reference manual)
 * @param hspi: Pointer to SPI handle structure
 */
void SPI_DisablePeripheral(SPI_HandleTypeDef *hspi)
{
    // Wait until FTLVL[1:0] = 00 (no more data to transmit)
    while ((hspi->Instance->SR & SPI_SR_FTLVL) != 0);
    
    // Wait until BSY = 0 (the last data frame is processed)
    while ((hspi->Instance->SR & SPI_SR_BSY) != 0);
    
    // Disable the SPI
    hspi->Instance->CR1 &= ~SPI_CR1_SPE;
    
    // Read data until FRLVL[1:0] = 00 (read all the received data)
    while ((hspi->Instance->SR & SPI_SR_FRLVL) != 0) {
        (void)hspi->Instance->DR;
    }
}

//============================================================================
// Helper Functions for Creating SPI Handle
//============================================================================

/**
 * @brief Get SPI handle for specified instance
 * @param instance: SPI instance
 * @return Pointer to SPI handle
 */
SPI_HandleTypeDef* SPI_GetHandle(SPI_Instance instance)
{
    return &g_spi_handle[instance];
}
