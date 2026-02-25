/**
 * @file Example_Slave_AllModes.c
 * @brief SPI Slave Mode - All Four Clock Modes Example
 * @description This example demonstrates all four SPI clock modes for slave:
 *              Mode 0: CPOL=0, CPHA=0
 *              Mode 1: CPOL=0, CPHA=1
 *              Mode 2: CPOL=1, CPHA=0
 *              Mode 3: CPOL=1, CPHA=1
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

#define BUFFER_SIZE    4

//============================================================================
// Global Variables
//============================================================================

SPI_HandleTypeDef hspi1;
uint8_t tx_buffer[BUFFER_SIZE] = {0x55, 0x66, 0x77, 0x88};
uint8_t rx_buffer[BUFFER_SIZE];

//============================================================================
// Function Prototypes
//============================================================================

void SPI1_Init_Slave_Mode0(void);
void SPI1_Init_Slave_Mode1(void);
void SPI1_Init_Slave_Mode2(void);
void SPI1_Init_Slave_Mode3(void);

//============================================================================
// Main Function
//============================================================================

int main(void)
{
    SystemClock_Config();
    SPI1_GPIO_Init();
    
    printf("SPI Slave - All Clock Modes Example\r\n");
    printf("===================================\r\n\r\n");
    
    printf("Slave configured. Waiting for master on each mode...\r\n\r\n");
    
    // Mode 0: CPOL=0, CPHA=0
    printf("Mode 0: CPOL=0, CPHA=0\r\n");
    SPI1_Init_Slave_Mode0();
    SPI_TransferBuffer(&hspi1, (uint16_t*)tx_buffer, (uint16_t*)rx_buffer, BUFFER_SIZE);
    printf("RX: ");
    for (int i = 0; i < BUFFER_SIZE; i++) printf("%02X ", rx_buffer[i]);
    printf("\r\n");
    SPI_Disable(&hspi1);
    
    // Mode 1: CPOL=0, CPHA=1
    printf("Mode 1: CPOL=0, CPHA=1\r\n");
    SPI1_Init_Slave_Mode1();
    SPI_TransferBuffer(&hspi1, (uint16_t*)tx_buffer, (uint16_t*)rx_buffer, BUFFER_SIZE);
    printf("RX: ");
    for (int i = 0; i < BUFFER_SIZE; i++) printf("%02X ", rx_buffer[i]);
    printf("\r\n");
    SPI_Disable(&hspi1);
    
    // Mode 2: CPOL=1, CPHA=0
    printf("Mode 2: CPOL=1, CPHA=0\r\n");
    SPI1_Init_Slave_Mode2();
    SPI_TransferBuffer(&hspi1, (uint16_t*)tx_buffer, (uint16_t*)rx_buffer, BUFFER_SIZE);
    printf("RX: ");
    for (int i = 0; i < BUFFER_SIZE; i++) printf("%02X ", rx_buffer[i]);
    printf("\r\n");
    SPI_Disable(&hspi1);
    
    // Mode 3: CPOL=1, CPHA=1
    printf("Mode 3: CPOL=1, CPHA=1\r\n");
    SPI1_Init_Slave_Mode3();
    SPI_TransferBuffer(&hspi1, (uint16_t*)tx_buffer, (uint16_t*)rx_buffer, BUFFER_SIZE);
    printf("RX: ");
    for (int i = 0; i < BUFFER_SIZE; i++) printf("%02X ", rx_buffer[i]);
    printf("\r\n");
    SPI_Disable(&hspi1);
    
    printf("\r\nAll modes tested!\r\n");
    
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
// SPI Mode Configurations
//============================================================================

void SPI1_Init_Slave_Mode0(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.mode = SPI_MODE_SLAVE;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;   // CPOL=0
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;    // CPHA=0
    hspi1.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1.Init.data_size = SPI_DATA_SIZE_8BIT;
    hspi1.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
    hspi1.Init.nss_mode = SPI_NSS_HARDWARE_INPUT;
    hspi1.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1.Init.crc_enabled = false;
    
    SPI_Init(&hspi1);
    SPI_Enable(&hspi1);
}

void SPI1_Init_Slave_Mode1(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.mode = SPI_MODE_SLAVE;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;   // CPOL=0
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_2ND_EDGE;    // CPHA=1
    hspi1.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1.Init.data_size = SPI_DATA_SIZE_8BIT;
    hspi1.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
    hspi1.Init.nss_mode = SPI_NSS_HARDWARE_INPUT;
    hspi1.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1.Init.crc_enabled = false;
    
    SPI_Init(&hspi1);
    SPI_Enable(&hspi1);
}

void SPI1_Init_Slave_Mode2(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.mode = SPI_MODE_SLAVE;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_HIGH;  // CPOL=1
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;    // CPHA=0
    hspi1.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1.Init.data_size = SPI_DATA_SIZE_8BIT;
    hspi1.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
    hspi1.Init.nss_mode = SPI_NSS_HARDWARE_INPUT;
    hspi1.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1.Init.crc_enabled = false;
    
    SPI_Init(&hspi1);
    SPI_Enable(&hspi1);
}

void SPI1_Init_Slave_Mode3(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.mode = SPI_MODE_SLAVE;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_HIGH;  // CPOL=1
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_2ND_EDGE;    // CPHA=1
    hspi1.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1.Init.data_size = SPI_DATA_SIZE_8BIT;
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
