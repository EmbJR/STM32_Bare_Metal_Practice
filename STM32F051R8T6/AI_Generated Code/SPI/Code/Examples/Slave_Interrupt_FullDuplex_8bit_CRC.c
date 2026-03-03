/**
 * @file    Slave_Interrupt_FullDuplex_8bit_CRC.c
 * @brief   SPI Slave - Full Duplex, 8-bit Data Frame, CRC Enabled, Interrupt Mode
 * @author  AI Generated
 * @date    2026-03-02
 * 
 * @note    This example demonstrates SPI slave communication in full duplex mode
 *          with 8-bit data frame size and CRC enabled using interrupt method.
 * 
 * Hardware Setup:
 *   SPI1 (Slave):
 *   - PA5: SCK  (Clock - from Master)
 *   - PA6: MISO (Master Input Slave Output)
 *   - PA7: MOSI (Master Output Slave Input)
 */

#include "spiF051.h"
#include "../../DeepSeek_Generated/RCC/rcc.h"

/*============================================================================
 * Macros
 *============================================================================*/
#define BUFFER_SIZE    8

/*============================================================================
 * Global Variables
 *============================================================================*/
uint8_t txBuffer[BUFFER_SIZE] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80};
uint8_t rxBuffer[BUFFER_SIZE] = {0};
volatile uint8_t txIndex = 0;
volatile uint8_t rxIndex = 0;
volatile bool transferComplete = false;
volatile bool crcError = false;

/*============================================================================
 * Function Prototypes
 *============================================================================*/
void SystemClock_Config(void);
void SPI1_Init(void);
void SPI1_IRQHandler(void);

/**
 * @brief  Main program
 */
int main(void) {
    /* Configure system clock */
    SystemClock_Config();
    
    /* Initialize SPI1 as Slave with CRC */
    SPI1_Init();
    
    /* Enable SPI1 */
    SPI_Enable(SPI1);
    
    while (1) {
        /* Wait for transfer to complete */
        while (!transferComplete);
        
        /* Check for CRC error */
        if (crcError) {
            /* Handle CRC error */
            crcError = false;
        }
        
        /* Reset flags and indexes */
        transferComplete = false;
        txIndex = 0;
        rxIndex = 0;
        
        /* Reset CRC for next transfer */
        SPI_ResetCRC(SPI1);
        
        /* Optional: Add delay between transfers */
        for (volatile uint32_t delay = 0; delay < 100000; delay++);
    }
}

/**
 * @brief  SPI1 IRQ Handler
 */
void SPI1_IRQHandler(void) {
    uint32_t spiStatus = SPI_GetStatus(SPI1);
    
    /* Check for errors */
    if (spiStatus & SPI_SR_OVR) {
        /* Overrun error - clear by reading DR */
        volatile uint8_t temp = SPI_ReceiveData(SPI1);
        (void)temp;
    }
    
    if (spiStatus & SPI_SR_MODF) {
        /* Mode fault error */
    }
    
    /* Check for CRC error */
    if (spiStatus & SPI_SR_CRCERR) {
        crcError = true;
        SPI_ClearCRCError(SPI1);
    }
    
    /* Check if TX buffer is empty */
    if ((spiStatus & SPI_SR_TXE) && (txIndex < BUFFER_SIZE)) {
        /* Send next byte */
        SPI_SendData(SPI1, txBuffer[txIndex]);
        txIndex++;
    }
    
    /* Check if RX buffer has data */
    if (spiStatus & SPI_SR_RXNE) {
        /* Read received byte */
        rxBuffer[rxIndex] = SPI_ReceiveData(SPI1);
        rxIndex++;
        
        /* Check if transfer is complete */
        if (rxIndex >= BUFFER_SIZE) {
            transferComplete = true;
        }
    }
}

/**
 * @brief  Initialize SPI1 as Slave with CRC and interrupts
 *         - Full duplex mode
 *         - 8-bit data frame
 *         - CRC enabled
 *         - Clock: Phase 1st edge, Polarity Low (must match Master)
 *         - MSB first
 *         - TXE, RXNE, and ERR interrupts enabled
 */
void SPI1_Init(void) {
    SPI_InitTypeDef SPI_InitStruct;
    
    /* Fill with default values */
    SPI_StructInit(&SPI_InitStruct);
    
    /* Configure as Slave */
    SPI_InitStruct.Mode = SPI_MODE_SLAVE;
    
    /* Configure clock polarity (must match Master) */
    SPI_InitStruct.ClockPolarity = SPI_CPOL_LOW;
    
    /* Configure clock phase (must match Master) */
    SPI_InitStruct.ClockPhase = SPI_CPHA_1EDGE;
    
    /* Configure data size: 8-bit */
    SPI_InitStruct.DataSize = SPI_DATASIZE_8BIT;
    
    /* Configure NSS: Software management */
    SPI_InitStruct.NSS = SPI_NSS_SOFT;
    
    /* Configure frame format: MSB first */
    SPI_InitStruct.FrameFormat = SPI_FRAME_FORMAT_MSBFIRST;
    
    /* Enable CRC - must match Master's polynomial */
    SPI_InitStruct.CRC_Enable = true;
    SPI_InitStruct.CRC_Polynomial = 7;
    
    /* Initialize SPI1 */
    SPI_Init(SPI1, &SPI_InitStruct);
    
    /* Enable TXE, RXNE and Error interrupts */
    SPI_EnableInterrupt(SPI1, SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR);
    
    /* Enable SPI1 interrupt in NVIC */
    NVIC_EnableSPI(SPI1_IRQn);
    NVIC_SetSPIPriority(SPI1_IRQn, 1);
}

/**
 * @brief  System Clock Configuration
 */
void SystemClock_Config(void) {
    RCC_Config rcc_config;
    
    /* Configure RCC for 48MHz using PLL with HSI */
    rcc_config.system_clock_source = CLOCK_SOURCE_PLL;
    rcc_config.target_frequency = SYSTEM_CLOCK_48MHZ;
    rcc_config.hse_enabled = false;
    rcc_config.pll_enabled = true;
    rcc_config.pll_source = PLL_SOURCE_HSI_DIV2;
    rcc_config.pll_multiplier = 12;  /* HSI (8MHz/2) * 12 = 48MHz */
    rcc_config.ahb_prescaler = AHB_PRESCALER_1;
    rcc_config.apb_prescaler = APB_PRESCALER_1;
    rcc_config.hsi48_enabled = false;
    rcc_config.css_enabled = false;
    
    /* Initialize RCC */
    RCC_Init(&rcc_config);
}
