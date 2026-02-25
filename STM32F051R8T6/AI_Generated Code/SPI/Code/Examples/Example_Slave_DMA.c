/**
 * @file Example_Slave_DMA.c
 * @brief SPI Slave Mode - DMA Transfer Example
 * @description This example demonstrates SPI slave using DMA for efficient
 *              data transfer without CPU intervention.
 * 
 * DMA Configuration (STM32F051):
 * - SPI1_TX -> DMA1 Channel 3
 * - SPI1_RX -> DMA1 Channel 2
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
#define RCC_AHBENR     (*(volatile uint32_t *)(RCC_BASE + 0x20))

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
// DMA Definitions
//============================================================================

#define DMA1_BASE       0x40020000UL

typedef struct {
    volatile uint32_t ISR;
    volatile uint32_t IFCR;
    volatile uint32_t CCR[7];
    volatile uint32_t CNDTR[7];
    volatile uint32_t CPAR[7];
    volatile uint32_t CMAR[7];
} DMA_Regs;

#define DMA1            ((DMA_Regs *)DMA1_BASE)

#define DMA_CH_TX       3
#define DMA_CH_RX       2

//============================================================================
// Configuration
//============================================================================

#define BUFFER_SIZE    32

//============================================================================
// Global Variables
//============================================================================

SPI_HandleTypeDef hspi1;
uint8_t tx_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];
volatile bool tx_done = false;
volatile bool rx_done = false;

//============================================================================
// Main Function
//============================================================================

int main(void)
{
    SystemClock_Config();
    SPI1_GPIO_Init();
    DMA1_Init();
    SPI1_Init_Slave_DMA();
    
    // Fill TX buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer[i] = 0x55 + i;
        rx_buffer[i] = 0;
    }
    
    printf("SPI Slave DMA Example\r\n");
    printf("====================\r\n\r\n");
    
    printf("Waiting for master to initiate DMA transfer...\r\n");
    
    // Wait for DMA completion
    while (!rx_done);
    
    // Print received data
    printf("\r\nReceived data:\r\n");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (i % 8 == 0) printf("\r\n");
        printf("%02X ", rx_buffer[i]);
    }
    printf("\r\n");
    
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
    
    // Enable DMA1 clock
    RCC_AHBENR |= (1 << 0);
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
    GPIOA->PUPDR &= ~((0x03 << (7 * 2)));
    GPIOA->PUPDR |= (0x01 << (7 * 2));
}

//============================================================================
// DMA Initialization
//============================================================================

void DMA1_Init(void)
{
    // Configure TX Channel (Channel 3)
    DMA1->CCR[DMA_CH_TX] = 0;
    DMA1->CNDTR[DMA_CH_TX] = BUFFER_SIZE;
    DMA1->CPAR[DMA_CH_TX] = (uint32_t)&SPI1->DR;
    DMA1->CMAR[DMA_CH_TX] = (uint32_t)tx_buffer;
    
    // TX: Memory to peripheral, increment memory, 8-bit
    DMA1->CCR[DMA_CH_TX] = (1 << 7) | (1 << 10) | (0 << 8) | (0 << 12);
    
    // Configure RX Channel (Channel 2)
    DMA1->CCR[DMA_CH_RX] = 0;
    DMA1->CNDTR[DMA_CH_RX] = BUFFER_SIZE;
    DMA1->CPAR[DMA_CH_RX] = (uint32_t)&SPI1->DR;
    DMA1->CMAR[DMA_CH_RX] = (uint32_t)rx_buffer;
    
    // RX: Peripheral to memory, increment memory, TCIE, 8-bit
    DMA1->CCR[DMA_CH_RX] = (1 << 8) | (1 << 10) | (1 << 12) | (1 << 1);
    
    // Enable DMA channels
    DMA1->CCR[DMA_CH_TX] |= 1;
    DMA1->CCR[DMA_CH_RX] |= 1;
}

//============================================================================
// SPI Initialization - Slave DMA
//============================================================================

void SPI1_Init_Slave_DMA(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.mode = SPI_MODE_SLAVE;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;
    hspi1.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1.Init.data_size = SPI_DATA_SIZE_8BIT;
    hspi1.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
    hspi1.Init.nss_mode = SPI_NSS_HARDWARE_INPUT;
    hspi1.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1.Init.crc_enabled = false;
    
    SPI_Init(&hspi1);
    
    // Enable DMA
    SPI_EnableDMA_TX(&hspi1);
    SPI_EnableDMA_RX(&hspi1);
    
    // Enable SPI
    SPI_Enable(&hspi1);
}

//============================================================================
// DMA IRQ Handler
//============================================================================

void DMA1_Channel2_3_IRQHandler(void)
{
    // Check RX complete
    if (DMA1->ISR & (1 << (DMA_CH_RX * 4 + 1))) {
        DMA1->IFCR = (1 << (DMA_CH_RX * 4 + 1));
        rx_done = true;
    }
    
    // Check TX complete
    if (DMA1->ISR & (1 << (DMA_CH_TX * 4 + 1))) {
        DMA1->IFCR = (1 << (DMA_CH_TX * 4 + 1));
        tx_done = true;
    }
}

//============================================================================
// End of File
//============================================================================
