/**
 * @file    Master_Interrupt_FullDuplex_8bit.c
 * @brief   SPI Master - Full Duplex, 8-bit Data Frame, Interrupt Mode
 * @author  AI Generated
 * @date    2026-03-02
 * 
 * @note    This example demonstrates SPI master communication in full duplex mode
 *          with 8-bit data frame size using interrupt method.
 * 
 * Hardware Setup:
 *   SPI1 (Master):
 *   - PA5: SCK  (Clock)
 *   - PA6: MISO (Master Input Slave Output)
 *   - PA7: MOSI (Master Output Slave Input)
 */
 
#include <stdint.h>
#include "enc28j60.h"
#include "spiF051.h"
#include "rcc.h"
#include "gpio.h"
#include "main.h"

/*============================================================================
 * Macros
 *============================================================================*/
#define BUFFER_SIZE    8

/*============================================================================
 * Global Variables
 *============================================================================*/
uint8_t txBuffer[BUFFER_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t rxBuffer[BUFFER_SIZE] = {0};
volatile uint8_t txIndex = 0;
volatile uint8_t rxIndex = 0;
volatile bool transferComplete = false;
ESP28J60_HandleTypeDef enc28j60_handle;

/*============================================================================
 * Function Prototypes
 *============================================================================*/
void SPI1_IRQHandler(void);
static void enc28j60_gpio_config(ESP28J60_HandleTypeDef *enc_handle);
void enc28j60_SPI_Initialize(ESP28J60_HandleTypeDef * SPI);

static void enc28j60_gpio_config(ESP28J60_HandleTypeDef *enc_handle)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    /* SPI2: PB13=SCK, PB14=MISO, PB15=MOSI */
        
    /* Enable GPIO clocks */
    GPIO_EnableClock(GPIOB);

    /* Configure CS pin as output */
    GPIO_InitStruct.Pin = enc_handle->CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_Init(enc_handle->CS_Port, &GPIO_InitStruct);
    
    /* PB13 - SPI2_SCK */
    GPIO_InitStruct.Pin = ESP28J60_SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_InitStruct.AF = GPIO_AF0;
    GPIO_Init(ESP28J60_GPIO_PORT, &GPIO_InitStruct);
    
    /* PB14 - SPI2_MISO */
    GPIO_InitStruct.Pin = ESP28J60_MISO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_InitStruct.AF = GPIO_AF0;
    GPIO_Init(ESP28J60_GPIO_PORT, &GPIO_InitStruct);
    
    /* PB15 - SPI2_MOSI */
    GPIO_InitStruct.Pin = ESP28J60_MOSI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_InitStruct.AF = GPIO_AF0;
    GPIO_Init(ESP28J60_GPIO_PORT, &GPIO_InitStruct);
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
 * @brief  Initialize SPI1 as Master with interrupts
 *         - Full duplex mode
 *         - 8-bit data frame
 *         - Clock: Phase 1st edge, Polarity Low
 *         - Baud rate: fPCLK/8
 *         - MSB first
 *         - TXE and RXNE interrupts enabled
 */
void enc28j60_SPI_Initialize(ESP28J60_HandleTypeDef * handle) {
    SPI_InitTypeDef SPI_InitStruct;
    
    enc28j60_gpio_config(handle);
#if 1
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
    
    /* Disable CRC */
    SPI_InitStruct.CRC_Enable = false;
    
    SPI_InitStruct.FIFOThreshold = true;

    /* Initialize SPI1 */
    SPI_Init(handle->SPIx, &SPI_InitStruct);
    

    /* Enable TXE and RXNE interrupts */
    //SPI_EnableInterrupt(handle->SPIx, SPI_IT_TXE | SPI_IT_RXNE);
    
    /* Enable SPI1 interrupt in NVIC */
    //NVIC_EnableSPI(SPI2_IRQn);
    //NVIC_SetSPIPriority(SPI2_IRQn, 1);

    /* Enable SPI1 */
    SPI_Enable(handle->SPIx);
#endif
//    SPI_InitStruct.Mode = SPI_MODE_MASTER;
//   SPI_InitStruct.ClockPolarity = SPI_CPOL_LOW;   /* Clock idle low */
//   SPI_InitStruct.ClockPhase = SPI_CPHA_1EDGE;    /* Data captured on first edge */
//   SPI_InitStruct.BaudRate = SPI_BAUDRATEPRESCALER_16;  /* Adjust as needed */
//   SPI_InitStruct.FrameFormat = SPI_FRAME_FORMAT_MSBFIRST;
//   SPI_InitStruct.DataSize = SPI_DATASIZE_8BIT;
//   SPI_InitStruct.NSS = SPI_NSS_SOFT;  /* Software NSS control */
//   SPI_InitStruct.FIFOThreshold = true;

//   /* Initialize SPI */
//   SPI_Init(SPI2, &SPI_InitStruct);
//
//   /* Enable SPI */
//   SPI_Enable(SPI2);
}

/**
 * @brief  Assert chip select
 */
static void ESP28J60_CSelect(ESP28J60_HandleTypeDef *handle) {
    ESP28J60_CS_LOW(handle);
}

/**
 * @brief  De-assert chip select
 */
static void ESP28J60_CDeselect(ESP28J60_HandleTypeDef *handle) {
    ESP28J60_CS_HIGH(handle);
}


void enc28j60_init(void)
{
    /* Initialize AT45DB handle */
    enc28j60_handle.SPIx = (void*)ESP28J60_SPI;
    enc28j60_handle.CS_Port = (void*)ESP28J60_GPIO_PORT;
    enc28j60_handle.CS_Pin = ESP28J60_CS_PIN;

    enc28j60_SPI_Initialize(&enc28j60_handle);
}

#if 1
bool enc28j60_test(void)
{
    uint8_t datatx[2] = {0xA5, 0x5A};
    uint8_t datarx[2] = {0};
    while(1)
    {
    	ESP28J60_CSelect(&enc28j60_handle);
        SPI_TransmitReceiveBuffer(enc28j60_handle.SPIx, datatx,datarx, sizeof(datatx));
        ESP28J60_CDeselect(&enc28j60_handle);
        Delay_ms(100);
    }
}
    #endif
