/**
 * @file    Master_Polling_FullDuplex_8bit_CRC.c
 * @brief   SPI Master - Full Duplex, 8-bit Data Frame, CRC Enabled, Polling Mode
 * @author  AI Generated
 * @date    2026-03-02
 * 
 * @note    This example demonstrates SPI master communication in full duplex mode
 *          with 8-bit data frame size and CRC enabled using polling method.
 * 
 * Hardware Setup:
 *   SPI1 (Master):
 *   - PA5: SCK  (Clock)
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
uint8_t txBuffer[BUFFER_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t rxBuffer[BUFFER_SIZE] = {0};

/*============================================================================
 * Function Prototypes
 *============================================================================*/
void SystemClock_Config(void);
void SPI1_Init(void);
uint8_t SPI_TransferWithCRC(SPI_TypeDef *SPIx, uint8_t *txData, uint8_t *rxData, uint16_t length);

/**
 * @brief  Main program
 */
int main(void) {
    uint16_t i;
    uint32_t spiStatus;
    uint16_t txCRC, rxCRC;
    
    /* Configure system clock */
    SystemClock_Config();
    
    /* Initialize SPI1 as Master with CRC */
    SPI1_Init();
    
    /* Enable SPI1 */
    SPI_Enable(SPI1);
    
    while (1) {
        /* Reset CRC before each transfer */
        SPI_ResetCRC(SPI1);
        
        /* Check SPI status before transfer */
        spiStatus = SPI_GetStatus(SPI1);
        
        /* Verify SPI is not busy and ready for transmission */
        if (!(spiStatus & SPI_SR_BSY) && (spiStatus & SPI_SR_TXE)) {
            /* Transmit and Receive data with CRC */
            for (i = 0; i < BUFFER_SIZE; i++) {
                /* Wait for TX buffer empty */
                while (!(SPI_GetStatus(SPI1) & SPI_SR_TXE));
                
                rxBuffer[i] = SPI_TransmitReceive(SPI1, txBuffer[i]);
            }
            
            /* Wait for transmission to complete */
            while (SPI_GetStatus(SPI1) & SPI_SR_BSY);
            
            /* Check for CRC error */
            if (SPI_IsCRCError(SPI1)) {
                /* Handle CRC error */
                SPI_ClearCRCError(SPI1);
            }
            
            /* Get CRC values for verification */
            txCRC = SPI_GetTransmitCRC(SPI1);
            rxCRC = SPI_GetReceiveCRC(SPI1);
            
            /* Check for any error flags */
            spiStatus = SPI_GetStatus(SPI1);
            if (spiStatus & SPI_SR_OVR) {
                /* Overrun error */
                volatile uint8_t temp = SPI_ReceiveData(SPI1);
                (void)temp;
            }
        }
        
        /* Optional: Add delay between transfers */
        for (volatile uint32_t delay = 0; delay < 100000; delay++);
    }
}

/**
 * @brief  Initialize SPI1 as Master with CRC enabled
 *         - Full duplex mode
 *         - 8-bit data frame
 *         - CRC enabled
 *         - Clock: Phase 1st edge, Polarity Low
 *         - Baud rate: fPCLK/8
 *         - MSB first
 */
void SPI1_Init(void) {
    SPI_InitTypeDef SPI_InitStruct;
    
    /* Fill with default values */
    SPI_StructInit(&SPI_InitStruct);
    
    /* Configure as Master */
    SPI_InitStruct.Mode = SPI_MODE_MASTER;
    
    /* Configure clock: Baud rate = fPCLK/8 */
    SPI_InitStruct.BaudRate = SPI_BAUDRATEPRESCALER_8;
    
    /* Configure clock polarity */
    SPI_InitStruct.ClockPolarity = SPI_CPOL_LOW;
    
    /* Configure clock phase */
    SPI_InitStruct.ClockPhase = SPI_CPHA_1EDGE;
    
    /* Configure data size: 8-bit */
    SPI_InitStruct.DataSize = SPI_DATASIZE_8BIT;
    
    /* Configure NSS: Software management */
    SPI_InitStruct.NSS = SPI_NSS_SOFT;
    
    /* Configure frame format: MSB first */
    SPI_InitStruct.FrameFormat = SPI_FRAME_FORMAT_MSBFIRST;
    
    /* Enable CRC */
    SPI_InitStruct.CRC_Enable = true;
    SPI_InitStruct.CRC_Polynomial = 7;  /* Default CRC polynomial */
    
    /* Initialize SPI1 */
    SPI_Init(SPI1, &SPI_InitStruct);
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
