/**
 * @file Example_Slave_Simplex_Rx.c
 * @brief SPI Slave Mode - Simplex Receive Only Example
 * @description This example demonstrates SPI slave in receive-only mode.
 *              The slave only receives data from master (MOSI line).
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

#define BUFFER_SIZE    16

//============================================================================
// Global Variables
//============================================================================

SPI_HandleTypeDef hspi1;
uint8_t rx_buffer[BUFFER_SIZE];

//============================================================================
// Main Function
//============================================================================

int main(void)
{
    SystemClock_Config();
    SPI1_GPIO_Init();
    SPI1_Init_Slave_RxOnly();
    
    printf("SPI Slave Simplex Receive Example\r\n");
    printf("=================================\r\n\r\n");
    
    // Clear buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        rx_buffer[i] = 0;
    }
    
    printf("Waiting for data from master...\r\n");
    
    // Receive data
    SPI_ReceiveBuffer(&hspi1, (uint16_t*)rx_buffer, BUFFER_SIZE);
    
    // Print received data
    printf("Received data:\r\n");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("%02X ", rx_buffer[i]);
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
    
    // PA4 - NSS
    GPIOA->MODER &= ~((0x03 << (4 * 2)));
    GPIOA->PUPDR &= ~((0x03 << (4 * 2)));
    GPIOA->PUPDR |= (0x01 << (4 * 2));
    
    // PA5 - SCK
    GPIOA->MODER &= ~((0x03 << (5 * 2)));
    GPIOA->PUPDR &= ~((0x03 << (5 * 2)));
    GPIOA->PUPDR |= (0x02 << (5 * 2));
    
    // Note: MISO not used in receive-only mode - can be GPIO
    
    // PA7 - MOSI (Input)
    GPIOA->MODER &= ~((0x03 << (7 * 2)));
    GPIOA->PUPDR &= ~((0x03 << (7 * 2)));
    GPIOA->PUPDR |= (0x01 << (7 * 2));
}

//============================================================================
// SPI Initialization - Slave Receive Only
//============================================================================

void SPI1_Init_Slave_RxOnly(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.mode = SPI_MODE_SLAVE;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;
    hspi1.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1.Init.data_size = SPI_DATA_SIZE_8BIT;
    hspi1.Init.comm_mode = SPI_COMM_MODE_RECEIVE_ONLY;  // RX only
    hspi1.Init.nss_mode = SPI_NSS_HARDWARE_INPUT;
    hspi1.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1.Init.crc_enabled = false;
    
    SPI_Init(&hspi1);
    SPI_Enable(&hspi1);
}

//============================================================================
// End of File
//============================================================================
