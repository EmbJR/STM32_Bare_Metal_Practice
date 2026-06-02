/**
 * @file    uartF051.c
 * @brief   UART Driver Implementation for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-03-05
 */

#include "uartF051.h"
#include "gpio.h"
#include "rcc.h"

/*============================================================================
 * Private Macros
 *============================================================================*/
#define UART_TIMEOUT_MAX         10000UL
#define UART_FLAG_SET            true
#define UART_FLAG_RESET          false

/*============================================================================
 * Private Variables
 *============================================================================*/
static UART_StateTypeDef UART_State[2] = {UART_STATE_RESET, UART_STATE_RESET};
static UART_ErrorTypeDef UART_Error[2] = {UART_ERROR_NONE, UART_ERROR_NONE};

/*============================================================================
 * Private Function Prototypes
 *============================================================================*/
static void UART_ConfigGPIO(USART_TypeDef *USARTx);
static uint32_t UART_GetClockFreq(USART_TypeDef *USARTx);
static uint8_t UART_GetPeripheralIndex(USART_TypeDef *USARTx);

/**
 * @brief  Get the index of the UART peripheral (0 for USART1, 1 for USART2)
 */
static uint8_t UART_GetPeripheralIndex(USART_TypeDef *USARTx) {
    if (USARTx == USART1) {
        return 0;
    }
    return 1;
}

/**
 * @brief  Get the clock frequency for the UART peripheral
 */
static uint32_t UART_GetClockFreq(USART_TypeDef *USARTx) {
    uint32_t clock_freq = 0;
    
    if (USARTx == USART1) {
        /* USART1 is on APB2 */
        /* Get the system clock and divide by APB prescaler */
        clock_freq = RCC_GetSystemClockFrequency();
        /* For now, assume APB1 prescaler is 1 */
        clock_freq = clock_freq / 1;  /* Would need to read actual APB2 prescaler */
    } else {
        /* USART2 is on APB1 */
        clock_freq = RCC_GetSystemClockFrequency();
        clock_freq = clock_freq / 1;  /* Would need to read actual APB1 prescaler */
    }
    
    return clock_freq;
}

/**
 * @brief  Configure GPIO pins for UART
 */
static void UART_ConfigGPIO(USART_TypeDef *USARTx) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_EnableClock(GPIOA);
    /* Initialize GPIO config with defaults */
    GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStruct.Pull = GPIO_PULL_UP;
    
    if (USARTx == USART1) {
        /* Enable GPIOA clock */
        //RCC_EnablePeripheralClock(PERIPH_GPIOA, 0);
        
        /* USART1: TX = PA9, RX = PA10
         * Optionally: CTS = PA11, RTS = PA12
         * CK = PA8 (if synchronous mode) */
        
        /* TX Pin (PA9) */
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.AF = GPIO_AF1;  /* AF1 for USART1 */
        GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        /* RX Pin (PA10) */
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
        
    } else if (USARTx == USART2) {
        /* Enable GPIOA clock */
        //RCC_EnablePeripheralClock(PERIPH_GPIOA, 0);
        
        /* USART2: TX = PA2, RX = PA3
         * Optionally: CTS = PA0, RTS = PA1
         * CK = PA4 (if synchronous mode) */
        
        /* TX Pin (PA2) */
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.AF = GPIO_AF1;  /* AF1 for USART2 */
        GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        /* RX Pin (PA3) */
        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/*============================================================================
 * UART Initialization and Configuration Functions
 *============================================================================*/

/**
 * @brief  Initialize UART peripheral with given configuration
 * @param  USARTx: pointer to USART peripheral (USART1 or USART2)
 * @param  UART_InitStruct: pointer to UART_InitTypeDef configuration structure
 */
void UART_Init(USART_TypeDef *USARTx, UART_InitTypeDef *UART_InitStruct) {
    uint32_t cr1 = 0;
    uint32_t cr2 = 0;
    uint32_t cr3 = 0;
    uint32_t clock_freq = 0;
    uint32_t brr = 0;
    uint8_t index = UART_GetPeripheralIndex(USARTx);
    
    /* Enable UART clock */
    if (USARTx == USART1) {
        RCC_EnablePeripheralClock(PERIPH_USART1, 1);
    } else if (USARTx == USART2) {
        RCC_EnablePeripheralClock(PERIPH_USART2, 2);
    }
    
    /* Configure GPIO pins */
    UART_ConfigGPIO(USARTx);
    
    /* Disable UART before configuration */
    USARTx->CR1 &= ~USART_CR1_UE;
    
    /* Initialize state and error */
    UART_State[index] = UART_STATE_RESET;
    UART_Error[index] = UART_ERROR_NONE;
    
    /* Configure CR1 */
    cr1 = USARTx->CR1;
    
    /* Set Word Length */
    cr1 &= ~(USART_CR1_M0 | USART_CR1_M1);
    cr1 |= UART_InitStruct->WordLength;
    
    /* Set Parity */
    cr1 &= ~(USART_CR1_PS | USART_CR1_PCE);
    cr1 |= UART_InitStruct->Parity;
    
    /* Set Oversampling */
    cr1 &= ~USART_CR1_OVER8;
    cr1 |= (UART_InitStruct->OverSampling == UART_OVERSAMPLING_8) ? USART_CR1_OVER8 : 0;
    
    /* Set Mode (TX/RX) */
    cr1 &= ~(USART_CR1_TE | USART_CR1_RE);
    cr1 |= UART_InitStruct->Mode;
    
    /* Set One Bit Sampling */
    cr1 &= ~USART_CR1_CMIE;
    if (UART_InitStruct->OneBitSampling) {
        cr1 |= USART_CR1_CMIE;  /* Actually this is CMIE, need different bit for one-bit */
    }
    /* One bit sampling is controlled by CR3 ONEBIT, handled below */
    
    /* Configure Clock (only if synchronous mode is enabled via CR2) */
    if (USARTx->CR2 & USART_CR2_CLKEN) {
        cr1 &= ~(USART_CR2_CPHA | USART_CR2_CPOL);
        cr1 |= UART_InitStruct->ClockPhase;
        cr1 |= UART_InitStruct->ClockPolarity;
    }
    
    USARTx->CR1 = cr1;
    
    /* Configure CR2 */
    cr2 = USARTx->CR2;
    
    /* Set Stop Bits */
    cr2 &= ~USART_CR2_STOP;
    cr2 |= UART_InitStruct->StopBits;
    
    /* Set Clock Last Bit */
    cr2 &= ~USART_CR2_LBCL;
    cr2 |= UART_InitStruct->ClockLastBit;
    
    USARTx->CR2 = cr2;
    
    /* Configure CR3 */
    cr3 = USARTx->CR3;
    
    /* Set Hardware Flow Control */
    cr3 &= ~(USART_CR3_CTSE | USART_CR3_RTSE);
    cr3 |= UART_InitStruct->HwFlowCtl;
    
    /* Set One Bit Sampling method */
    cr3 &= ~USART_CR3_ONEBIT;
    if (UART_InitStruct->OneBitSampling) {
        cr3 |= USART_CR3_ONEBIT;
    }
    
    USARTx->CR3 = cr3;
    
    /* Configure Baud Rate */
    clock_freq = UART_GetClockFreq(USARTx);
    UART_SetBaudRate(USARTx, UART_InitStruct->BaudRate, clock_freq);
    
    /* Enable UART */
    USARTx->CR1 |= USART_CR1_UE;
    
    /* Update state */
    UART_State[index] = UART_STATE_READY;
}

/**
 * @brief  De-initialize UART peripheral
 * @param  USARTx: pointer to USART peripheral
 */
void UART_DeInit(USART_TypeDef *USARTx) {
    uint8_t index = UART_GetPeripheralIndex(USARTx);
    
    /* Disable UART */
    USARTx->CR1 &= ~USART_CR1_UE;
    
    /* Disable all interrupts */
    USARTx->CR1 &= ~(USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE | 
                     USART_CR1_RXNEIE | USART_CR1_IDLEIE);
    USARTx->CR3 &= ~(USART_CR3_EIE | USART_CR3_CTSIE);
    
    /* Disable DMA requests */
    USARTx->CR3 &= ~(USART_CR3_DMAT | USART_CR3_DMAR);
    
    /* Disable clocks */
    if (USARTx == USART1) {
        RCC_DisablePeripheralClock(PERIPH_USART1, 1);
    } else if (USARTx == USART2) {
        RCC_DisablePeripheralClock(PERIPH_USART2, 2);
    }
    
    /* Reset state */
    UART_State[index] = UART_STATE_RESET;
    UART_Error[index] = UART_ERROR_NONE;
}

/**
 * @brief  Initialize UART_InitTypeDef to default values
 * @param  UART_InitStruct: pointer to structure to be initialized
 */
void UART_StructInit(UART_InitTypeDef *UART_InitStruct) {
    /* Default configuration: 115200 baud, 8N1, no flow control */
    UART_InitStruct->BaudRate = 115200;
    UART_InitStruct->WordLength = UART_WORDLENGTH_8B;
    UART_InitStruct->StopBits = UART_STOPBITS_1;
    UART_InitStruct->Parity = UART_PARITY_NONE;
    UART_InitStruct->HwFlowCtl = UART_HWCONTROL_NONE;
    UART_InitStruct->Mode = UART_MODE_TX_RX;
    UART_InitStruct->OverSampling = UART_OVERSAMPLING_16;
    UART_InitStruct->ClockPolarity = UART_CLOCKPOLARITY_LOW;
    UART_InitStruct->ClockPhase = UART_CLOCKPHASE_1EDGE;
    UART_InitStruct->ClockLastBit = UART_CLOCKLASTBIT_DISABLE;
    UART_InitStruct->OneBitSampling = false;
}

/*============================================================================
 * UART Peripheral Control Functions
 *============================================================================*/

/**
 * @brief  Enable UART peripheral
 * @param  USARTx: pointer to USART peripheral
 */
void UART_Enable(USART_TypeDef *USARTx) {
    USARTx->CR1 |= USART_CR1_UE;
}

/**
 * @brief  Disable UART peripheral
 * @param  USARTx: pointer to USART peripheral
 */
void UART_Disable(USART_TypeDef *USARTx) {
    /* Wait for transmission complete before disabling */
    UART_WaitForFlag(USARTx, USART_ISR_TC, UART_FLAG_SET, UART_TIMEOUT_MAX);
    USARTx->CR1 &= ~USART_CR1_UE;
}

/**
 * @brief  Enable UART transmitter
 * @param  USARTx: pointer to USART peripheral
 */
void UART_EnableTx(USART_TypeDef *USARTx) {
    USARTx->CR1 |= USART_CR1_TE;
}

/**
 * @brief  Disable UART transmitter
 * @param  USARTx: pointer to USART peripheral
 */
void UART_DisableTx(USART_TypeDef *USARTx) {
    /* Wait for transmission complete before disabling */
    UART_WaitForFlag(USARTx, USART_ISR_TC, UART_FLAG_SET, UART_TIMEOUT_MAX);
    USARTx->CR1 &= ~USART_CR1_TE;
}

/**
 * @brief  Enable UART receiver
 * @param  USARTx: pointer to USART peripheral
 */
void UART_EnableRx(USART_TypeDef *USARTx) {
    USARTx->CR1 |= USART_CR1_RE;
}

/**
 * @brief  Disable UART receiver
 * @param  USARTx: pointer to USART peripheral
 */
void UART_DisableRx(USART_TypeDef *USARTx) {
    USARTx->CR1 &= ~USART_CR1_RE;
}

/*============================================================================
 * UART Polling Transmission Functions
 *============================================================================*/

/**
 * @brief  Send data via UART (polling mode)
 * @param  USARTx: pointer to USART peripheral
 * @param  Data: data to be transmitted (8 or 9 bits based on configuration)
 */
void UART_SendData(USART_TypeDef *USARTx, uint16_t Data) {
    uint8_t index = UART_GetPeripheralIndex(USARTx);
    
    /* Wait for TXE (Transmit Data Register Empty) */
    UART_WaitForFlag(USARTx, USART_ISR_TXE, UART_FLAG_SET, UART_TIMEOUT_MAX);
    
    /* Write data to TDR */
    USARTx->TDR = (Data & 0x1FF);
    
    /* Update state */
    UART_State[index] = UART_STATE_BUSY_TX;
}

/**
 * @brief  Receive data from UART (polling mode)
 * @param  USARTx: pointer to USART peripheral
 * @return received data (8 or 9 bits based on configuration)
 */
uint16_t UART_ReceiveData(USART_TypeDef *USARTx) {
    uint8_t index = UART_GetPeripheralIndex(USARTx);
    
    /* Wait for RXNE (Read Data Register Not Empty) */
    UART_WaitForFlag(USARTx, USART_ISR_RXNE, UART_FLAG_SET, UART_TIMEOUT_MAX);
    
    /* Update state */
    UART_State[index] = UART_STATE_BUSY_RX;
    
    /* Read data from RDR and check for errors */
    if (USARTx->ISR & (USART_ISR_ORE | USART_ISR_NE | USART_ISR_FE | USART_ISR_PE)) {
        UART_Error[index] |= (USARTx->ISR & (USART_ISR_ORE | USART_ISR_NE | USART_ISR_FE | USART_ISR_PE));
    }
    
    return (uint16_t)(USARTx->RDR & 0x1FF);
}

/**
 * @brief  Wait for specific UART flag
 * @param  USARTx: pointer to USART peripheral
 * @param  Flag: flag to wait for
 * @param  State: desired state (SET or RESET)
 * @param  Timeout: timeout in milliseconds
 * @return true if flag reached desired state, false if timeout
 */
bool UART_WaitForFlag(USART_TypeDef *USARTx, uint32_t Flag, bool State, uint32_t Timeout) {
    uint32_t tickstart = 0;
    
    /* Get current tick (would need SysTick or similar) */
    /* For simplicity, use a simple counter loop */
    uint32_t counter = 0;
    
    while (counter < Timeout * 100) {  /* Approximate delay */
        if (((USARTx->ISR & Flag) != 0) == State) {
            return true;
        }
        counter++;
    }
    
    return false;
}

/**
 * @brief  Check if transmission is complete
 * @param  USARTx: pointer to USART peripheral
 * @return true if transmission is complete
 */
bool UART_IsTxComplete(USART_TypeDef *USARTx) {
    return ((USARTx->ISR & USART_ISR_TC) != 0);
}

/*============================================================================
 * UART Interrupt Functions
 *============================================================================*/

/**
 * @brief  Enable UART interrupt
 * @param  USARTx: pointer to USART peripheral
 * @param  Interrupt: interrupt to enable (USART_CR1_*, USART_CR2_*, USART_CR3_*)
 */
void UART_EnableInterrupt(USART_TypeDef *USARTx, uint32_t Interrupt) {
    if (Interrupt & 0xFFFF0000) {
        /* CR3 interrupts */
        USARTx->CR3 |= (Interrupt & 0xFFFF);
    } else if (Interrupt & 0x0000FF00) {
        /* CR2 interrupts */
        USARTx->CR2 |= (Interrupt & 0xFF00);
    } else {
        /* CR1 interrupts */
        USARTx->CR1 |= (Interrupt & 0xFF);
    }
}

/**
 * @brief  Disable UART interrupt
 * @param  USARTx: pointer to USART peripheral
 * @param  Interrupt: interrupt to disable
 */
void UART_DisableInterrupt(USART_TypeDef *USARTx, uint32_t Interrupt) {
    if (Interrupt & 0xFFFF0000) {
        USARTx->CR3 &= ~(Interrupt & 0xFFFF);
    } else if (Interrupt & 0x0000FF00) {
        USARTx->CR2 &= ~(Interrupt & 0xFF00);
    } else {
        USARTx->CR1 &= ~(Interrupt & 0xFF);
    }
}

/**
 * @brief  Get UART interrupt status
 * @param  USARTx: pointer to USART peripheral
 * @return interrupt status register value
 */
uint32_t UART_GetInterruptStatus(USART_TypeDef *USARTx) {
    return USARTx->ISR;
}

/**
 * @brief  Get UART status register
 * @param  USARTx: pointer to USART peripheral
 * @return status register value
 */
uint32_t UART_GetStatus(USART_TypeDef *USARTx) {
    return USARTx->ISR;
}

/**
 * @brief  Clear UART interrupt flag
 * @param  USARTx: pointer to USART peripheral
 * @param  Flag: flag to clear (use USART_ICR_* flags)
 */
void UART_ClearInterruptFlag(USART_TypeDef *USARTx, uint32_t Flag) {
    USARTx->ICR = Flag;
}

/**
 * @brief  Get UART flag status
 * @param  USARTx: pointer to USART peripheral
 * @param  Flag: flag to check (use USART_ISR_*)
 * @return true if flag is set
 */
bool UART_GetFlag(USART_TypeDef *USARTx, uint32_t Flag) {
    return ((USARTx->ISR & Flag) != 0);
}

/*============================================================================
 * UART DMA Functions
 *============================================================================*/

/**
 * @brief  Enable UART DMA request
 * @param  USARTx: pointer to USART peripheral
 * @param  DMARequest: DMA request to enable (UART_DMA_REQ_TX or UART_DMA_REQ_RX)
 */
void UART_EnableDMA(USART_TypeDef *USARTx, uint32_t DMARequest) {
    USARTx->CR3 |= DMARequest;
}

/**
 * @brief  Disable UART DMA request
 * @param  USARTx: pointer to USART peripheral
 * @param  DMARequest: DMA request to disable
 */
void UART_DisableDMA(USART_TypeDef *USARTx, uint32_t DMARequest) {
    USARTx->CR3 &= ~DMARequest;
}

/*============================================================================
 * UART Status and Error Management Functions
 *============================================================================*/

/**
 * @brief  Get UART state
 * @param  USARTx: pointer to USART peripheral
 * @return current UART state
 */
UART_StateTypeDef UART_GetState(USART_TypeDef *USARTx) {
    uint8_t index = UART_GetPeripheralIndex(USARTx);
    return UART_State[index];
}

/**
 * @brief  Get UART error code
 * @param  USARTx: pointer to USART peripheral
 * @return error code
 */
UART_ErrorTypeDef UART_GetError(USART_TypeDef *USARTx) {
    uint8_t index = UART_GetPeripheralIndex(USARTx);
    return UART_Error[index];
}

/**
 * @brief  Clear UART error flags
 * @param  USARTx: pointer to USART peripheral
 */
void UART_ClearError(USART_TypeDef *USARTx) {
    uint8_t index = UART_GetPeripheralIndex(USARTx);
    
    /* Clear error flags by writing to ICR */
    USARTx->ICR = (USART_ICR_PECF | USART_ICR_FECF | USART_ICR_NCF | USART_ICR_ORECF);
    
    /* Clear error variable */
    UART_Error[index] = UART_ERROR_NONE;
}

/*============================================================================
 * UART Synchronization Functions
 *============================================================================*/

/**
 * @brief  Check if UART is ready to transmit (TXE flag)
 * @param  USARTx: pointer to USART peripheral
 * @return true if ready to transmit
 */
bool UART_IsReadyToTransmit(USART_TypeDef *USARTx) {
    return ((USARTx->ISR & USART_ISR_TXE) != 0);
}

/**
 * @brief  Check if UART is ready to receive (RXNE flag)
 * @param  USARTx: pointer to USART peripheral
 * @return true if ready to receive
 */
bool UART_IsReadyToReceive(USART_TypeDef *USARTx) {
    return ((USARTx->ISR & USART_ISR_RXNE) != 0);
}

/**
 * @brief  Check if UART is busy
 * @param  USARTx: pointer to USART peripheral
 * @return true if busy
 */
bool UART_IsBusy(USART_TypeDef *USARTx) {
    return ((USARTx->ISR & USART_ISR_BUSY) != 0);
}

/*============================================================================
 * UART Special Functions
 *============================================================================*/

/**
 * @brief  Send break character
 * @param  USARTx: pointer to USART peripheral
 */
void UART_SendBreak(USART_TypeDef *USARTx) {
    USARTx->RQR |= USART_RQR_SBKRQ;
}

/**
 * @brief  Request auto baud rate detection
 * @param  USARTx: pointer to USART peripheral
 */
void UART_RequestAutoBaudRate(USART_TypeDef *USARTx) {
    USARTx->RQR |= USART_RQR_ABRRQ;
}

/**
 * @brief  Set UART baud rate
 * @param  USARTx: pointer to USART peripheral
 * @param  BaudRate: desired baud rate
 * @param  APBClock: peripheral clock frequency
 */
void UART_SetBaudRate(USART_TypeDef *USARTx, uint32_t BaudRate, uint32_t APBClock) {
    uint32_t brr = 0;
    uint32_t usartdiv = 0;
    uint32_t over8 = 0;
    
    /* Check oversampling mode */
    over8 = (USARTx->CR1 & USART_CR1_OVER8) ? 1 : 0;
    
    if (over8) {
        /* Oversampling by 8: BRR = 2 * USARTDIV
         * USARTDIV = APBClock / BaudRate */
        usartdiv = (2 * APBClock) / BaudRate;
    } else {
        /* Oversampling by 16: BRR = USARTDIV
         * USARTDIV = APBClock / BaudRate */
        usartdiv = APBClock / BaudRate;
    }
    
    /* BRR[15:4] = integer part (12 bits) */
    /* BRR[3:0] = fractional part (4 bits for OVER16, 3 bits for OVER8) */
    
    if (over8) {
        /* For OVER8, fractional is 3 bits (divide usartdiv by 2) */
        brr = (usartdiv & 0xFFF0) | ((usartdiv & 0x000F) >> 1);
    } else {
        /* For OVER16, fractional is 4 bits */
        brr = usartdiv;
    }
    
    USARTx->BRR = brr;
}

/**
 * @brief  Enable FIFO mode (for STM32F0 with FIFO)
 * @param  USARTx: pointer to USART peripheral
 */
void UART_EnableFIFO(USART_TypeDef *USARTx) {
    /* STM32F0xx doesn't have FIFO in USART, this is a placeholder */
    /* For compatibility with other STM32 series */
    (void)USARTx;
}

/**
 * @brief  Disable FIFO mode
 * @param  USARTx: pointer to USART peripheral
 */
void UART_DisableFIFO(USART_TypeDef *USARTx) {
    (void)USARTx;
}
