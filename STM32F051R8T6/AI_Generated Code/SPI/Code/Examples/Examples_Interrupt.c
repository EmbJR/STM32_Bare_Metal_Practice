/**
 * @file Examples_Interrupt.c
 * @brief SPI Interrupt-Based Examples for STM32F051
 * @description This file contains interrupt-based (non-blocking) versions
 *              of all SPI communication examples.
 * 
 * Hardware Setup (SPI1):
 * - PA5 = SCK
 * - PA6 = MISO  
 * - PA7 = MOSI
 * - PA4 = NSS (Software control)
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

#define NVIC_BASE       0xE000E100UL
#define NVIC_ISER      (*(volatile uint32_t *)(NVIC_BASE + 0x00))
#define NVIC_ICER      (*(volatile uint32_t *)(NVIC_BASE + 0x80))
#define NVIC_IPR       ((volatile uint32_t *)(NVIC_BASE + 0x300))

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
// NVIC Functions
//============================================================================

/**
 * @brief Enable NVIC interrupt
 */
static void NVIC_EnableIRQ(uint8_t irq)
{
    if (irq < 32) {
        NVIC_ISER = (1U << irq);
    } else if (irq < 64) {
        NVIC_ISER = (1U << (irq - 32));
    }
}

/**
 * @brief Disable NVIC interrupt
 */
static void NVIC_DisableIRQ(uint8_t irq)
{
    if (irq < 32) {
        NVIC_ICER = (1U << irq);
    } else if (irq < 64) {
        NVIC_ICER = (1U << (irq - 32));
    }
}

/**
 * @brief Set NVIC priority
 */
static void NVIC_SetPriority(uint8_t irq, uint8_t priority)
{
    NVIC_IPR[irq >> 2] = (priority << ((irq % 4) * 8)) & 0xFF;
}

//============================================================================
// Example 1: Master Full-Duplex 8-bit with Interrupt
//============================================================================

#define BUFFER_SIZE    16

SPI_HandleTypeDef hspi1_int;
uint8_t tx_buffer_int[BUFFER_SIZE];
uint8_t rx_buffer_int[BUFFER_SIZE];
volatile bool transfer_complete_int = false;
volatile bool transfer_error_int = false;
volatile uint8_t tx_index_int = 0;
volatile uint8_t rx_index_int = 0;

/**
 * @brief Example 1: Master Full-Duplex 8-bit Interrupt
 */
void Example1_Master_FullDuplex_8bit_Interrupt(void)
{
    printf("Example 1: Master Full-Duplex 8-bit Interrupt\r\n");
    printf("--------------------------------------------\r\n\r\n");
    
    // Initialize SPI for interrupt mode
    hspi1_int.Instance = SPI1;
    hspi1_int.Init.mode = SPI_MODE_MASTER;
    hspi1_int.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    hspi1_int.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;
    hspi1_int.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1_int.Init.data_size = SPI_DATA_SIZE_8BIT;
    hspi1_int.Init.baud_rate = SPI_BAUD_RATE_DIV_8;
    hspi1_int.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
    hspi1_int.Init.nss_mode = SPI_NSS_SOFTWARE;
    hspi1_int.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1_int.Init.rx_fifo_threshold = SPI_FIFO_THRESHOLD_1_2;
    hspi1_int.Init.crc_enabled = false;
    
    SPI_Init(&hspi1_int);
    
    // Enable NVIC interrupt for SPI1 (IRQ 25)
    NVIC_EnableIRQ(SPI1_IRQn);
    NVIC_SetPriority(SPI1_IRQn, 0);
    
    // Enable interrupts
    SPI_EnableInterrupt(&hspi1_int, SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR);
    
    // Enable SPI
    SPI_Enable(&hspi1_int);
    
    // Fill buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer_int[i] = i;
        rx_buffer_int[i] = 0;
    }
    tx_index_int = 0;
    rx_index_int = 0;
    transfer_complete_int = false;
    transfer_error_int = false;
    
    // Set NSS low
    SPI_SetNSSPin(&hspi1_int, false);
    
    // Trigger first transmission
    SPI_SendData8(&hspi1_int, tx_buffer_int[0]);
    tx_index_int = 1;
    
    // Wait for completion (in real app, this would be in main loop)
    // while (!transfer_complete_int);
    
    printf("Transfer initiated (interrupt-driven)\r\n");
}

/**
 * @brief SPI1 IRQ Handler for interrupt-based transfers
 */
void SPI1_IRQHandler_Example1(void)
{
    uint32_t sr = hspi1_int.Instance->SR;
    
    // TXE interrupt - send next data
    if (sr & SPI_SR_TXE) {
        if (tx_index_int < BUFFER_SIZE) {
            SPI_SendData8(&hspi1_int, tx_buffer_int[tx_index_int]);
            tx_index_int++;
        }
    }
    
    // RXNE interrupt - receive data
    if (sr & SPI_SR_RXNE) {
        if (rx_index_int < BUFFER_SIZE) {
            rx_buffer_int[rx_index_int] = SPI_ReceiveData8(&hspi1_int);
            rx_index_int++;
        }
    }
    
    // Error handling
    if (sr & (SPI_SR_OVR | SPI_SR_MODF | SPI_SR_CRCERR)) {
        transfer_error_int = true;
    }
    
    // Check completion - wait for BSY to go low before de-asserting NSS
    // TXE only indicates buffer is empty, NOT that transmission is complete
    if (rx_index_int >= BUFFER_SIZE && tx_index_int >= BUFFER_SIZE) {
        // Wait for SPI to finish transmitting all data (BSY flag cleared)
        while (hspi1_int.Instance->SR & SPI_SR_BSY);
        
        transfer_complete_int = true;
        SPI_SetNSSPin(&hspi1_int, true);
        SPI_DisableInterrupt(&hspi1_int, SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR);
    }
}

//============================================================================
// Example 2: Master Full-Duplex 16-bit with Interrupt
//============================================================================

#define BUFFER_SIZE_16    8

SPI_HandleTypeDef hspi1_16;
uint16_t tx_buffer_16[BUFFER_SIZE_16];
uint16_t rx_buffer_16[BUFFER_SIZE_16];
volatile bool transfer_complete_16 = false;
volatile uint8_t tx_index_16 = 0;
volatile uint8_t rx_index_16 = 0;

/**
 * @brief Example 2: Master Full-Duplex 16-bit Interrupt
 */
void Example2_Master_FullDuplex_16bit_Interrupt(void)
{
    printf("Example 2: Master Full-Duplex 16-bit Interrupt\r\n");
    printf("----------------------------------------------\r\n\r\n");
    
    hspi1_16.Instance = SPI1;
    hspi1_16.Init.mode = SPI_MODE_MASTER;
    hspi1_16.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    hspi1_16.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;
    hspi1_16.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1_16.Init.data_size = SPI_DATA_SIZE_16BIT;
    hspi1_16.Init.baud_rate = SPI_BAUD_RATE_DIV_8;
    hspi1_16.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
    hspi1_16.Init.nss_mode = SPI_NSS_SOFTWARE;
    hspi1_16.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1_16.Init.rx_fifo_threshold = SPI_FIFO_THRESHOLD_1_2;
    hspi1_16.Init.crc_enabled = false;
    
    SPI_Init(&hspi1_16);
    // Enable NVIC for SPI1
    NVIC_EnableIRQ(SPI1_IRQn);
    NVIC_SetPriority(SPI1_IRQn, 0);
    
    SPI_EnableInterrupt(&hspi1_16, SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR);
    SPI_Enable(&hspi1_16);
    
    // Fill 16-bit buffer
    for (int i = 0; i < BUFFER_SIZE_16; i++) {
        tx_buffer_16[i] = 0x0100 | i;
    }
    tx_index_16 = 0;
    rx_index_16 = 0;
    transfer_complete_16 = false;
    
    SPI_SetNSSPin(&hspi1_16, false);
    SPI_SendData(&hspi1_16, tx_buffer_16[0]);
    tx_index_16 = 1;
    
    printf("16-bit transfer initiated (interrupt-driven)\r\n");
}

void SPI1_IRQHandler_Example2(void)
{
    uint32_t sr = hspi1_16.Instance->SR;
    
    if (sr & SPI_SR_TXE) {
        if (tx_index_16 < BUFFER_SIZE_16) {
            SPI_SendData(&hspi1_16, tx_buffer_16[tx_index_16]);
            tx_index_16++;
        }
    }
    
    if (sr & SPI_SR_RXNE) {
        if (rx_index_16 < BUFFER_SIZE_16) {
            rx_buffer_16[rx_index_16] = SPI_ReceiveData(&hspi1_16);
            rx_index_16++;
        }
    }
    
    // Check completion - wait for BSY to go low before de-asserting NSS
    // TXE only indicates buffer is empty, NOT that transmission is complete
    if (rx_index_16 >= BUFFER_SIZE_16 && tx_index_16 >= BUFFER_SIZE_16) {
        // Wait for SPI to finish transmitting all data (BSY flag cleared)
        while (hspi1_16.Instance->SR & SPI_SR_BSY);
        
        transfer_complete_16 = true;
        SPI_SetNSSPin(&hspi1_16, true);
    }
}

//============================================================================
// Example 3: Master Simplex Transmit-Only Interrupt
//============================================================================

#define TX_BUFFER_SIZE    16

SPI_HandleTypeDef hspi1_simplex;
uint8_t tx_simplex[TX_BUFFER_SIZE];
volatile bool tx_complete_simplex = false;
volatile uint8_t tx_index_simplex = 0;

/**
 * @brief Example 3: Master Simplex TX Interrupt
 */
void Example3_Master_Simplex_Tx_Interrupt(void)
{
    printf("Example 3: Master Simplex TX Interrupt\r\n");
    printf("--------------------------------------\r\n\r\n");
    
    hspi1_simplex.Instance = SPI1;
    hspi1_simplex.Init.mode = SPI_MODE_MASTER;
    hspi1_simplex.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    hspi1_simplex.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;
    hspi1_simplex.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1_simplex.Init.data_size = SPI_DATA_SIZE_8BIT;
    hspi1_simplex.Init.baud_rate = SPI_BAUD_RATE_DIV_8;
    hspi1_simplex.Init.comm_mode = SPI_COMM_MODE_TRANSMIT_ONLY;
    hspi1_simplex.Init.nss_mode = SPI_NSS_SOFTWARE;
    hspi1_simplex.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1_simplex.Init.crc_enabled = false;
    
    SPI_Init(&hspi1_simplex);
    // Enable NVIC for SPI1
    NVIC_EnableIRQ(SPI1_IRQn);
    NVIC_SetPriority(SPI1_IRQn, 0);
    
    SPI_EnableInterrupt(&hspi1_simplex, SPI_IT_TXE);
    SPI_Enable(&hspi1_simplex);
    
    for (int i = 0; i < TX_BUFFER_SIZE; i++) {
        tx_simplex[i] = 0xAA;
    }
    tx_index_simplex = 0;
    tx_complete_simplex = false;
    
    SPI_SetNSSPin(&hspi1_simplex, false);
    SPI_SendData8(&hspi1_simplex, tx_simplex[0]);
    tx_index_simplex = 1;
    
    printf("Simplex TX initiated (interrupt-driven)\r\n");
}

void SPI1_IRQHandler_Example3(void)
{
    uint32_t sr = hspi1_simplex.Instance->SR;
    
    if (sr & SPI_SR_TXE) {
        if (tx_index_simplex < TX_BUFFER_SIZE) {
            SPI_SendData8(&hspi1_simplex, tx_simplex[tx_index_simplex]);
            tx_index_simplex++;
        } else {
            // Wait for BSY to go low
            while (hspi1_simplex.Instance->SR & SPI_SR_BSY);
            tx_complete_simplex = true;
            SPI_SetNSSPin(&hspi1_simplex, true);
            SPI_DisableInterrupt(&hspi1_simplex, SPI_IT_TXE);
        }
    }
}

//============================================================================
// Example 4: Slave Full-Duplex 8-bit Interrupt
//============================================================================

SPI_HandleTypeDef hspi1_slave;
uint8_t tx_slave[BUFFER_SIZE];
uint8_t rx_slave[BUFFER_SIZE];
volatile bool slave_rx_ready = false;
volatile uint8_t slave_tx_index = 0;
volatile uint8_t slave_rx_index = 0;

/**
 * @brief Example 4: Slave Full-Duplex Interrupt
 */
void Example4_Slave_FullDuplex_Interrupt(void)
{
    printf("Example 4: Slave Full-Duplex Interrupt\r\n");
    printf("-------------------------------------\r\n\r\n");
    
    hspi1_slave.Instance = SPI1;
    hspi1_slave.Init.mode = SPI_MODE_SLAVE;
    hspi1_slave.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    hspi1_slave.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;
    hspi1_slave.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1_slave.Init.data_size = SPI_DATA_SIZE_8BIT;
    hspi1_slave.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
    hspi1_slave.Init.nss_mode = SPI_NSS_HARDWARE_INPUT;
    hspi1_slave.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    hspi1_slave.Init.crc_enabled = false;
    
    SPI_Init(&hspi1_slave);
    // Enable NVIC for SPI1
    NVIC_EnableIRQ(SPI1_IRQn);
    NVIC_SetPriority(SPI1_IRQn, 0);
    
    SPI_EnableInterrupt(&hspi1_slave, SPI_IT_RXNE | SPI_IT_ERR);
    SPI_Enable(&hspi1_slave);
    
    // Fill response buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_slave[i] = 0x55 + i;
        rx_slave[i] = 0;
    }
    slave_tx_index = 0;
    slave_rx_index = 0;
    slave_rx_ready = false;
    
    printf("Slave ready (interrupt-driven)\r\n");
}

void SPI1_IRQHandler_Example4(void)
{
    uint32_t sr = hspi1_slave.Instance->SR;
    
    // RXNE - receive data and send response
    if (sr & SPI_SR_RXNE) {
        if (slave_rx_index < BUFFER_SIZE) {
            rx_slave[slave_rx_index] = SPI_ReceiveData8(&hspi1_slave);
            slave_rx_index++;
            
            // Send response
            if (slave_tx_index < BUFFER_SIZE) {
                SPI_SendData8(&hspi1_slave, tx_slave[slave_tx_index]);
                slave_tx_index++;
            }
        }
    }
    
    if (slave_rx_index >= BUFFER_SIZE) {
        slave_rx_ready = true;
    }
}

//============================================================================
// Example 5: Master with CRC Interrupt
//============================================================================

#define CRC_BUFFER_SIZE   8

SPI_HandleTypeDef hspi1_crc;
uint8_t tx_crc[CRC_BUFFER_SIZE];
uint8_t rx_crc[CRC_BUFFER_SIZE];
volatile bool crc_transfer_done = false;
volatile uint8_t crc_tx_index = 0;
volatile uint8_t crc_rx_index = 0;

/**
 * @brief Example 5: Master CRC Interrupt
 */
void Example5_Master_CRC_Interrupt(void)
{
    printf("Example 5: Master CRC Interrupt\r\n");
    printf("------------------------------\r\n\r\n");
    
    hspi1_crc.Instance = SPI1;
    hspi1_crc.Init.mode = SPI_MODE_MASTER;
    hspi1_crc.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    hspi1_crc.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;
    hspi1_crc.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    hspi1_crc.Init.data_size = SPI_DATA_SIZE_8BIT;
    hspi1_crc.Init.baud_rate = SPI_BAUD_RATE_DIV_8;
    hspi1_crc.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
    hspi1_crc.Init.nss_mode = SPI_NSS_SOFTWARE;
    hspi1_crc.Init.crc_enabled = true;
    hspi1_crc.Init.crc_polynomial = 0x07;
    
    SPI_Init(&hspi1_crc);
    // Enable NVIC for SPI1
    NVIC_EnableIRQ(SPI1_IRQn);
    NVIC_SetPriority(SPI1_IRQn, 0);
    
    SPI_EnableInterrupt(&hspi1_crc, SPI_IT_TXE | SPI_IT_RXNE);
    SPI_Enable(&hspi1_crc);
    
    // Fill buffer
    for (int i = 0; i < CRC_BUFFER_SIZE; i++) {
        tx_crc[i] = i + 1;
    }
    crc_tx_index = 0;
    crc_rx_index = 0;
    crc_transfer_done = false;
    
    SPI_ResetCRC(&hspi1_crc);
    SPI_SetNSSPin(&hspi1_crc, false);
    SPI_SendData8(&hspi1_crc, tx_crc[0]);
    crc_tx_index = 1;
    
    printf("CRC transfer initiated (interrupt-driven)\r\n");
}

void SPI1_IRQHandler_Example5(void)
{
    uint32_t sr = hspi1_crc.Instance->SR;
    
    if (sr & SPI_SR_TXE) {
        if (crc_tx_index < CRC_BUFFER_SIZE) {
            SPI_SendData8(&hspi1_crc, tx_crc[crc_tx_index]);
            crc_tx_index++;
        } else if (crc_tx_index == CRC_BUFFER_SIZE) {
            // All data sent, trigger CRC transmission
            SPI_TransmitCRC(&hspi1_crc);
            crc_tx_index++;
        }
    }
    
    if (sr & SPI_SR_RXNE) {
        if (crc_rx_index < CRC_BUFFER_SIZE) {
            rx_crc[crc_rx_index] = SPI_ReceiveData8(&hspi1_crc);
            crc_rx_index++;
        }
    }
    
    // Check CRC error
    if (sr & SPI_SR_CRCERR) {
        printf("CRC Error detected!\r\n");
    }
    
    // Check completion - wait for BSY to go low before de-asserting NSS
    if (crc_rx_index >= CRC_BUFFER_SIZE && crc_tx_index > CRC_BUFFER_SIZE) {
        // Wait for SPI to finish transmitting all data including CRC (BSY flag cleared)
        while (hspi1_crc.Instance->SR & SPI_SR_BSY);
        
        crc_transfer_done = true;
        SPI_SetNSSPin(&hspi1_crc, true);
        
        // Print CRC values
        printf("TX CRC: %02X\r\n", SPI_GetTXCRC(&hspi1_crc));
        printf("RX CRC: %02X\r\n", SPI_GetRXCRC(&hspi1_crc));
    }
}

//============================================================================
// Example 6: Master All Modes Interrupt
//============================================================================

SPI_HandleTypeDef hspi1_modes;
uint8_t tx_mode = 0x55;
volatile bool mode_transfer_done = false;

/**
 * @brief Example 6: All SPI Modes Interrupt (demonstrates Mode 0-3)
 */
void Example6_Master_AllModes_Interrupt(void)
{
    printf("Example 6: Master All Modes Interrupt\r\n");
    printf("--------------------------------------\r\n\r\n");
    
    // Test each mode
    for (int mode = 0; mode < 4; mode++) {
        SPI_ClockPolarity polarity = (mode & 0x02) ? SPI_CLOCK_POLARITY_HIGH : SPI_CLOCK_POLARITY_LOW;
        SPI_ClockPhase phase = (mode & 0x01) ? SPI_CLOCK_PHASE_2ND_EDGE : SPI_CLOCK_PHASE_1ST_EDGE;
        
        printf("Testing Mode %d (CPOL=%d, CPHA=%d)\r\n", mode, 
               (mode & 0x02) >> 1, mode & 0x01);
        
        // Reconfigure for each mode
        SPI_Disable(&hspi1_modes);
        hspi1_modes.Instance = SPI1;
        hspi1_modes.Init.mode = SPI_MODE_MASTER;
        hspi1_modes.Init.clock_polarity = polarity;
        hspi1_modes.Init.clock_phase = phase;
        hspi1_modes.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
        hspi1_modes.Init.data_size = SPI_DATA_SIZE_8BIT;
        hspi1_modes.Init.baud_rate = SPI_BAUD_RATE_DIV_8;
        hspi1_modes.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
        hspi1_modes.Init.nss_mode = SPI_NSS_SOFTWARE;
        hspi1_modes.Init.crc_enabled = false;
        
        SPI_Init(&hspi1_modes);
        SPI_Enable(&hspi1_modes);
        
        // Simple transfer
        mode_transfer_done = false;
        SPI_SetNSSPin(&hspi1_modes, false);
        
        // Polling for this example (in real app, use interrupts)
        SPI_SendData8(&hspi1_modes, tx_mode);
        while (!SPI_IsRXNE(&hspi1_modes));
        uint8_t rx = SPI_ReceiveData8(&hspi1_modes);
        
        SPI_WaitNotBusy(&hspi1_modes);
        SPI_SetNSSPin(&hspi1_modes, true);
        
        printf("  Sent: %02X, Received: %02X\r\n", tx_mode, rx);
    }
    
    printf("\r\nAll modes tested!\r\n");
}

//============================================================================
// GPIO and Clock Setup (Common for all examples)
//============================================================================

void GPIO_Init_Common(void)
{
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

void Clock_Config(void)
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
// Main Function - Select Example to Run
//============================================================================

int main(void)
{
    Clock_Config();
    GPIO_Init_Common();
    
    printf("SPI Interrupt-Based Examples\r\n");
    printf("===========================\r\n\r\n");
    
    // Uncomment one of the following to test:
    
    // Example 1: Master Full-Duplex 8-bit Interrupt
    Example1_Master_FullDuplex_8bit_Interrupt();
    
    // Example 2: Master Full-Duplex 16-bit Interrupt
    // Example2_Master_FullDuplex_16bit_Interrupt();
    
    // Example 3: Master Simplex TX Interrupt
    // Example3_Master_Simplex_Tx_Interrupt();
    
    // Example 4: Slave Full-Duplex Interrupt
    // Example4_Slave_FullDuplex_Interrupt();
    
    // Example 5: Master CRC Interrupt
    // Example5_Master_CRC_Interrupt();
    
    // Example 6: All Modes (polling for simplicity)
    // Example6_Master_AllModes_Interrupt();
    
    while (1) {
        // Main loop - process interrupts
    }
}

//============================================================================
// Combined IRQ Handler (would be in startup file)
//============================================================================

/**
 * @brief SPI1 Interrupt Service Routine
 * @note This handler should be placed in the startup file or vector table
 * 
 * For different examples, use the corresponding handler:
 * - Example 1: SPI1_IRQHandler_Example1()
 * - Example 2: SPI1_IRQHandler_Example2()
 * - Example 3: SPI1_IRQHandler_Example3()
 * - Example 4: SPI1_IRQHandler_Example4()
 * - Example 5: SPI1_IRQHandler_Example5()
 */
void SPI1_IRQHandler(void)
{
    // Call Example 1 handler (Master Full-Duplex 8-bit Interrupt)
    SPI1_IRQHandler_Example1();
    
    // For other examples, uncomment the corresponding handler:
    // SPI1_IRQHandler_Example2();
    // SPI1_IRQHandler_Example3();
    // SPI1_IRQHandler_Example4();
    // SPI1_IRQHandler_Example5();
}

//============================================================================
// Interrupt Usage Notes:
//============================================================================
/*
 * Interrupt-Based SPI Communication:
 * 
 * 1. Enable NVIC for SPI1 (IRQn = 23)
 * 2. Enable SPI interrupts in CR2:
 *    - TXEIE: TX buffer empty interrupt
 *    - RXNEIE: RX buffer not empty interrupt
 *    - ERRIE: Error interrupt
 * 3. In TXE handler: Send next data or disable TXE if done
 * 4. In RXNE handler: Read received data
 * 5. Handle errors (OVR, MODF, CRCERR) in ERRIE
 * 6. For CRC: Set CRCNEXT after last data byte
 * 
 * Example IRQ handler flow:
 * void SPI1_IRQHandler(void) {
 *     if (SPI1->SR & SPI_SR_TXE) {
 *         // Send data
 *     }
 *     if (SPI1->SR & SPI_SR_RXNE) {
 *         // Receive data
 *     }
 *     if (SPI1->SR & SPI_SR_OVR) {
 *         // Handle overrun
 *     }
 * }
 */

//============================================================================
// End of File
//============================================================================
