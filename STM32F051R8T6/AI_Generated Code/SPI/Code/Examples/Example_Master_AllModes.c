/**
 * @file Example_Master_AllModes.c
 * @brief SPI Master Mode - All Four Clock Modes Example
 * @description This example demonstrates all four SPI clock modes:
 *              Mode 0: CPOL=0, CPHA=0 (Clock idle low, sample on first edge)
 *              Mode 1: CPOL=0, CPHA=1 (Clock idle low, sample on second edge)
 *              Mode 2: CPOL=1, CPHA=0 (Clock idle high, sample on first edge)
 *              Mode 3: CPOL=1, CPHA=1 (Clock idle high, sample on second edge)
 * 
 * Hardware Setup:
 * - SPI1: PA5 (SCK), PA6 (MISO), PA7 (MOSI), PA4 (NSS)
 * 
 * Important Fixes:
 * - MODF (Mode Fault) is prevented by proper initialization sequence:
 *   1. Configure GPIO pins first
 *   2. Set NSS pin HIGH using GPIO (for manual control)
 *   3. Initialize SPI with software NSS mode (sets SSI bit internally)
 *   4. Enable SPI peripheral
 * - MISO pin has pull-up configured for proper idle state
 */

#include "../spiF051.h"
#include "../../GPIO_AI/gpio.h"
#include "../../DeepSeek_Generated/RCC/rcc.h"
#include <stdio.h>

//============================================================================
// Register Definitions (for Clock Config only)
//============================================================================

#define RCC_BASE        0x40021000UL
#define RCC_CR         (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR       (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_APB2ENR    (*(volatile uint32_t *)(RCC_BASE + 0x24))

//============================================================================
// Global Variables
//============================================================================

SPI_HandleTypeDef hspi1;
uint8_t tx_data = 0x55;
uint8_t rx_data = 0;

//============================================================================
// Function Prototypes
//============================================================================

void SystemClock_Config(void);
void SPI1_GPIO_Init(void);
void SPI1_Init_Mode0(void);
void SPI1_Init_Mode1(void);
void SPI1_Init_Mode2(void);
void SPI1_Init_Mode3(void);

//============================================================================
// Main Function
//============================================================================

int main(void)
{
    SystemClock_Config();
    SPI1_GPIO_Init();
    
    printf("SPI Master - All Clock Modes Example\r\n");
    printf("====================================\r\n\r\n");
    
    // Test Mode 0
    printf("Testing Mode 0 (CPOL=0, CPHA=0)...\r\n");
    SPI1_Init_Mode0();
    // NSS LOW to start transmission
    GPIO_ResetPin(GPIOA, GPIO_PIN_4);
    rx_data = SPI_Transfer(&hspi1, tx_data);
    // NSS HIGH after transmission
    GPIO_SetPin(GPIOA, GPIO_PIN_4);
    printf("  Sent: %02X, Received: %02X\r\n", tx_data, rx_data);
    SPI_Disable(&hspi1);
    
    // Test Mode 1
    printf("Testing Mode 1 (CPOL=0, CPHA=1)...\r\n");
    SPI1_Init_Mode1();
    GPIO_ResetPin(GPIOA, GPIO_PIN_4);
    rx_data = SPI_Transfer(&hspi1, tx_data);
    GPIO_SetPin(GPIOA, GPIO_PIN_4);
    printf("  Sent: %02X, Received: %02X\r\n", tx_data, rx_data);
    SPI_Disable(&hspi1);
    
    // Test Mode 2
    printf("Testing Mode 2 (CPOL=1, CPHA=0)...\r\n");
    SPI1_Init_Mode2();
    GPIO_ResetPin(GPIOA, GPIO_PIN_4);
    rx_data = SPI_Transfer(&hspi1, tx_data);
    GPIO_SetPin(GPIOA, GPIO_PIN_4);
    printf("  Sent: %02X, Received: %02X\r\n", tx_data, rx_data);
    SPI_Disable(&hspi1);
    
    // Test Mode 3
    printf("Testing Mode 3 (CPOL=1, CPHA=1)...\r\n");
    SPI1_Init_Mode3();
    GPIO_ResetPin(GPIOA, GPIO_PIN_4);
    rx_data = SPI_Transfer(&hspi1, tx_data);
    GPIO_SetPin(GPIOA, GPIO_PIN_4);
    printf("  Sent: %02X, Received: %02X\r\n", tx_data, rx_data);
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
/* GPIO Initialization
  The function sets up the four required SPI pins:

PA4 (NSS) - Slave Select/Chip Select (output)
PA5 (SCK) - Serial Clock (alternate function)
PA6 (MISO) - Master In Slave Out (alternate function)
PA7 (MOSI) - Master Out Slave In (alternate function)

Pin	GPIO Register	Configuration
NSS (PA4)	MODER = 0x01 (Output)	Push-pull, High speed
SCK (PA5)	MODER = 0x02 (AF)	Push-pull, High speed, AF0
MISO (PA6)	MODER = 0x02 (AF)	Push-pull, High speed, AF0
MOSI (PA7)	MODER = 0x02 (AF)	Push-pull, High speed, AF0
*/
//============================================================================

void SPI1_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Enable GPIOA clock
    GPIO_EnableClock(GPIOA);
    
    // Configure NSS (PA4) as GPIO output - Manual slave select control
    // This pin will be controlled manually, not by SPI hardware
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;  // No pull-up/down for NSS
    GPIO_InitStruct.AF = GPIO_AF0;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Set NSS pin HIGH initially (inactive) - IMPORTANT for MODF prevention
    GPIO_SetPin(GPIOA, GPIO_PIN_4);
    
    // Configure SCK (PA5) - SPI1_SCK
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_InitStruct.AF = GPIO_AF0;  // SPI1 uses AF0 on PA5
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure MISO (PA6) - SPI1_MISO
    // IMPORTANT: MISO needs pull-up for proper idle state in master mode
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULL_UP;  // Pull-up for MISO (prevents floating)
    GPIO_InitStruct.AF = GPIO_AF0;  // SPI1 uses AF0 on PA6
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure MOSI (PA7) - SPI1_MOSI
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_InitStruct.AF = GPIO_AF0;  // SPI1 uses AF0 on PA7
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

//============================================================================
// SPI Mode Configurations
//============================================================================

void SPI1_Init_Mode0(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.mode = SPI_MODE_MASTER;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;   // CPOL = 0
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;   // CPHA = 0
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
    
    // CRITICAL FIX: Set NSS pin HIGH via GPIO BEFORE initializing SPI
    // This prevents MODF (Mode Fault) when using software slave management
    GPIO_SetPin(GPIOA, GPIO_PIN_4);  // NSS HIGH via GPIO
    
    SPI_Init(&hspi1);
    SPI_Enable(&hspi1);
}

void SPI1_Init_Mode1(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.mode = SPI_MODE_MASTER;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;   // CPOL = 0
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_2ND_EDGE;   // CPHA = 1
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
    
    // CRITICAL FIX: Set NSS pin HIGH via GPIO BEFORE initializing SPI
    GPIO_SetPin(GPIOA, GPIO_PIN_4);  // NSS HIGH via GPIO
    
    SPI_Init(&hspi1);
    SPI_Enable(&hspi1);
}

void SPI1_Init_Mode2(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.mode = SPI_MODE_MASTER;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_HIGH;  // CPOL = 1
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;   // CPHA = 0
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
    
    // CRITICAL FIX: Set NSS pin HIGH via GPIO BEFORE initializing SPI
    GPIO_SetPin(GPIOA, GPIO_PIN_4);  // NSS HIGH via GPIO
    
    SPI_Init(&hspi1);
    SPI_Enable(&hspi1);
}

void SPI1_Init_Mode3(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.mode = SPI_MODE_MASTER;
    hspi1.Init.clock_polarity = SPI_CLOCK_POLARITY_HIGH;  // CPOL = 1
    hspi1.Init.clock_phase = SPI_CLOCK_PHASE_2ND_EDGE;   // CPHA = 1
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
    
    // CRITICAL FIX: Set NSS pin HIGH via GPIO BEFORE initializing SPI
    GPIO_SetPin(GPIOA, GPIO_PIN_4);  // NSS HIGH via GPIO
    
    SPI_Init(&hspi1);
    SPI_Enable(&hspi1);
}

//============================================================================
// End of File
//============================================================================
