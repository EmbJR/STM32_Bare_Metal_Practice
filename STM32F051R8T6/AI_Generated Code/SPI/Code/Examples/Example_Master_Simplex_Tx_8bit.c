/**
 * @file Example_Master_Simplex_Tx_8bit.c
 * @brief SPI Master Mode - Simplex Transmit Only - 8-bit Data Example
 * @description This example demonstrates SPI communication in master mode
 *              with simplex (transmit-only) configuration and 8-bit data frame.
 * 
 * Hardware Setup:
 * - SPI1: PA5 (SCK), PA7 (MOSI)
 * - Connect SCK to slave SCK
 * - Connect MOSI to slave MOSI
 * - NSS can be GPIO or not used
 * 
 * Note: In simplex mode, only the MOSI line is used for transmission.
 *       The MISO line is free and can be used as GPIO.
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

// Buffer size
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

//============================================================================
// Function Prototypes
//============================================================================

void SystemClock_Config(void);
void SPI1_GPIO_Init(void);
void SPI1_Init_Master_Simplex_Tx_8bit(void);
void FillTxBuffer(void);

//============================================================================
// Main Function
//============================================================================

int main(void)
{
    // Configure system clock
    SystemClock_Config();
    
    // Initialize GPIO for SPI1 (simplex - only SCK and MOSI needed)
    SPI1_GPIO_Init();
    
    // Initialize SPI1 as master, simplex transmit-only, 8-bit
    SPI1_Init_Master_Simplex_Tx_8bit();
    
    // Fill transmit buffer
    FillTxBuffer();
    
    printf("SPI Master Simplex Transmit-Only 8-bit Example\r\n");
    printf("===============================================\r\n\r\n");
    
    // Send data (simplex - transmit only)
    printf("Sending data...\r\n");
    SPI_SendBuffer(&hspi1, (uint16_t*)tx_buffer, BUFFER_SIZE);
    
    printf("Data sent successfully!\r\n");
    
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
// GPIO Initialization (Simplex - Only SCK and MOSI)
//============================================================================

void SPI1_GPIO_Init(void)
{
    // Enable GPIOA clock
    RCC_APB2ENR |= (1 << 2);
    
    // PA4 - NSS (optional, used as GPIO here)
    GPIOA->MODER &= ~((0x03 << (4 * 2)));
    GPIOA->MODER |= (0x01 << (4 * 2));  // Output
    GPIOA->OTYPER &= ~(1 << 4);
    GPIOA->OSPEEDR &= ~((0x03 << (4 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (4 * 2));
    GPIOA->ODR |= (1 << 4);  // NSS high
    
    // PA5 - SCK - Alternate Function
    GPIOA->MODER &= ~((0x03 << (5 * 2)));
    GPIOA->MODER |= (0x02 << (5 * 2));
    GPIOA->OTYPER &= ~(1 << 5);
    GPIOA->OSPEEDR &= ~((0x03 << (5 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (5 * 2));
    GPIOA->AFRL &= ~((0x0F << (5 * 4)));
    GPIOA->AFRL |= (0x00 << (5 * 4));
    
    // PA7 - MOSI - Alternate Function (only needed for simplex TX)
    GPIOA->MODER &= ~((0x03 << (7 * 2)));
    GPIOA->MODER |= (0x02 << (7 * 2));
    GPIOA->OTYPER &= ~(1 << 7);
    GPIOA->OSPEEDR &= ~((0x03 << (7 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (7 * 2));
    GPIOA->AFRL &= ~((0x0F << (7 * 4)));
    GPIOA->AFRL |= (0x00 << (7 * 4));
    
    // Note: PA6 (MISO) can be used as GPIO in simplex mode
}

//============================================================================
// SPI Initialization - Master Simplex Transmit-Only 8-bit
//============================================================================

void SPI1_Init_Master_Simplex_Tx_8bit(void)
{
    // Initialize SPI handle
    hspi1.Instance = SPI_HW;
    
    // Configure SPI for simplex transmit-only mode
    hspi1.Init.mode = SPI_MODE_MASTER;                    // Master
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW; // CPOL = 0
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE; // CPHA = 0
    hspi1.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1.Init.data_size = SPI_DATA_SIZE_8BIT;           // 8-bit
    hspi1.Init.baud_rate = SPI_BAUD_RATE_DIV_8;
    hspi1.Init.comm_mode = SPI_COMM_MODE_TRANSMIT_ONLY; // Simplex TX
    hspi1.Init.nss_mode = SPI_NSS_SOFTWARE;
    hspi1.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1.Init.rx_fifo_threshold = SPI_FIFO_THRESHOLD_1_2;
    hspi1.Init.crc_enabled = false;
    hspi1.Init.crc_polynomial = 7;
    hspi1.Init.nss_pulse_enabled = false;
    
    // Initialize SPI
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
    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer[i] = 0xAA;  // Pattern: 10101010
    }
}

//============================================================================
// End of File
//============================================================================
