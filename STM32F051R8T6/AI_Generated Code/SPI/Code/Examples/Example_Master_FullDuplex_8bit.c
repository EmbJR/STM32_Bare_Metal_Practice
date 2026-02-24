/**
 * @file Example_Master_FullDuplex_8bit.c
 * @brief SPI Master Mode - Full Duplex - 8-bit Data Example
 * @description This example demonstrates SPI communication in master mode
 *              with full-duplex configuration and 8-bit data frame size.
 * 
 * Hardware Setup:
 * - SPI1: PA5 (SCK), PA6 (MISO), PA7 (MOSI)
 * - Connect SCK to slave SCK
 * - Connect MISO to slave MISO  
 * - Connect MOSI to slave MOSI
 * - Connect NSS (PA4) to slave NSS (or use software NSS)
 */

#include "spiF051.h"
#include "gpio.h"
#include "rcc.h"
#include <stdio.h>

//============================================================================
// Register Definitions (Direct Access)
//============================================================================

// RCC Registers
#define RCC_BASE        0x40021000UL
#define RCC_CR         (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR       (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_APB2ENR    (*(volatile uint32_t *)(RCC_BASE + 0x24))

// GPIO Registers
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
// Configuration
//============================================================================

// SPI1 Pin Configuration (STM32F051R8T6)
// PA4 = NSS (Slave Select) - Software control
// PA5 = SCK (Serial Clock) - Alternate Function 0
// PA6 = MISO (Master In Slave Out) - Alternate Function 0
// PA7 = MOSI (Master Out Slave In) - Alternate Function 0

#define SPI_INSTANCE    SPI_INSTANCE_1
#define SPI_HW         SPI1

// Buffer sizes
#define BUFFER_SIZE    16

//============================================================================
// Global Variables
//============================================================================

// SPI Handle
SPI_HandleTypeDef hspi1;

// Data buffers
uint8_t tx_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];

//============================================================================
// Function Prototypes
//============================================================================

void SystemClock_Config(void);
void SPI1_GPIO_Init(void);
void SPI1_Init_Master_FullDuplex_8bit(void);
void FillTxBuffer(void);
void PrintBuffers(void);

//============================================================================
// Main Function
//============================================================================

int main(void)
{
    // Configure system clock
    SystemClock_Config();
    
    // Initialize GPIO for SPI1
    SPI1_GPIO_Init();
    
    // Initialize SPI1 as master, full-duplex, 8-bit
    SPI1_Init_Master_FullDuplex_8bit();
    
    // Fill transmit buffer with test data
    FillTxBuffer();
    
    printf("SPI Master Full-Duplex 8-bit Example\r\n");
    printf("=====================================\r\n\r\n");
    
    // Perform full-duplex transfer
    printf("Performing full-duplex transfer...\r\n");
    SPI_TransferBuffer(&hspi1, (uint16_t*)tx_buffer, (uint16_t*)rx_buffer, BUFFER_SIZE);
    
    // Print results
    PrintBuffers();
    
    // Wait for transmission to complete
    SPI_WaitNotBusy(&hspi1);
    
    printf("\r\nTransfer complete!\r\n");
    
    // Disable SPI
    SPI_Disable(&hspi1);
    
    while (1) {
        // Main loop - add your application code here
    }
}

//============================================================================
// Clock Configuration
//============================================================================

void SystemClock_Config(void)
{
    // Configure PLL for 48MHz system clock
    // HSI = 8MHz, PLL multiplier = 6, PLL clock = 48MHz
    
    // Enable HSI
    RCC_CR |= (1 << 0);  // HSION
    
    // Wait for HSI to be ready
    while (!(RCC_CR & (1 << 1)));
    
    // Configure PLL
    RCC_CFGR = (6 << 18);  // PLLMUL = 6 (8MHz * 6 = 48MHz)
    RCC_CFGR |= (1 << 16); // PLLSRC = HSI/2
    
    // Enable PLL
    RCC_CR |= (1 << 24);  // PLLON
    
    // Wait for PLL to be ready
    while (!(RCC_CR & (1 << 25)));
    
    // Configure flash wait states
    // FLASH->ACR = 0x00000011; // 1 wait state for 48MHz
    
    // Select PLL as system clock
    RCC_CFGR = (RCC_CFGR & ~0x03) | (0x02); // SW = PLL
    
    // Wait for PLL to be selected
    while (((RCC_CFGR >> 2) & 0x03) != 0x02);
}

//============================================================================
// GPIO Initialization
//============================================================================

void SPI1_GPIO_Init(void)
{
    // Enable GPIOA clock
    RCC_APB2ENR |= (1 << 2);  // IOPAEN
    
    // Configure GPIO pins
    // PA4 = NSS (Output, Push-Pull)
    // PA5 = SCK (Alternate Function, Push-Pull, High Speed)
    // PA6 = MISO (Alternate Function, Push-Pull, High Speed)
    // PA7 = MOSI (Alternate Function, Push-Pull, High Speed)
    
    // PA4 - NSS (Software NSS mode)
    GPIOA->MODER &= ~((0x03 << (4 * 2)));  // Clear
    GPIOA->MODER |= (0x01 << (4 * 2));     // Output
    GPIOA->OTYPER &= ~(1 << 4);            // Push-Pull
    GPIOA->OSPEEDR &= ~((0x03 << (4 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (4 * 2));    // High Speed
    
    // PA5 - SCK
    GPIOA->MODER &= ~((0x03 << (5 * 2)));  // Clear
    GPIOA->MODER |= (0x02 << (5 * 2));     // Alternate Function
    GPIOA->OTYPER &= ~(1 << 5);            // Push-Pull
    GPIOA->OSPEEDR &= ~((0x03 << (5 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (5 * 2));   // High Speed
    GPIOA->AFRL &= ~((0x0F << (5 * 4)));   // Clear
    GPIOA->AFRL |= (0x00 << (5 * 4));      // AF0 (SPI1)
    
    // PA6 - MISO
    GPIOA->MODER &= ~((0x03 << (6 * 2)));  // Clear
    GPIOA->MODER |= (0x02 << (6 * 2));     // Alternate Function
    GPIOA->OTYPER &= ~(1 << 6);            // Push-Pull
    GPIOA->OSPEEDR &= ~((0x03 << (6 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (6 * 2));   // High Speed
    GPIOA->AFRL &= ~((0x0F << (6 * 4)));   // Clear
    GPIOA->AFRL |= (0x00 << (6 * 4));      // AF0 (SPI1)
    
    // PA7 - MOSI
    GPIOA->MODER &= ~((0x03 << (7 * 2)));  // Clear
    GPIOA->MODER |= (0x02 << (7 * 2));     // Alternate Function
    GPIOA->OTYPER &= ~(1 << 7);            // Push-Pull
    GPIOA->OSPEEDR &= ~((0x03 << (7 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (7 * 2));   // High Speed
    GPIOA->AFRL &= ~((0x0F << (7 * 4)));   // Clear
    GPIOA->AFRL |= (0x00 << (7 * 4));      // AF0 (SPI1)
}

//============================================================================
// SPI Initialization - Master Full-Duplex 8-bit
//============================================================================

void SPI1_Init_Master_FullDuplex_8bit(void)
{
    // Initialize SPI handle
    hspi1.Instance = SPI_HW;
    
    // Configure SPI
    hspi1.Init.mode = SPI_MODE_MASTER;                    // Master mode
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;  // CPOL = 0 (clock idle low)
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;   // CPHA = 0 (first clock transition)
    hspi1.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST; // MSB first
    hspi1.Init.data_size = SPI_DATA_SIZE_8BIT;           // 8-bit data
    hspi1.Init.baud_rate = SPI_BAUD_RATE_DIV_8;          // Baud rate = Fcpu/8 = 6MHz
    hspi1.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;    // Full-duplex
    hspi1.Init.nss_mode = SPI_NSS_SOFTWARE;              // Software NSS
    hspi1.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;     // Motorola SPI
    hspi1.Init.rx_fifo_threshold = SPI_FIFO_THRESHOLD_1_2; // RX FIFO 1/2
    hspi1.Init.crc_enabled = false;                       // No CRC
    hspi1.Init.crc_polynomial = 7;                        // Default CRC polynomial
    hspi1.Init.nss_pulse_enabled = false;                 // No NSS pulse
    
    // Initialize SPI
    SPI_Init(&hspi1);
    
    // Enable SPI
    SPI_Enable(&hspi1);
    
    // Set NSS high (inactive) for software NSS mode
    SPI_SetNSSPin(&hspi1, true);
}

//============================================================================
// Helper Functions
//============================================================================

void FillTxBuffer(void)
{
    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer[i] = i;  // 0x00, 0x01, 0x02, ..., 0x0F
    }
}

void PrintBuffers(void)
{
    printf("TX Buffer: ");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("%02X ", tx_buffer[i]);
    }
    printf("\r\n");
    
    printf("RX Buffer: ");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("%02X ", rx_buffer[i]);
    }
    printf("\r\n");
}

//============================================================================
// Optional: Simple Transfer Function (Polling)
//============================================================================

/**
 * @brief Simple single-byte transfer
 * @param data: Data to send
 * @return Received data
 */
uint8_t SPI_SimpleTransfer(uint8_t data)
{
    // Set NSS low (active)
    SPI_SetNSSPin(&hspi1, false);
    
    // Wait for TX buffer empty
    while (!SPI_IsTXE(&hspi1));
    
    // Send data
    SPI_SendData8(&hspi1, data);
    
    // Wait for RX buffer not empty
    while (!SPI_IsRXNE(&hspi1));
    
    // Receive data
    uint8_t received = SPI_ReceiveData8(&hspi1);
    
    // Wait for transmission complete
    SPI_WaitNotBusy(&hspi1);
    
    // Set NSS high (inactive)
    SPI_SetNSSPin(&hspi1, true);
    
    return received;
}

//============================================================================
// Interrupt Handlers (if needed)
//============================================================================

// void SPI1_IRQHandler(void)
// {
//     // Add interrupt handling code here
// }

//============================================================================
// End of File
//============================================================================
