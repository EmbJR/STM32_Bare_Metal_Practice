/**
 * @file    spiF051.c
 * @brief   SPI Driver Implementation for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-03-02
 */

#include "spiF051.h"
#include "../../GPIO_AI/gpio.h"
#include "../../DeepSeek_Generated/RCC/rcc.h"

/*============================================================================
 * Private Macros
 *============================================================================*/
#define SPI_TIMEOUT_MAX    10000UL

/*============================================================================
 * Private Function Prototypes
 *============================================================================*/
static void SPI_ConfigGPIO(SPI_TypeDef *SPIx);
static uint32_t SPI_GetClockFreq(SPI_TypeDef *SPIx);

/*============================================================================
 * SPI Initialization and Configuration
 *============================================================================*/

/**
 * @brief  Initialize SPI peripheral
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 * @param  SPI_Init: pointer to SPI_InitTypeDef structure
 */
void SPI_Init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_Init) {
    uint32_t cr1 = 0;
    uint32_t cr2 = 0;
    
    /* Enable SPI clock */
    if (SPIx == SPI1) {
        RCC_EnablePeripheralClock(PERIPH_SPI1, 1);
    } else if (SPIx == SPI2) {
        RCC_EnablePeripheralClock(PERIPH_SPI2, 2);
    }
    
    /* Disable SPI before configuration */
    SPIx->CR1 &= ~SPI_CR1_SPE;
    
    /* Configure CR1 */
    cr1 = SPIx->CR1;
    
    /* Set Master/Slave mode */
    if (SPI_Init->Mode == SPI_MODE_MASTER) {
        cr1 |= SPI_CR1_MSTR;
    } else {
        cr1 &= ~SPI_CR1_MSTR;
    }
    
    /* Set Clock Polarity */
    if (SPI_Init->ClockPolarity == SPI_CPOL_HIGH) {
        cr1 |= SPI_CR1_CPOL;
    } else {
        cr1 &= ~SPI_CR1_CPOL;
    }
    
    /* Set Clock Phase */
    if (SPI_Init->ClockPhase == SPI_CPHA_2EDGE) {
        cr1 |= SPI_CR1_CPHA;
    } else {
        cr1 &= ~SPI_CR1_CPHA;
    }
    
    /* Set Baud Rate Prescaler (only for master) */
    if (SPI_Init->Mode == SPI_MODE_MASTER) {
        cr1 &= ~SPI_CR1_BR;
        cr1 |= SPI_Init->BaudRate;
    }
    
    /* Set LSBFIRST */
    cr1 &= ~SPI_CR1_LSBFIRST;
    cr1 |= SPI_Init->FrameFormat;
    
    /* Set Software Slave Management */
    cr1 |= SPI_CR1_SSM;  /* Software NSS management */
    cr1 |= SPI_CR1_SSI;  /* Internal slave select (for master) */
    
    /* Set CRC */
    if (SPI_Init->CRC_Enable) {
        cr1 |= SPI_CR1_CRCEN;
    } else {
        cr1 &= ~SPI_CR1_CRCEN;
    }
    
    SPIx->CR1 = cr1;
    
    /* Configure CR2 */
    cr2 = SPIx->CR2;
    
    /* Set Data Size */
    cr2 &= ~SPI_CR2_DS;
    cr2 |= SPI_Init->DataSize;
    
    /* Set FIFO Threshold (for 8-bit data, threshold at 4 bits) */
    if (SPI_Init->FIFOThreshold) {
        cr2 |= SPI_CR2_FRXTH;
    } else {
        cr2 &= ~SPI_CR2_FRXTH;
    }
    
    /* SS Output Enable for master */
    if (SPI_Init->Mode == SPI_MODE_MASTER) {
        if (SPI_Init->NSS == SPI_NSS_HARD_OUTPUT) {
            cr2 |= SPI_CR2_SSOE;
        } else {
            cr2 &= ~SPI_CR2_SSOE;
        }
    }
    
    SPIx->CR2 = cr2;
    
    /* Configure CRC polynomial if enabled */
    if (SPI_Init->CRC_Enable) {
        SPIx->CRCPR = SPI_Init->CRC_Polynomial;
    }
    
    /* Configure GPIO */
    SPI_ConfigGPIO(SPIx);
}

/**
 * @brief  Configure GPIO for SPI
 * @param  SPIx: pointer to SPI peripheral
 */
static void SPI_ConfigGPIO(SPI_TypeDef *SPIx) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /* Configure based on SPI instance */
    if (SPIx == SPI1) {
        /* SPI1: PA5=SCK, PA6=MISO, PA7=MOSI */
        
        /* Enable GPIO clocks */
        GPIO_EnableClock(GPIOA);
        
        /* PA5 - SPI1_SCK */
        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Pull = GPIO_PULL_NO;
        GPIO_InitStruct.AF = GPIO_AF0;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        /* PA6 - SPI1_MISO */
        GPIO_InitStruct.Pin = GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Pull = GPIO_PULL_NO;
        GPIO_InitStruct.AF = GPIO_AF0;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        /* PA7 - SPI1_MOSI */
        GPIO_InitStruct.Pin = GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Pull = GPIO_PULL_NO;
        GPIO_InitStruct.AF = GPIO_AF0;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
        
    } else if (SPIx == SPI2) {
        /* SPI2: PB13=SCK, PB14=MISO, PB15=MOSI */
        
        /* Enable GPIO clocks */
        GPIO_EnableClock(GPIOB);
        
        /* PB13 - SPI2_SCK */
        GPIO_InitStruct.Pin = GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Pull = GPIO_PULL_NO;
        GPIO_InitStruct.AF = GPIO_AF0;
        GPIO_Init(GPIOB, &GPIO_InitStruct);
        
        /* PB14 - SPI2_MISO */
        GPIO_InitStruct.Pin = GPIO_PIN_14;
        GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Pull = GPIO_PULL_NO;
        GPIO_InitStruct.AF = GPIO_AF0;
        GPIO_Init(GPIOB, &GPIO_InitStruct);
        
        /* PB15 - SPI2_MOSI */
        GPIO_InitStruct.Pin = GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Pull = GPIO_PULL_NO;
        GPIO_InitStruct.AF = GPIO_AF0;
        GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

/**
 * @brief  De-Initialize SPI peripheral
 * @param  SPIx: pointer to SPI peripheral (SPI1 or SPI2)
 */
void SPI_DeInit(SPI_TypeDef *SPIx) {
    /* Disable SPI */
    SPIx->CR1 &= ~SPI_CR1_SPE;
    
    /* Disable peripheral clock */
    if (SPIx == SPI1) {
        RCC_DisablePeripheralClock(PERIPH_SPI1, 1);
    } else if (SPIx == SPI2) {
        RCC_DisablePeripheralClock(PERIPH_SPI2, 2);
    }
}

/**
 * @brief  Fill initialization structure with default values
 * @param  SPI_InitStruct: pointer to SPI_InitTypeDef structure
 */
void SPI_StructInit(SPI_InitTypeDef *SPI_Init) {
    SPI_Init->Mode = SPI_MODE_SLAVE;
    SPI_Init->BaudRate = SPI_BAUDRATEPRESCALER_8;
    SPI_Init->ClockPolarity = SPI_CPOL_LOW;
    SPI_Init->ClockPhase = SPI_CPHA_1EDGE;
    SPI_Init->DataSize = SPI_DATASIZE_8BIT;
    SPI_Init->NSS = SPI_NSS_SOFT;
    SPI_Init->FrameFormat = SPI_FRAME_FORMAT_MSBFIRST;
    SPI_Init->CRC_Enable = false;
    SPI_Init->CRC_Polynomial = 7;
    SPI_Init->FIFOThreshold = false;
}

/**
 * @brief  Enable SPI peripheral
 * @param  SPIx: pointer to SPI peripheral
 */
void SPI_Enable(SPI_TypeDef *SPIx) {
    SPIx->CR1 |= SPI_CR1_SPE;
}

/**
 * @brief  Disable SPI peripheral
 * @param  SPIx: pointer to SPI peripheral
 */
void SPI_Disable(SPI_TypeDef *SPIx) {
    /* Wait for TXFIFO empty */
    while ((SPIx->SR & SPI_SR_FTLVL) != 0) {
        /* Wait */
    }
    
    /* Wait for busy flag */
    while (SPIx->SR & SPI_SR_BSY) {
        /* Wait */
    }
    
    /* Disable SPI */
    SPIx->CR1 &= ~SPI_CR1_SPE;
    
    /* Read any remaining data */
    while (SPIx->SR & SPI_SR_RXNE) {
        volatile uint8_t temp = (uint8_t)SPIx->DR;
        (void)temp;
    }
}

/*============================================================================
 * SPI Data Transfer Functions (Polling)
 *============================================================================*/

/**
 * @brief  Transmit and Receive a data byte (Full Duplex - Polling)
 * @param  SPIx: pointer to SPI peripheral
 * @param  txData: data to transmit
 * @return received data
 */
uint8_t SPI_TransmitReceive(SPI_TypeDef *SPIx, uint8_t txData) {
    uint32_t timeout = SPI_TIMEOUT_MAX;
    
    /* Wait for TX buffer empty */
    while (!(SPIx->SR & SPI_SR_TXE)) {
        if (timeout-- == 0) {
            return 0;
        }
    }
    
    /* Send data */
    SPIx->DR = txData;
    
    /* Wait for RX buffer not empty */
    timeout = SPI_TIMEOUT_MAX;
    while (!(SPIx->SR & SPI_SR_RXNE)) {
        if (timeout-- == 0) {
            return 0;
        }
    }
    
    /* Return received data */
    return (uint8_t)SPIx->DR;
}

/**
 * @brief  Transmit a data byte (Polling)
 * @param  SPIx: pointer to SPI peripheral
 * @param  txData: data to transmit
 */
void SPI_SendData(SPI_TypeDef *SPIx, uint8_t txData) {
    uint32_t timeout = SPI_TIMEOUT_MAX;
    
    /* Wait for TX buffer empty */
    while (!(SPIx->SR & SPI_SR_TXE)) {
        if (timeout-- == 0) {
            return;
        }
    }
    
    /* Send data */
    SPIx->DR = txData;
}

/**
 * @brief  Receive a data byte (Polling)
 * @param  SPIx: pointer to SPI peripheral
 * @return received data
 */
uint8_t SPI_ReceiveData(SPI_TypeDef *SPIx) {
    uint32_t timeout = SPI_TIMEOUT_MAX;
    
    /* Wait for RX buffer not empty */
    while (!(SPIx->SR & SPI_SR_RXNE)) {
        if (timeout-- == 0) {
            return 0;
        }
    }
    
    /* Return received data */
    return (uint8_t)SPIx->DR;
}

/**
 * @brief  Transmit and Receive multiple data bytes (Full Duplex - Polling)
 * @param  SPIx: pointer to SPI peripheral
 * @param  txBuffer: pointer to transmit buffer
 * @param  rxBuffer: pointer to receive buffer
 * @param  length: number of bytes to transfer
 */
void SPI_TransmitReceiveBuffer(SPI_TypeDef *SPIx, uint8_t *txBuffer, uint8_t *rxBuffer, uint16_t length) {
    uint16_t i;
    
    for (i = 0; i < length; i++) {
        /* Wait for TX buffer empty */
        while (!(SPIx->SR & SPI_SR_TXE)) {
            /* Wait */
        }
        
        /* Send data */
        if (txBuffer != 0) {
            SPIx->DR = txBuffer[i];
        } else {
            SPIx->DR = 0xFF;  /* Dummy byte */
        }
        
        /* Wait for RX buffer not empty */
        while (!(SPIx->SR & SPI_SR_RXNE)) {
            /* Wait */
        }
        
        /* Receive data */
        if (rxBuffer != 0) {
            rxBuffer[i] = (uint8_t)SPIx->DR;
        } else {
            volatile uint8_t temp = (uint8_t)SPIx->DR;
            (void)temp;
        }
    }
}

/**
 * @brief  Transmit multiple data bytes (Polling)
 * @param  SPIx: pointer to SPI peripheral
 * @param  txBuffer: pointer to transmit buffer
 * @param  length: number of bytes to transmit
 */
void SPI_SendBuffer(SPI_TypeDef *SPIx, uint8_t *txBuffer, uint16_t length) {
    uint16_t i;
    
    for (i = 0; i < length; i++) {
        /* Wait for TX buffer empty */
        while (!(SPIx->SR & SPI_SR_TXE)) {
            /* Wait */
        }
        
        /* Send data */
        SPIx->DR = txBuffer[i];
    }
    
    /* Wait for transmission to complete */
    while (SPIx->SR & SPI_SR_BSY) {
        /* Wait */
    }
}

/**
 * @brief  Receive multiple data bytes (Polling)
 * @param  SPIx: pointer to SPI peripheral
 * @param  rxBuffer: pointer to receive buffer
 * @param  length: number of bytes to receive
 */
void SPI_ReceiveBuffer(SPI_TypeDef *SPIx, uint8_t *rxBuffer, uint16_t length) {
    uint16_t i;
    
    for (i = 0; i < length; i++) {
        /* Send dummy byte to generate clock */
        while (!(SPIx->SR & SPI_SR_TXE)) {
            /* Wait */
        }
        SPIx->DR = 0xFF;
        
        /* Wait for RX buffer not empty */
        while (!(SPIx->SR & SPI_SR_RXNE)) {
            /* Wait */
        }
        
        /* Receive data */
        rxBuffer[i] = (uint8_t)SPIx->DR;
    }
}

/*============================================================================
 * SPI Interrupt Functions
 *============================================================================*/

/**
 * @brief  Enable SPI interrupts
 * @param  SPIx: pointer to SPI peripheral
 * @param  interrupt: interrupt source to enable
 */
void SPI_EnableInterrupt(SPI_TypeDef *SPIx, uint32_t interrupt) {
    SPIx->CR2 |= interrupt;
}

/**
 * @brief  Disable SPI interrupts
 * @param  SPIx: pointer to SPI peripheral
 * @param  interrupt: interrupt source to disable
 */
void SPI_DisableInterrupt(SPI_TypeDef *SPIx, uint32_t interrupt) {
    SPIx->CR2 &= ~interrupt;
}

/**
 * @brief  Get SPI interrupt status
 * @param  SPIx: pointer to SPI peripheral
 * @param  interrupt: interrupt source to check
 * @return interrupt status
 */
uint32_t SPI_GetInterruptStatus(SPI_TypeDef *SPIx, uint32_t interrupt) {
    return (SPIx->CR2 & interrupt) ? 1 : 0;
}

/**
 * @brief  Clear SPI interrupt flag
 * @param  SPIx: pointer to SPI peripheral
 * @param  flag: flag to clear
 */
void SPI_ClearFlag(SPI_TypeDef *SPIx, uint32_t flag) {
    /* Flags that need to be cleared by software read */
    if (flag & (SPI_SR_OVR | SPI_SR_MODF)) {
        volatile uint8_t temp = (uint8_t)SPIx->DR;
        (void)temp;
        temp = (uint8_t)SPIx->SR;
        (void)temp;
    }
}

/*============================================================================
 * SPI Status Functions
 *============================================================================*/

/**
 * @brief  Get SPI status flags
 * @param  SPIx: pointer to SPI peripheral
 * @return SPI status register value
 */
uint32_t SPI_GetStatus(SPI_TypeDef *SPIx) {
    return SPIx->SR;
}

/**
 * @brief  Check if TX buffer is empty
 * @param  SPIx: pointer to SPI peripheral
 * @return true if TX buffer is empty
 */
bool SPI_IsTXEmpty(SPI_TypeDef *SPIx) {
    return (SPIx->SR & SPI_SR_TXE) ? true : false;
}

/**
 * @brief  Check if RX buffer is not empty
 * @param  SPIx: pointer to SPI peripheral
 * @return true if RX buffer has data
 */
bool SPI_IsRXNotEmpty(SPI_TypeDef *SPIx) {
    return (SPIx->SR & SPI_SR_RXNE) ? true : false;
}

/**
 * @brief  Check if SPI is busy
 * @param  SPIx: pointer to SPI peripheral
 * @return true if SPI is busy
 */
bool SPI_IsBusy(SPI_TypeDef *SPIx) {
    return (SPIx->SR & SPI_SR_BSY) ? true : false;
}

/*============================================================================
 * SPI CRC Functions
 *============================================================================*/

/**
 * @brief  Enable CRC
 * @param  SPIx: pointer to SPI peripheral
 */
void SPI_EnableCRC(SPI_TypeDef *SPIx) {
    SPIx->CR1 |= SPI_CR1_CRCEN;
}

/**
 * @brief  Disable CRC
 * @param  SPIx: pointer to SPI peripheral
 */
void SPI_DisableCRC(SPI_TypeDef *SPIx) {
    SPIx->CR1 &= ~SPI_CR1_CRCEN;
}

/**
 * @brief  Reset CRC
 * @param  SPIx: pointer to SPI peripheral
 */
void SPI_ResetCRC(SPI_TypeDef *SPIx) {
    /* Disable CRC */
    SPIx->CR1 &= ~SPI_CR1_CRCEN;
    
    /* Enable CRC */
    SPIx->CR1 |= SPI_CR1_CRCEN;
}

/**
 * @brief  Get transmitted CRC value
 * @param  SPIx: pointer to SPI peripheral
 * @return transmitted CRC value
 */
uint16_t SPI_GetTransmitCRC(SPI_TypeDef *SPIx) {
    return SPIx->TXCRCR;
}

/**
 * @brief  Get received CRC value
 * @param  SPIx: pointer to SPI peripheral
 * @return received CRC value
 */
uint16_t SPI_GetReceiveCRC(SPI_TypeDef *SPIx) {
    return SPIx->RXCRCR;
}

/**
 * @brief  Check if CRC error occurred
 * @param  SPIx: pointer to SPI peripheral
 * @return true if CRC error occurred
 */
bool SPI_IsCRCError(SPI_TypeDef *SPIx) {
    return (SPIx->SR & SPI_SR_CRCERR) ? true : false;
}

/**
 * @brief  Clear CRC error flag
 * @param  SPIx: pointer to SPI peripheral
 */
void SPI_ClearCRCError(SPI_TypeDef *SPIx) {
    SPIx->SR = ~SPI_SR_CRCERR;
}

/*============================================================================
 * SPI DMA Functions
 *============================================================================*/

/**
 * @brief  Enable SPI DMA
 * @param  SPIx: pointer to SPI peripheral
 * @param  direction: DMA direction
 */
void SPI_EnableDMA(SPI_TypeDef *SPIx, uint8_t direction) {
    if (direction & SPI_DMA_DIR_TX) {
        SPIx->CR2 |= SPI_CR2_TXDMAEN;
    }
    if (direction & SPI_DMA_DIR_RX) {
        SPIx->CR2 |= SPI_CR2_RXDMAEN;
    }
}

/**
 * @brief  Disable SPI DMA
 * @param  SPIx: pointer to SPI peripheral
 * @param  direction: DMA direction
 */
void SPI_DisableDMA(SPI_TypeDef *SPIx, uint8_t direction) {
    if (direction & SPI_DMA_DIR_TX) {
        SPIx->CR2 &= ~SPI_CR2_TXDMAEN;
    }
    if (direction & SPI_DMA_DIR_RX) {
        SPIx->CR2 &= ~SPI_CR2_RXDMAEN;
    }
}

/*============================================================================
 * DMA Functions
 *============================================================================*/

/**
 * @brief  Configure DMA channel for SPI
 * @param  channel: DMA channel number (1-7)
 * @param  peripheralAddr: peripheral address (SPI data register)
 * @param  memoryAddr: memory address
 * @param  dataSize: number of data to transfer
 * @param  direction: transfer direction (0=read from peripheral, 1=read from memory)
 * @param  memoryIncrement: enable memory increment
 */
void DMA_ConfigChannel(uint8_t channel, uint32_t peripheralAddr, uint32_t memoryAddr, 
                       uint16_t dataSize, uint8_t direction, bool memoryIncrement) {
    volatile uint32_t *CCR;
    volatile uint32_t *CPAR;
    volatile uint32_t *CMAR;
    volatile uint32_t *CNDTR;
    uint32_t cr_value = 0;
    
    /* Get register addresses based on channel */
    switch (channel) {
        case 1:
            CCR = &DMA_CCR1; CPAR = &DMA_CPAR1; CMAR = &DMA_CMAR1; CNDTR = &DMA_CNDTR1;
            break;
        case 2:
            CCR = &DMA_CCR2; CPAR = &DMA_CPAR2; CMAR = &DMA_CMAR2; CNDTR = &DMA_CNDTR2;
            break;
        case 3:
            CCR = &DMA_CCR3; CPAR = &DMA_CPAR3; CMAR = &DMA_CMAR3; CNDTR = &DMA_CNDTR3;
            break;
        case 4:
            CCR = &DMA_CCR4; CPAR = &DMA_CPAR4; CMAR = &DMA_CMAR4; CNDTR = &DMA_CNDTR4;
            break;
        case 5:
            CCR = &DMA_CCR5; CPAR = &DMA_CPAR5; CMAR = &DMA_CMAR5; CNDTR = &DMA_CNDTR5;
            break;
        case 6:
            CCR = &DMA_CCR6; CPAR = &DMA_CPAR6; CMAR = &DMA_CMAR6; CNDTR = &DMA_CNDTR6;
            break;
        case 7:
            CCR = &DMA_CCR7; CPAR = &DMA_CPAR7; CMAR = &DMA_CMAR7; CNDTR = &DMA_CNDTR7;
            break;
        default:
            return;
    }
    
    /* Disable channel first */
    *CCR &= ~DMA_CR_EN;
    
    /* Configure direction */
    if (direction) {
        cr_value |= DMA_CR_DIR;  /* Read from memory */
    } else {
        cr_value &= ~DMA_CR_DIR;  /* Read from peripheral */
    }
    
    /* Configure memory increment */
    if (memoryIncrement) {
        cr_value |= DMA_CR_MINC;
    }
    
    /* Set data size to byte (8-bit) */
    cr_value &= ~(DMA_CR_PSIZE | DMA_CR_MSIZE);
    
    /* Set priority */
    cr_value |= (0x02 << 12);  /* Medium priority */
    
    /* Configure other settings */
    cr_value &= ~DMA_CR_CIRC;  /* Disable circular mode */
    
    /* Write configuration */
    *CCR = cr_value;
    
    /* Set peripheral address */
    *CPAR = peripheralAddr;
    
    /* Set memory address */
    *CMAR = memoryAddr;
    
    /* Set number of data to transfer */
    *CNDTR = dataSize;
}

/**
 * @brief  Enable DMA channel
 * @param  channel: DMA channel number (1-7)
 */
void DMA_EnableChannel(uint8_t channel) {
    volatile uint32_t *CCR;
    
    switch (channel) {
        case 1: CCR = &DMA_CCR1; break;
        case 2: CCR = &DMA_CCR2; break;
        case 3: CCR = &DMA_CCR3; break;
        case 4: CCR = &DMA_CCR4; break;
        case 5: CCR = &DMA_CCR5; break;
        case 6: CCR = &DMA_CCR6; break;
        case 7: CCR = &DMA_CCR7; break;
        default: return;
    }
    
    *CCR |= DMA_CR_EN;
}

/**
 * @brief  Disable DMA channel
 * @param  channel: DMA channel number (1-7)
 */
void DMA_DisableChannel(uint8_t channel) {
    volatile uint32_t *CCR;
    
    switch (channel) {
        case 1: CCR = &DMA_CCR1; break;
        case 2: CCR = &DMA_CCR2; break;
        case 3: CCR = &DMA_CCR3; break;
        case 4: CCR = &DMA_CCR4; break;
        case 5: CCR = &DMA_CCR5; break;
        case 6: CCR = &DMA_CCR6; break;
        case 7: CCR = &DMA_CCR7; break;
        default: return;
    }
    
    *CCR &= ~DMA_CR_EN;
}

/**
 * @brief  Get DMA channel status flags
 * @param  channel: DMA channel number (1-7)
 * @return status flags
 */
uint32_t DMA_GetStatusFlags(uint8_t channel) {
    uint32_t flag_base = (channel - 1) * 4;
    return DMA_ISR & (0x0F << flag_base);
}

/**
 * @brief  Clear DMA channel flags
 * @param  channel: DMA channel number (1-7)
 * @param  flags: flags to clear
 */
void DMA_ClearFlags(uint8_t channel, uint32_t flags) {
    uint32_t flag_base = (channel - 1) * 4;
    DMA_IFCR = (flags << flag_base);
}

/**
 * @brief  Check if DMA transfer is complete
 * @param  channel: DMA channel number (1-7)
 * @return true if transfer is complete
 */
bool DMA_IsTransferComplete(uint8_t channel) {
    uint32_t flag_base = (channel - 1) * 4;
    return (DMA_ISR & (DMA_FLAG_TCIF << flag_base)) ? true : false;
}

/*============================================================================
 * NVIC Functions for SPI
 *============================================================================*/

/**
 * @brief  Enable SPI interrupt in NVIC
 * @param  IRQn: interrupt number
 */
void NVIC_EnableSPI(uint8_t IRQn) {
    NVIC_EnableIRQ(IRQn);
}

/**
 * @brief  Disable SPI interrupt in NVIC
 * @param  IRQn: interrupt number
 */
void NVIC_DisableSPI(uint8_t IRQn) {
    NVIC_DisableIRQ(IRQn);
}

/**
 * @brief  Set SPI interrupt priority
 * @param  IRQn: interrupt number
 * @param  priority: priority (0-3)
 */
void NVIC_SetSPIPriority(uint8_t IRQn, uint8_t priority) {
    NVIC_SetPriority(IRQn, priority);
}

/*============================================================================
 * Helper Functions
 *============================================================================*/

/**
 * @brief  Get SPI clock frequency
 * @param  SPIx: pointer to SPI peripheral
 * @return clock frequency in Hz
 */
static uint32_t SPI_GetClockFreq(SPI_TypeDef *SPIx) {
    uint32_t clock_freq = 0;
    
    if (SPIx == SPI1) {
        /* SPI1 is on APB2 */
        clock_freq = RCC_GetSystemClockFrequency();
        /* Apply APB2 prescaler if needed */
    } else if (SPIx == SPI2) {
        /* SPI2 is on APB1 */
        clock_freq = RCC_GetSystemClockFrequency();
        /* Apply APB1 prescaler if needed */
    }
    
    return clock_freq;
}
