/**
 * @file    Slave_DMA_Interrupt_FullDuplex_8bit.c
 * @brief   SPI Slave - Full Duplex, 8-bit Data Frame, DMA Mode with Interrupts
 * @author  AI Generated
 * @date    2026-03-02
 * 
 * @note    This example demonstrates SPI slave communication in full duplex mode
 *          with 8-bit data frame size using DMA with interrupt for transfer complete.
 * 
 * Hardware Setup:
 *   SPI1 (Slave):
 *   - PA5: SCK  (Clock - from Master)
 *   - PA6: MISO (Master Input Slave Output)
 *   - PA7: MOSI (Master Output Slave Input)
 *   
 *   DMA:
 *   - DMA1 Channel 3: SPI1 TX
 *   - DMA1 Channel 2: SPI1 RX
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
volatile bool transferComplete = false;

/*============================================================================
 * Function Prototypes
 *============================================================================*/
void SystemClock_Config(void);
void SPI1_Init(void);
void DMA_Init(void);

/**
 * @brief  Main program
 */
int main(void) {
    /* Configure system clock */
    SystemClock_Config();
    
    /* Initialize DMA for SPI */
    DMA_Init();
    
    /* Initialize SPI1 as Slave */
    SPI1_Init();
    
    /* Enable SPI1 */
    SPI_Enable(SPI1);
    
    while (1) {
        /* Wait for transfer to complete */
        while (!transferComplete);
        
        /* Reset flag */
        transferComplete = false;
        
        /* Optional: Process received data here */
        
        /* Re-enable DMA for next transfer */
        DMA_CCR3 |= DMA_CR_EN;
        DMA_CCR2 |= DMA_CR_EN;
        
        /* Enable SPI to start transfer */
        SPI_Enable(SPI1);
        
        /* Optional: Add delay between transfers */
        for (volatile uint32_t delay = 0; delay < 100000; delay++);
    }
}

/**
 * @brief  DMA1 Channel 2/3 IRQ Handler for RX/TX Complete
 */
void DMA1_Channel2_3_IRQHandler(void) {
    uint32_t spiStatus;
    
    /* Check TX transfer complete */
    if (DMA_ISR & (1 << 9)) {  /* TCIF3 */
        /* Clear flags */
        DMA_IFCR = (1 << 9);
        
        /* Wait for SPI to finish */
        while (SPI_GetStatus(SPI1) & SPI_SR_BSY);
        
        /* Check SPI status for errors */
        spiStatus = SPI_GetStatus(SPI1);
        
        if (spiStatus & SPI_SR_OVR) {
            /* Overrun error - clear by reading DR */
            volatile uint8_t temp = SPI_ReceiveData(SPI1);
            (void)temp;
        }
        
        if (spiStatus & SPI_SR_MODF) {
            /* Mode fault error */
        }
        
        /* Mark transfer complete */
        transferComplete = true;
    }
    
    /* Check for DMA errors */
    if (DMA_ISR & (1 << 8)) {  /* TEIF3 */
        /* Transfer error - clear flag */
        DMA_IFCR = (1 << 8);
    }
}

/**
 * @brief  Initialize SPI1 as Slave with DMA
 *         - Full duplex mode
 *         - 8-bit data frame
 *         - Clock: Phase 1st edge, Polarity Low (must match Master)
 *         - MSB first
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
    
    /* Disable CRC */
    SPI_InitStruct.CRC_Enable = false;
    
    /* Initialize SPI1 */
    SPI_Init(SPI1, &SPI_InitStruct);
    
    /* Enable DMA for TX and RX */
    SPI_EnableDMA(SPI1, SPI_DMA_DIR_TX_RX);
}

/**
 * @brief  Initialize DMA for SPI communication with interrupts
 */
void DMA_Init(void) {
    /* Enable DMA1 clock */
    RCC_EnablePeripheralClock(PERIPH_DMA1, 1);
    
    /* Configure TX DMA Channel (Channel 3 for SPI1 TX) */
    /* Direction: Memory to Peripheral */
    DMA_CCR3 = 0;
    DMA_CCR3 |= DMA_CR_DIR;           /* Read from memory */
    DMA_CCR3 |= DMA_CR_MINC;          /* Memory increment */
    DMA_CCR3 &= ~DMA_CR_CIRC;         /* Disable circular mode */
    DMA_CCR3 &= ~(DMA_CR_PSIZE | DMA_CR_MSIZE);  /* 8-bit size */
    DMA_CCR3 |= (0x02 << 12);         /* Medium priority */
    DMA_CCR3 |= DMA_CR_TCIE;          /* Transfer complete interrupt */
    
    /* Configure RX DMA Channel (Channel 2 for SPI1 RX) */
    /* Direction: Peripheral to Memory */
    DMA_CCR2 = 0;
    DMA_CCR2 &= ~DMA_CR_DIR;          /* Read from peripheral */
    DMA_CCR2 |= DMA_CR_MINC;          /* Memory increment */
    DMA_CCR2 &= ~DMA_CR_CIRC;         /* Disable circular mode */
    DMA_CCR2 &= ~(DMA_CR_PSIZE | DMA_CR_MSIZE);  /* 8-bit size */
    DMA_CCR2 |= (0x02 << 12);         /* Medium priority */
    DMA_CCR2 |= DMA_CR_TCIE;          /* Transfer complete interrupt */
    
    /* Configure TX DMA (Channel 3) */
    DMA_CPAR3 = (uint32_t)&SPI1->DR;  /* Peripheral address */
    DMA_CMAR3 = (uint32_t)txBuffer;   /* Memory address */
    DMA_CNDTR3 = BUFFER_SIZE;         /* Number of data */
    
    /* Configure RX DMA (Channel 2) */
    DMA_CPAR2 = (uint32_t)&SPI1->DR;  /* Peripheral address */
    DMA_CMAR2 = (uint32_t)rxBuffer;   /* Memory address */
    DMA_CNDTR2 = BUFFER_SIZE;         /* Number of data */
    
    /* Enable DMA channels */
    DMA_CCR3 |= DMA_CR_EN;
    DMA_CCR2 |= DMA_CR_EN;
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
