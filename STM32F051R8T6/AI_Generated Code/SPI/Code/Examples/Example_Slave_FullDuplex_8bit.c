/**
 * @file Example_Slave_FullDuplex_8bit.c
 * @brief SPI Slave Mode - Full Duplex - 8-bit Data Example
 * @description This example demonstrates SPI communication in slave mode
 *              with full-duplex configuration and 8-bit data frame size.
 * 
 * Hardware Setup:
 * - SPI1: PA5 (SCK), PA6 (MISO), PA7 (MOSI)
 * - Connect SCK to master SCK
 * - Connect MISO to master MISO  
 * - Connect MOSI to master MOSI
 * - Connect NSS (PA4) to master NSS
 */

#include "spiF051.h"
#include "gpio.h"
#include "rcc.h"
#include <stdio.h>

//============================================================================
// Configuration
//============================================================================

#define SPI_INSTANCE    SPI_INSTANCE_1
#define SPI_HW         SPI1

// Buffer sizes
#define BUFFER_SIZE    16

//============================================================================
// Register Definitions
//============================================================================

#define RCC_BASE        0x40021000UL
#define RCC_CR         (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR       (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_APB2ENR    (*(volatile uint32_t *)(RCC_BASE + 0x24))

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
volatile bool data_received = false;

//============================================================================
// Function Prototypes
//============================================================================

void SystemClock_Config(void);
void SPI1_GPIO_Init(void);
void SPI1_Init_Slave_FullDuplex_8bit(void);
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
    
    // Initialize SPI1 as slave, full-duplex, 8-bit
    SPI1_Init_Slave_FullDuplex_8bit();
    
    // Fill transmit buffer
    FillTxBuffer();
    
    printf("SPI Slave Full-Duplex 8-bit Example\r\n");
    printf("===================================\r\n\r\n");
    
    // Clear receive buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        rx_buffer[i] = 0x00;
    }
    
    printf("Waiting for master to initiate transfer...\r\n");
    
    // Wait for data reception (polling)
    // In real application, you might use interrupts
    
    // Perform full-duplex transfer (slave responds to master)
    SPI_TransferBuffer(&hspi1, (uint16_t*)tx_buffer, (uint16_t*)rx_buffer, BUFFER_SIZE);
    
    // Print received data
    printf("Received data from master:\r\n");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("%02X ", rx_buffer[i]);
    }
    printf("\r\n");
    
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
    
    // PA4 - NSS (Hardware mode - input)
    GPIOA->MODER &= ~((0x03 << (4 * 2)));  // Input
    GPIOA->PUPDR &= ~((0x03 << (4 * 2)));
    GPIOA->PUPDR |= (0x01 << (4 * 2));     // Pull-up
    
    // PA5 - SCK (Input from master)
    GPIOA->MODER &= ~((0x03 << (5 * 2)));  // Input
    GPIOA->PUPDR &= ~((0x03 << (5 * 2)));
    GPIOA->PUPDR |= (0x02 << (5 * 2));     // Pull-down
    
    // PA6 - MISO (Alternate Function output)
    GPIOA->MODER &= ~((0x03 << (6 * 2)));
    GPIOA->MODER |= (0x02 << (6 * 2));     // Alternate Function
    GPIOA->OTYPER &= ~(1 << 6);            // Push-Pull
    GPIOA->OSPEEDR &= ~((0x03 << (6 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (6 * 2));
    GPIOA->AFRL &= ~((0x0F << (6 * 4)));
    GPIOA->AFRL |= (0x00 << (6 * 4));      // AF0
    
    // PA7 - MOSI (Input)
    GPIOA->MODER &= ~((0x03 << (7 * 2)));  // Input
    GPIOA->PUPDR &= ~((0x03 << (7 * 2)));
    GPIOA->PUPDR |= (0x01 << (7 * 2));     // Pull-up
}

//============================================================================
// SPI Initialization - Slave Full-Duplex 8-bit
//============================================================================

void SPI1_Init_Slave_FullDuplex_8bit(void)
{
    // Initialize SPI handle
    hspi1.Instance = SPI_HW;
    
    // Configure SPI for slave mode
    hspi1.Init.mode = SPI_MODE_SLAVE;                    // Slave mode
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;  // Must match master
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;  // Must match master
    hspi1.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1.Init.data_size = SPI_DATA_SIZE_8BIT;           // 8-bit
    // Baud rate is not used in slave mode
    hspi1.Init.baud_rate = SPI_BAUD_RATE_DIV_2;
    hspi1.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;    // Full-duplex
    hspi1.Init.nss_mode = SPI_NSS_HARDWARE_INPUT;       // Hardware NSS
    hspi1.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1.Init.rx_fifo_threshold = SPI_FIFO_THRESHOLD_1_2;
    hspi1.Init.crc_enabled = false;
    hspi1.Init.crc_polynomial = 7;
    hspi1.Init.nss_pulse_enabled = false;
    
    // Initialize SPI
    SPI_Init(&hspi1);
    
    // Enable SPI
    SPI_Enable(&hspi1);
}

//============================================================================
// Helper Functions
//============================================================================

void FillTxBuffer(void)
{
    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer[i] = 0x55 + i;  // Pattern: 0x55, 0x56, 0x57...
    }
}

//============================================================================
// Interrupt Handler Example (for data reception)
//============================================================================

/**
 * @brief This function handles SPI1 interrupt
 * @note This is just a template - actual implementation depends on needs
 */
// void SPI1_IRQHandler(void)
// {
//     if (SPI_IsRXNE(&hspi1)) {
//         // Read received data
//         uint8_t data = SPI_ReceiveData8(&hspi1);
//         
//         // Process data (example: store in buffer)
//         // rx_buffer[rx_index++] = data;
//         
//         data_received = true;
//     }
//     
//     // Handle other interrupts (errors, etc.)
// }

//============================================================================
// End of File
//============================================================================
