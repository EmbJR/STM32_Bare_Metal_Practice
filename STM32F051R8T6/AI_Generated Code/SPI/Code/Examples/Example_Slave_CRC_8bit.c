/**
 * @file Example_Slave_CRC_8bit.c
 * @brief SPI Slave Mode - Full Duplex with CRC Example
 * @description This example demonstrates SPI communication in slave mode
 *              with CRC enabled for data integrity checking.
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

#define BUFFER_SIZE    8

#define CRC_POLYNOMIAL  0x07  // CRC-8 polynomial (must match master)

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

//============================================================================
// Function Prototypes
//============================================================================

void SystemClock_Config(void);
void SPI1_GPIO_Init(void);
void SPI1_Init_Slave_CRC_8bit(void);
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
    
    // Initialize SPI1 as slave with CRC enabled
    SPI1_Init_Slave_CRC_8bit();
    
    // Fill transmit buffer
    FillTxBuffer();
    
    printf("SPI Slave with CRC Example\r\n");
    printf("==========================\r\n\r\n");
    
    printf("Waiting for master to initiate transfer...\r\n");
    
    // Reset CRC before communication
    SPI_ResetCRC(&hspi1);
    
    // Receive data from master
    // In CRC mode, master sends data + CRC, we need to respond with our data + CRC
    
    // Receive all data
    for (int i = 0; i < BUFFER_SIZE; i++) {
        // Wait for RX buffer not empty
        while (!SPI_IsRXNE(&hspi1));
        
        // Read received data
        rx_buffer[i] = SPI_ReceiveData8(&hspi1);
        
        // Wait for TX buffer empty
        while (!SPI_IsTXE(&hspi1));
        
        // Send response data
        SPI_SendData8(&hspi1, tx_buffer[i]);
    }
    
    // Wait for transmission complete
    SPI_WaitNotBusy(&hspi1);
    
    // Small delay
    for (volatile int i = 0; i < 1000; i++);
    
    // Check for CRC error
    if (SPI_CheckCRCError(&hspi1)) {
        printf("CRC ERROR: Data corruption detected!\r\n");
        SPI_ClearCRCError(&hspi1);
    } else {
        printf("CRC OK: No data corruption detected.\r\n");
    }
    
    // Print received data
    printf("RX Buffer (from master): ");
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
    
    // PA4 - NSS (Hardware input)
    GPIOA->MODER &= ~((0x03 << (4 * 2)));
    GPIOA->PUPDR &= ~((0x03 << (4 * 2)));
    GPIOA->PUPDR |= (0x01 << (4 * 2));  // Pull-up
    
    // PA5 - SCK (Input from master)
    GPIOA->MODER &= ~((0x03 << (5 * 2)));
    GPIOA->PUPDR &= ~((0x03 << (5 * 2)));
    GPIOA->PUPDR |= (0x02 << (5 * 2));  // Pull-down
    
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
    GPIOA->PUPDR |= (0x01 << (7 * 2));  // Pull-up
}

//============================================================================
// SPI Initialization - Slave with CRC
//============================================================================

void SPI1_Init_Slave_CRC_8bit(void)
{
    // Initialize SPI handle
    hspi1.Instance = SPI_HW;
    
    // Configure SPI for slave with CRC
    hspi1.Init.mode = SPI_MODE_SLAVE;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;   // Must match master
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;  // Must match master
    hspi1.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1.Init.data_size = SPI_DATA_SIZE_8BIT;
    hspi1.Init.baud_rate = SPI_BAUD_RATE_DIV_2;
    hspi1.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
    hspi1.Init.nss_mode = SPI_NSS_HARDWARE_INPUT;
    hspi1.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1.Init.rx_fifo_threshold = SPI_FIFO_THRESHOLD_1_2;
    hspi1.Init.crc_enabled = true;           // Enable CRC
    hspi1.Init.crc_polynomial = CRC_POLYNOMIAL;  // CRC-8 polynomial
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
    // Response data pattern
    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer[i] = 0xAA;  // 10101010 pattern
    }
}

//============================================================================
// End of File
//============================================================================
