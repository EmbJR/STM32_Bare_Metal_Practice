/**
 * @file Example_Master_CRC_8bit.c
 * @brief SPI Master Mode - Full Duplex with CRC Example
 * @description This example demonstrates SPI communication in master mode
 *              with CRC enabled for data integrity checking.
 * 
 * Hardware Setup:
 * - SPI1: PA5 (SCK), PA6 (MISO), PA7 (MOSI)
 * - Connect SCK to slave SCK
 * - Connect MISO to slave MISO  
 * - Connect MOSI to slave MOSI
 * - Connect NSS (PA4) to slave NSS
 */

#include "spiF051.h"
#include "gpio.h"
#include "rcc.h"
#include <stdio.h>

//============================================================================
// Configuration
//============================================================================

#define BUFFER_SIZE    8

// CRC Polynomial (default is 7 for CRC-8)
// For CRC-8: polynomial = 0x07 (x^8 + x^2 + x + 1)
// For CRC-16: polynomial = 0x1021 (x^16 + x^12 + x^5 + 1)

#define CRC_POLYNOMIAL  0x07  // CRC-8 polynomial

//============================================================================
// Register Definitions - Using spiF051.h definitions instead
//============================================================================

#define GPIOA_BASE      0x48000000UL
#define GPIOA          ((GPIO_TypeDef *)GPIOA_BASE)

typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFRL;
    volatile uint32_t AFRH;
    volatile uint32_t BRR;
} GPIO_TypeDef;

//============================================================================
// Global Variables
//============================================================================

SPI_HandleTypeDef hspi1;
uint8_t tx_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];

//============================================================================
// Function Prototypes
//============================================================================

void SystemClock_Config(void);
void SPI1_GPIO_Init(void);
void SPI1_Init_Master_CRC_8bit(void);
void FillTxBuffer(void);

//============================================================================
// Main Function
//============================================================================

int main(void)
{
    // Configure system clock
    SystemClock_Config();
    
    // Initialize GPIO for SPI1
    SPI1_GPIO_Init();
    
    // Initialize SPI1 as master with CRC enabled
    SPI1_Init_Master_CRC_8bit();
    
    // Fill transmit buffer
    FillTxBuffer();
    
    printf("SPI Master with CRC Example\r\n");
    printf("===========================\r\n\r\n");
    
    // Print TX buffer
    printf("TX Buffer: ");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("%02X ", tx_buffer[i]);
    }
    printf("\r\n");
    
    // Transfer data with CRC
    // Note: For CRC transfer, we need to set CRCNEXT after last data
    
    // Set NSS low
    SPI_SetNSSPin(&hspi1, false);
    
    // Reset CRC before communication (important!)
    SPI_ResetCRC(&hspi1);
    
    // Send all data
    for (int i = 0; i < BUFFER_SIZE; i++) {
        // Wait for TX buffer empty
        while (!SPI_IsTXE(&hspi1));
        
        // Send data
        SPI_SendData8(&hspi1, tx_buffer[i]);
        
        // Wait for RX buffer not empty
        while (!SPI_IsRXNE(&hspi1));
        
        // Read received data
        rx_buffer[i] = SPI_ReceiveData8(&hspi1);
    }
    
    // Set CRCNEXT to transmit CRC after last data
    SPI_TransmitCRC(&hspi1);
    
    // Wait for transmission complete
    SPI_WaitNotBusy(&hspi1);
    
    // Wait for CRC reception (in full-duplex, CRC is received after data)
    // Wait for RXNE flag to indicate CRC has been received
    while (!SPI_IsRXNE(&hspi1));
    
    // Read the CRC value from RX register (dummy read to clear RXNE)
    volatile uint8_t dummy = SPI_ReceiveData8(&hspi1);
    (void)dummy;  // Suppress unused variable warning
    
    // Check for CRC error
    if (SPI_CheckCRCError(&hspi1)) {
        printf("CRC ERROR: Data corruption detected!\r\n");
        SPI_ClearCRCError(&hspi1);
    } else {
        printf("CRC OK: No data corruption detected.\r\n");
    }
    
    // Print received data
    printf("RX Buffer: ");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("%02X ", rx_buffer[i]);
    }
    printf("\r\n");
    
    // Print CRC values
    printf("TX CRC: %02X\r\n", SPI_GetTXCRC(&hspi1));
    printf("RX CRC: %02X\r\n", SPI_GetRXCRC(&hspi1));
    
    // Disable SPI
    SPI_Disable(&hspi1);
    
    while (1) {
        // Main loop
    }
}

//============================================================================
// Clock Configuration
//============================================================================

void SystemClock_Config(void)
{
    // Enable HSI
    RCC_CR |= (1 << 0);
    
    // Wait for HSI ready
    while (!(RCC_CR & (1 << 1)));
    
    // Configure PLL
    RCC_CFGR = (6 << 18);
    RCC_CFGR |= (1 << 16);
    
    // Enable PLL
    RCC_CR |= (1 << 24);
    
    // Wait for PLL ready
    while (!(RCC_CR & (1 << 25)));
    
    // Select PLL as system clock
    RCC_CFGR = (RCC_CFGR & ~0x03) | (0x02);
    
    // Wait for PLL selected
    while (((RCC_CFGR >> 2) & 0x03) != 0x02);
}

//============================================================================
// GPIO Initialization
//============================================================================

void SPI1_GPIO_Init(void)
{
    // Enable GPIOA clock
    RCC_APB2ENR |= (1 << 2);
    
    // PA4 - NSS
    GPIOA->MODER &= ~((0x03 << (4 * 2)));
    GPIOA->MODER |= (0x01 << (4 * 2));
    GPIOA->OTYPER &= ~(1 << 4);
    GPIOA->OSPEEDR &= ~((0x03 << (4 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (4 * 2));
    
    // PA5 - SCK
    GPIOA->MODER &= ~((0x03 << (5 * 2)));
    GPIOA->MODER |= (0x02 << (5 * 2));
    GPIOA->OTYPER &= ~(1 << 5);
    GPIOA->OSPEEDR &= ~((0x03 << (5 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (5 * 2));
    GPIOA->AFRL &= ~((0x0F << (5 * 4)));
    GPIOA->AFRL |= (0x00 << (5 * 4));
    
    // PA6 - MISO
    GPIOA->MODER &= ~((0x03 << (6 * 2)));
    GPIOA->MODER |= (0x02 << (6 * 2));
    GPIOA->OTYPER &= ~(1 << 6);
    GPIOA->OSPEEDR &= ~((0x03 << (6 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (6 * 2));
    GPIOA->AFRL &= ~((0x0F << (6 * 4)));
    GPIOA->AFRL |= (0x00 << (6 * 4));
    
    // PA7 - MOSI
    GPIOA->MODER &= ~((0x03 << (7 * 2)));
    GPIOA->MODER |= (0x02 << (7 * 2));
    GPIOA->OTYPER &= ~(1 << 7);
    GPIOA->OSPEEDR &= ~((0x03 << (7 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (7 * 2));
    GPIOA->AFRL &= ~((0x0F << (7 * 4)));
    GPIOA->AFRL |= (0x00 << (7 * 4));
}

//============================================================================
// SPI Initialization - Master with CRC
//============================================================================

void SPI1_Init_Master_CRC_8bit(void)
{
    // Initialize SPI handle - use SPI1 directly
    hspi1.Instance = SPI1;
    
    // Configure SPI with CRC enabled
    hspi1.Init.mode = SPI_MODE_MASTER;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;
    hspi1.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1.Init.data_size = SPI_DATA_SIZE_8BIT;
    hspi1.Init.baud_rate = SPI_BAUD_RATE_DIV_8;
    hspi1.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
    hspi1.Init.nss_mode = SPI_NSS_SOFTWARE;
    hspi1.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1.Init.rx_fifo_threshold = SPI_FIFO_THRESHOLD_1_2;
    hspi1.Init.crc_enabled = true;         // Enable CRC
    hspi1.Init.crc_polynomial = CRC_POLYNOMIAL;  // CRC-8 polynomial
    hspi1.Init.nss_pulse_enabled = false;
    
    // Initialize SPI (this enables the clock internally)
    SPI_Init(&hspi1);
    
    // Enable SPI
    SPI_Enable(&hspi1);
    
    // Set NSS high
    SPI_SetNSSPin(&hspi1, true);
}

//============================================================================
// Helper Functions
//============================================================================

void FillTxBuffer(void)
{
    // Test data pattern
    tx_buffer[0] = 0x01;
    tx_buffer[1] = 0x02;
    tx_buffer[2] = 0x03;
    tx_buffer[3] = 0x04;
    tx_buffer[4] = 0x05;
    tx_buffer[5] = 0x06;
    tx_buffer[6] = 0x07;
    tx_buffer[7] = 0x08;
}

//============================================================================
// CRC Communication Notes:
//============================================================================
/*
 * CRC Usage in STM32F0 SPI:
 * 
 * 1. Enable CRC by setting CRCEN in CR1
 * 2. Set CRC polynomial in CRCPR register
 * 3. Reset CRC before each communication by:
 *    - Clearing CRCEN
 *    - Setting CRCEN (this resets internal CRC)
 * 4. After sending all data frames, set CRCNEXT to transmit CRC
 * 5. In full-duplex, CRC is automatically received after data
 * 6. Check CRCERR flag in SR to detect errors
 * 7. Read RXCRC to get received CRC value
 * 8. Read TXCRC to get calculated CRC value
 * 
 * Note: CRC is calculated on both TX and RX independently.
 *       TXCRC is the CRC of transmitted data
 *       RXCRC is the CRC of received data
 *       In normal operation, these should match (if slave also has CRC enabled)
 */

//============================================================================
// End of File
//============================================================================
