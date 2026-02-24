/**
 * @file Example_Master_DMA.c
 * @brief SPI Master Mode - DMA Transfer Example
 * @description This example demonstrates SPI communication in master mode
 *              using DMA for efficient data transfer without CPU intervention.
 * 
 * Hardware Setup:
 * - SPI1: PA5 (SCK), PA6 (MISO), PA7 (MOSI)
 * - Connect SCK to slave SCK
 * - Connect MISO to slave MISO  
 * - Connect MOSI to slave MOSI
 * - Connect NSS (PA4) to slave NSS
 * 
 * DMA Configuration:
 * - SPI1_TX -> DMA1 Channel 3
 * - SPI1_RX -> DMA1 Channel 2
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
#define BUFFER_SIZE    32

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
// DMA Register Definitions (STM32F051)
//============================================================================

#define DMA1_BASE       0x40020000UL

typedef struct {
    volatile uint32_t CCR;      // Channel configuration register
    volatile uint32_t CNDTR;    // Number of data register
    volatile uint32_t CPAR;     // Peripheral address register
    volatile uint32_t CMAR;     // Memory address register
} DMA_Channel_TypeDef;

typedef struct {
    volatile uint32_t ISR;      // Interrupt status register
    volatile uint32_t IFCR;     // Interrupt flag clear register
    DMA_Channel_TypeDef Channel[7];  // DMA1 has 7 channels
} DMA_TypeDef;

#define DMA1            ((DMA_TypeDef *)DMA1_BASE)

// DMA Channel definitions
#define DMA_CHANNEL_SPI1_TX   3  // Channel 3 for SPI1 TX
#define DMA_CHANNEL_SPI1_RX   2  // Channel 2 for SPI1 RX

// DMA flags
#define DMA_FLAG_TC          (1U << 1)  // Transfer complete
#define DMA_FLAG_TC_TX        (1U << (DMA_CHANNEL_SPI1_TX * 4 + 1))
#define DMA_FLAG_TC_RX        (1U << (DMA_CHANNEL_SPI1_RX * 4 + 1))

//============================================================================
// Global Variables
//============================================================================

SPI_HandleTypeDef hspi1;
uint8_t tx_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];
volatile bool tx_complete = false;
volatile bool rx_complete = false;

//============================================================================
// Function Prototypes
//============================================================================

void SystemClock_Config(void);
void SPI1_GPIO_Init(void);
void DMA1_Init(void);
void SPI1_Init_Master_DMA(void);
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
    
    // Initialize DMA
    DMA1_Init();
    
    // Initialize SPI1 as master with DMA
    SPI1_Init_Master_DMA();
    
    // Fill transmit buffer with test data
    FillTxBuffer();
    
    printf("SPI Master DMA Transfer Example\r\n");
    printf("===============================\r\n\r\n");
    
    // Print TX buffer
    printf("TX Buffer: ");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (i % 8 == 0) printf("\r\n");
        printf("%02X ", tx_buffer[i]);
    }
    printf("\r\n\r\n");
    
    // Clear receive buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        rx_buffer[i] = 0x00;
    }
    
    // Start DMA transfer
    tx_complete = false;
    rx_complete = false;
    
    // Enable DMA for RX first, then TX (as per STM32 guidelines)
    SPI_EnableDMA_RX(&hspi1);
    SPI_EnableDMA_TX(&hspi1);
    
    // Enable SPI
    SPI_Enable(&hspi1);
    
    // Wait for DMA transfer to complete
    printf("Transfer in progress...\r\n");
    
    // Wait for TX complete
    while (!tx_complete);
    
    // Wait for SPI to finish (BSY = 0)
    SPI_WaitNotBusy(&hspi1);
    
    // Small delay for RX to complete
    for (volatile int i = 0; i < 10000; i++);
    
    // Wait for RX complete
    while (!rx_complete);
    
    // Print RX buffer
    printf("\r\nRX Buffer: ");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (i % 8 == 0) printf("\r\n");
        printf("%02X ", rx_buffer[i]);
    }
    printf("\r\n\r\n");
    
    printf("DMA transfer complete!\r\n");
    
    // Disable DMA
    SPI_DisableDMA_TX(&hspi1);
    SPI_DisableDMA_RX(&hspi1);
    
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
    
    // Enable DMA1 clock
    RCC_AHBENR |= (1 << 0);  // DMA1EN
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
// DMA Initialization
//============================================================================

void DMA1_Init(void)
{
    // Configure TX DMA Channel (Channel 3)
    // Memory -> Peripheral
    DMA1->Channel[DMA_CHANNEL_SPI1_TX].CCR = 0;
    DMA1->Channel[DMA_CHANNEL_SPI1_TX].CNDTR = BUFFER_SIZE;
    DMA1->Channel[DMA_CHANNEL_SPI1_TX].CPAR = (uint32_t)&(SPI1->DR);  // SPI1 Data Register
    DMA1->Channel[DMA_CHANNEL_SPI1_TX].CMAR = (uint32_t)tx_buffer;
    
    // Configure: MEM2PER, MINC, PSIZE=8bit, MSIZE=8bit
    DMA1->Channel[DMA_CHANNEL_SPI1_TX].CCR = (1 << 7) |   // MEM2PER (Memory to peripheral)
                                             (1 << 10) |  // MINC (Memory increment)
                                             (0 << 8) |   // PSIZE = 8-bit
                                             (0 << 12);   // MSIZE = 8-bit
    
    // Configure RX DMA Channel (Channel 2)
    // Peripheral -> Memory
    DMA1->Channel[DMA_CHANNEL_SPI1_RX].CCR = 0;
    DMA1->Channel[DMA_CHANNEL_SPI1_RX].CNDTR = BUFFER_SIZE;
    DMA1->Channel[DMA_CHANNEL_SPI1_RX].CPAR = (uint32_t)&(SPI1->DR);
    DMA1->Channel[DMA_CHANNEL_SPI1_RX].CMAR = (uint32_t)rx_buffer;
    
    // Configure: PER2MEM, MINC, PSIZE=8bit, MSIZE=8bit, TCIE
    DMA1->Channel[DMA_CHANNEL_SPI1_RX].CCR = (1 << 8) |   // TCIE (Transfer complete interrupt enable)
                                             (1 << 7) |   // PER2MEM
                                             (1 << 10) |  // MINC
                                             (0 << 8) |   // PSIZE = 8-bit
                                             (0 << 12);   // MSIZE = 8-bit
    
    // Enable DMA channels
    DMA1->Channel[DMA_CHANNEL_SPI1_TX].CCR |= (1 << 0);  // EN
    DMA1->Channel[DMA_CHANNEL_SPI1_RX].CCR |= (1 << 0);  // EN
}

//============================================================================
// SPI Initialization - Master with DMA
//============================================================================

void SPI1_Init_Master_DMA(void)
{
    // Initialize SPI handle
    hspi1.Instance = SPI_HW;
    
    // Configure SPI for DMA mode
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
    hspi1.Init.crc_enabled = false;
    hspi1.Init.crc_polynomial = 7;
    hspi1.Init.nss_pulse_enabled = false;
    
    // Initialize SPI
    SPI_Init(&hspi1);
    
    // Enable DMA requests
    SPI_EnableDMA_TX(&hspi1);
    SPI_EnableDMA_RX(&hspi1);
    
    // Set NSS high
    SPI_SetNSSPin(&hspi1, true);
}

//============================================================================
// Helper Functions
//============================================================================

void FillTxBuffer(void)
{
    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer[i] = i;  // 0x00, 0x01, 0x02, ...
    }
}

//============================================================================
// DMA and SPI Interrupt Handlers
//============================================================================

/**
 * @brief DMA1 Channel 2-3 IRQ Handler (SPI1_RX and SPI1_TX)
 */
void DMA1_Channel2_3_IRQHandler(void)
{
    // Check TX channel transfer complete
    if (DMA1->ISR & DMA_FLAG_TC_TX) {
        DMA1->IFCR = DMA_FLAG_TC_TX;  // Clear flag
        tx_complete = true;
    }
    
    // Check RX channel transfer complete
    if (DMA1->ISR & DMA_FLAG_TC_RX) {
        DMA1->IFCR = DMA_FLAG_TC_RX;  // Clear flag
        rx_complete = true;
    }
}

//============================================================================
// DMA Usage Notes:
//============================================================================
/*
 * STM32F051 DMA - SPI Configuration:
 * 
 * 1. Enable DMA clock (RCC_AHBENR |= DMA1EN)
 * 2. Configure DMA channels:
 *    - TX: Channel 3 for SPI1_TX (or appropriate channel)
 *    - RX: Channel 2 for SPI1_RX (or appropriate channel)
 * 3. Configure DMA channel registers:
 *    - CNDTR: Number of data to transfer
 *    - CPAR: Peripheral address (SPIx_DR)
 *    - CMAR: Memory address (buffer)
 *    - CCR: Configuration (DIR, MINC, PSIZE, MSIZE, etc.)
 * 4. Enable DMA channels (CCR.EN = 1)
 * 5. Enable DMA requests in SPI_CR2 (TXDMAEN, RXDMAEN)
 * 6. Enable SPI
 * 
 * Important: Enable RX DMA before TX DMA (as per STM32 guidelines)
 * 
 * To close DMA communication:
 * 1. Disable DMA streams in DMA registers
 * 2. Disable SPI using SPI_DisablePeripheral()
 * 3. Disable DMA Tx/Rx buffers in SPI_CR2
 */

//============================================================================
// End of File
//============================================================================
