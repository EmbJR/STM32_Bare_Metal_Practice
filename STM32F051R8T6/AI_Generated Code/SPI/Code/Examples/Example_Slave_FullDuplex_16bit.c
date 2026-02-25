/**
 * @file Example_Slave_FullDuplex_16bit.c
 * @brief SPI Slave Mode - Full Duplex - 16-bit Data Example
 * @description This example demonstrates SPI communication in slave mode
 *              with full-duplex configuration and 16-bit data frame size.
 */

#include "spiF051.h"
#include "gpio.h"
#include "rcc.h"
#include <stdio.h>

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
// Configuration
//============================================================================

#define BUFFER_SIZE    8

//============================================================================
// Global Variables
//============================================================================

SPI_HandleTypeDef hspi1;
uint16_t tx_buffer[BUFFER_SIZE];
uint16_t rx_buffer[BUFFER_SIZE];

//============================================================================
// Main Function
//============================================================================

int main(void)
{
    SystemClock_Config();
    SPI1_GPIO_Init();
    SPI1_Init_Slave_16bit();
    
    printf("SPI Slave Full-Duplex 16-bit Example\r\n");
    printf("===================================\r\n\r\n");
    
    // Fill response buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer[i] = 0xAA00 | i;
    }
    
    // Clear receive buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        rx_buffer[i] = 0;
    }
    
    printf("Waiting for master...\r\n");
    
    // Full-duplex transfer
    SPI_TransferBuffer(&hspi1, tx_buffer, rx_buffer, BUFFER_SIZE);
    
    // Print received data
    printf("Received from master:\r\n");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("%04X ", rx_buffer[i]);
    }
    printf("\r\n");
    
    SPI_Disable(&hspi1);
    
    while (1);
}

//============================================================================
// Clock Configuration
//============================================================================

void SystemClock_Config(void)
{
    RCC_CR |= (1 << 0);
    while (!(RCC_CR & (1 << 1)));
    RCC_CFGR = (6 << 18);
    RCC_CFGR |= (1 << 16);
    RCC_CR |= (1 << 24);
    while (!(RCC_CR & (1 << 25)));
    RCC_CFGR = (RCC_CFGR & ~0x03) | (0x02);
    while (((RCC_CFGR >> 2) & 0x03) != 0x02);
}

//============================================================================
// GPIO Initialization
//============================================================================

void SPI1_GPIO_Init(void)
{
    RCC_APB2ENR |= (1 << 2);
    
    // PA4 - NSS (Input)
    GPIOA->MODER &= ~((0x03 << (4 * 2)));
    GPIOA->PUPDR &= ~((0x03 << (4 * 2)));
    GPIOA->PUPDR |= (0x01 << (4 * 2));
    
    // PA5 - SCK (Input)
    GPIOA->MODER &= ~((0x03 << (5 * 2)));
    GPIOA->PUPDR &= ~((0x03 << (5 * 2)));
    GPIOA->PUPDR |= (0x02 << (5 * 2));
    
    // PA6 - MISO (Output)
    GPIOA->MODER &= ~((0x03 << (6 * 2)));
    GPIOA->MODER |= (0x02 << (6 * 2));
    GPIOA->OTYPER &= ~(1 << 6);
    GPIOA->OSPEEDR &= ~((0x03 << (6 * 2)));
    GPIOA->OSPEEDR |= (0x03 << (6 * 2));
    GPIOA->AFRL &= ~((0x0F << (6 * 4)));
    GPIOA->AFRL |= (0x00 << (6 * 4));
    
    // PA7 - MOSI (Input)
    GPIOA->MODER &= ~((0x03 << (7 * 2)));
    GPIOA->PUPDR &= ~((0x03 << (7 * 2)));
    GPIOA->PUPDR |= (0x01 << (7 * 2));
}

//============================================================================
// SPI Initialization - Slave 16-bit
//============================================================================

void SPI1_Init_Slave_16bit(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.mode = SPI_MODE_SLAVE;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;
    hspi1.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1.Init.data_size = SPI_DATA_SIZE_16BIT;
    hspi1.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
    hspi1.Init.nss_mode = SPI_NSS_HARDWARE_INPUT;
    hspi1.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1.Init.crc_enabled = false;
    
    SPI_Init(&hspi1);
    SPI_Enable(&hspi1);
}

//============================================================================
// End of File
//============================================================================
