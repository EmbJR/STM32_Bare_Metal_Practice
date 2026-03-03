/**
 * @file    Master_Polling_FullDuplex_8bit.c
 * @brief   SPI Master - Full Duplex, 8-bit Data Frame, Polling Mode
 * @author  AI Generated
 * @date    2026-03-02
 * 
 * @note    This example demonstrates SPI master communication in full duplex mode
 *          with 8-bit data frame size using polling (blocking) method.
 * 
 * Hardware Setup:
 *   SPI1 (Master):
 *   - PA5: SCK  (Clock)
 *   - PA6: MISO (Master Input Slave Output)
 *   - PA7: MOSI (Master Output Slave Input)
 *   
 *   Connect to Slave device:
 *   - SCK  -> SCK
 *   - MISO -> MISO
 *   - MOSI -> MOSI
 */

#include "spiF051.h"
#include "rcc.h"
#include "gpio.h"

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

void SPI_gpio_cs_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Enable GPIOC clock for spi_cs PA4 */
	GPIO_EnableClock(GPIOA);

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULL_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_SetPin(GPIOA, GPIO_PIN_4);
}

/**
 * @brief  Main program
 */
int main(void) {
    uint16_t i;
    uint32_t spiStatus;
    
    /* Configure system clock */
    SystemClock_Config();
    
    SPI_gpio_cs_init();

    /* Initialize SPI1 as Master */
    SPI1_Init();
    
    /* Enable SPI1 */
    SPI_Enable(SPI1);
    
    while (1) {

    	GPIO_ResetPin(GPIOA, GPIO_PIN_4);
        /* Check SPI status before transfer */
        spiStatus = SPI_GetStatus(SPI1);
        
        /* Verify SPI is not busy and ready for transmission */
        if (!(spiStatus & SPI_SR_BSY) && (spiStatus & SPI_SR_TXE)) {
            /* Transmit and Receive data in full duplex mode */
            for (i = 0; i < BUFFER_SIZE; i++) {
                /* Wait for TX buffer empty */
                while (!(SPI_GetStatus(SPI1) & SPI_SR_TXE));
                
                /* Send data and receive response simultaneously */
                rxBuffer[i] = SPI_TransmitReceive(SPI1, txBuffer[i]);
            }
            
            /* Wait for transmission to complete */
            while (SPI_GetStatus(SPI1) & SPI_SR_BSY);
            
            /* Check for any error flags */
            spiStatus = SPI_GetStatus(SPI1);
            if (spiStatus & SPI_SR_OVR) {
                /* Overrun error - clear by reading DR */
                volatile uint8_t temp = SPI_ReceiveData(SPI1);
                (void)temp;
            }
            if (spiStatus & SPI_SR_MODF) {
                /* Mode fault error */
            }
        }
        GPIO_SetPin(GPIOA, GPIO_PIN_4);
        
        /* Optional: Add delay between transfers */
        for (volatile uint32_t delay = 0; delay < 1000; delay++);
    }
}

/**
 * @brief  Initialize SPI1 as Master
 *         - Full duplex mode
 *         - 8-bit data frame
 *         - Clock: Phase 1st edge, Polarity Low
 *         - Baud rate: fPCLK/8
 *         - MSB first
 *         - Software NSS management
 */
void SPI1_Init(void) {
    SPI_InitTypeDef SPI_InitStruct;
    
    /* Fill with default values */
    SPI_StructInit(&SPI_InitStruct);
    
    /* Configure as Master */
    SPI_InitStruct.Mode = SPI_MODE_MASTER;
    
    /* Configure clock: Baud rate = fPCLK/8 */
    SPI_InitStruct.BaudRate = SPI_BAUDRATEPRESCALER_64;
    
    /* Configure clock polarity (CPOL = 0: clock low when idle) */
    SPI_InitStruct.ClockPolarity = SPI_CPOL_LOW;
    
    /* Configure clock phase (CPHA = 0: first clock transition is first data capture edge) */
    SPI_InitStruct.ClockPhase = SPI_CPHA_1EDGE;
    
    /* Configure data size: 8-bit */
    SPI_InitStruct.DataSize = SPI_DATASIZE_8BIT;
    
    /* Configure NSS: Software management */
    SPI_InitStruct.NSS = SPI_NSS_SOFT;
    
    /* Configure frame format: MSB first */
    SPI_InitStruct.FrameFormat = SPI_FRAME_FORMAT_MSBFIRST;
    
    /* Configure frame format: MSB first */
    SPI_InitStruct.FIFOThreshold = true;

    /* Disable CRC */
    SPI_InitStruct.CRC_Enable = false;
    
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
