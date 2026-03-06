/**
 * @file    UART_Interrupt_Mode.c
 * @brief   UART Interrupt Mode Example for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-03-06
 * 
 * @note    This example demonstrates UART transmission and reception using interrupts
 *          Non-blocking communication with interrupt handlers using the UART API
 */

/*============================================================================
 * Includes
 *============================================================================*/
#include "uartF051.h"
#include "../../DeepSeek_Generated/RCC/rcc.h"
#include <stdint.h>
#include <stdbool.h>

/*============================================================================
 * Macros
 *============================================================================*/
#define USART_BAUD_RATE    115200UL
#define TX_BUFFER_SIZE     256
#define RX_BUFFER_SIZE     256

/*============================================================================
 * NVIC Definitions (STM32F051)
 *============================================================================*/
#define NVIC_BASE_ADDR     0xE000E100UL
#define NVIC_ISER          (*(volatile uint32_t *)(NVIC_BASE_ADDR))
#define NVIC_ICER          (*(volatile uint32_t *)(NVIC_BASE_ADDR + 0x80))
#define NVIC_IPR           (*(volatile uint32_t *)(NVIC_BASE_ADDR + 0x300))

/*============================================================================
 * Private Variables
 *============================================================================*/
volatile bool txComplete = false;
volatile bool rxComplete = false;
volatile bool errorOccurred = false;

/* TX Buffer */
static uint8_t txBuffer[TX_BUFFER_SIZE];
static volatile uint32_t txHead = 0;
static volatile uint32_t txTail = 0;
static volatile uint32_t txCount = 0;

/* RX Buffer */
static uint8_t rxBuffer[RX_BUFFER_SIZE];
static volatile uint32_t rxHead = 0;
static volatile uint32_t rxTail = 0;
static volatile uint32_t rxCount = 0;

/*============================================================================
 * Private Function Prototypes
 *============================================================================*/
static void SystemClock_Config(void);
static void NVIC_EnableUARTInterrupt(uint8_t irq);
static void NVIC_DisableUARTInterrupt(uint8_t irq);

/*============================================================================
 * NVIC Helper Functions
 *============================================================================*/

/**
 * @brief  Enable UART interrupt in NVIC
 * @param  irq: IRQ number (UART1_IRQn = 27, UART2_IRQn = 28)
 */
static void NVIC_EnableUARTInterrupt(uint8_t irq) {
    if (irq < 32) {
        NVIC_ISER = (1 << irq);
    }
}

/**
 * @brief  Disable UART interrupt in NVIC
 * @param  irq: IRQ number (UART1_IRQn = 27, UART2_IRQn = 28)
 */
static void NVIC_DisableUARTInterrupt(uint8_t irq) {
    if (irq < 32) {
        NVIC_ICER = (1 << irq);
    }
}

/*============================================================================
 * Interrupt Service Functions
 *============================================================================*/

/**
 * @brief  UART1 Interrupt Handler
 * @note   Handles TXE, RXNE, PE, and error interrupts
 */
void USART1_IRQHandler(void) {
    uint32_t isr = USART1->ISR;
    uint8_t data;
    
    /* Check for Parity Error */
    if (isr & USART_ISR_PE) {
        errorOccurred = true;
        UART_ClearInterruptFlag(USART1, USART_ICR_PECF);
    }
    
    /* Check for Frame Error, Noise, or Overrun */
    if (isr & (USART_ISR_FE | USART_ISR_NE | USART_ISR_ORE)) {
        errorOccurred = true;
        /* Clear the error flags */
        UART_ClearInterruptFlag(USART1, USART_ICR_FECF | USART_ICR_NCF | USART_ICR_ORECF);
    }
    
    /* Check for RXNE - Data Received */
    if (isr & USART_ISR_RXNE) {
        /* Read received data */
        data = (uint8_t)(USART1->RDR & 0xFF);
        
        /* Store in buffer if space available */
        if (rxCount < RX_BUFFER_SIZE) {
            rxBuffer[rxHead] = data;
            rxHead = (rxHead + 1) % RX_BUFFER_SIZE;
            rxCount++;
            rxComplete = true;
        } else {
            /* Buffer overflow - error */
            errorOccurred = true;
        }
    }
    
    /* Check for TXE - Transmit Buffer Empty */
    if (isr & USART_ISR_TXE) {
        /* Check if there's data to send */
        if (txCount > 0) {
            /* Send next character */
            USART1->TDR = txBuffer[txTail];
            txTail = (txTail + 1) % TX_BUFFER_SIZE;
            txCount--;
        } else {
            /* No more data - disable TXE interrupt to prevent continuous interrupts */
            UART_DisableInterrupt(USART1, USART_CR1_TXEIE);
            
            /* Enable TC interrupt to know when transmission is truly complete */
            UART_EnableInterrupt(USART1, USART_CR1_TCIE);
        }
    }
    
    /* Check for Transmission Complete */
    if (isr & USART_ISR_TC) {
        /* Clear TC flag */
        UART_ClearInterruptFlag(USART1, USART_ICR_TCCF);
        
        /* Disable TC interrupt */
        UART_DisableInterrupt(USART1, USART_CR1_TCIE);
        
        /* Signal transmission complete */
        txComplete = true;
    }
}

/*============================================================================
 * Public API Functions (using UART Library)
 *============================================================================*/

/**
 * @brief  Configure system clock
 */
static void SystemClock_Config(void) {
    /* Enable HSI and use as system clock */
    RCC_EnableHSI();
    RCC_SetSystemClockSource(CLOCK_SOURCE_HSI);
}

/**
 * @brief  Send data via interrupt (non-blocking)
 * @param  USARTx: UART peripheral
 * @param  data: data to send
 * @return true if data queued, false if buffer full
 */
bool UART_SendDataIT(USART_TypeDef *USARTx, uint8_t data) {
    if (txCount >= TX_BUFFER_SIZE) {
        return false;  /* Buffer full */
    }
    
    txBuffer[txHead] = data;
    txHead = (txHead + 1) % TX_BUFFER_SIZE;
    txCount++;
    
    /* Enable TXE interrupt to start transmission */
    UART_EnableInterrupt(USARTx, USART_CR1_TXEIE);
    
    return true;
}

/**
 * @brief  Receive data from interrupt buffer
 * @param  USARTx: UART peripheral
 * @param  data: pointer to store received data
 * @return true if data available, false if buffer empty
 */
bool UART_ReceiveDataIT(USART_TypeDef *USARTx, uint8_t *data) {
    (void)USARTx;  /* Not needed for buffer access */

    if (rxCount == 0) {
        return false;  /* Buffer empty */
    }
    
    *data = rxBuffer[rxTail];
    rxTail = (rxTail + 1) % RX_BUFFER_SIZE;
    rxCount--;
    
    return true;
}

/**
 * @brief  Get number of bytes available in RX buffer
 */
uint32_t UART_Available(void) {
    return rxCount;
}

/**
 * @brief  Get number of bytes in TX buffer waiting to be sent
 */
uint32_t UART_TxWaiting(void) {
    return txCount;
}

/**
 * @brief  Send string via interrupt
 * @param  USARTx: UART peripheral
 * @param  str: string to send
 * @return true if all data queued
 */
bool UART_SendStringIT(USART_TypeDef *USARTx, const char *str) {
    while (*str) {
        if (!UART_SendDataIT(USARTx, (uint8_t)*str)) {
            return false;  /* Buffer full */
        }
        str++;
    }
    return true;
}

/**
 * @brief  Initialize UART for interrupt mode
 * @param  USARTx: UART peripheral
 * @param  baudRate: desired baud rate
 */
void UART_InterruptInit(USART_TypeDef *USARTx, uint32_t baudRate) {
    UART_InitTypeDef UART_InitStruct;

    /* Initialize UART configuration to defaults */
    UART_StructInit(&UART_InitStruct);

    /* Configure for interrupt mode */
    UART_InitStruct.BaudRate = baudRate;
    UART_InitStruct.WordLength = UART_WORDLENGTH_8B;
    UART_InitStruct.StopBits = UART_STOPBITS_1;
    UART_InitStruct.Parity = UART_PARITY_NONE;
    UART_InitStruct.Mode = UART_MODE_TX_RX;
    UART_InitStruct.HwFlowCtl = UART_HWCONTROL_NONE;
    UART_InitStruct.OverSampling = UART_OVERSAMPLING_16;
    UART_InitStruct.OneBitSampling = false;

    /* Initialize UART peripheral */
    UART_Init(USARTx, &UART_InitStruct);
}

/**
 * @brief  Enable UART interrupts (RX, TX, PE, Error)
 * @param  USARTx: UART peripheral
 */
void UART_EnableRxTxInterrupts(USART_TypeDef *USARTx) {
    /* Enable RXNE interrupt for receive */
    UART_EnableInterrupt(USARTx, USART_CR1_RXNEIE);

    /* Enable PE interrupt for parity error */
    UART_EnableInterrupt(USARTx, USART_CR1_PEIE);

    /* Enable Error interrupts (ORE, NE, FE) */
    UART_EnableInterrupt(USARTx, USART_CR3_EIE);
}

/**
 * @brief  Disable UART interrupts
 * @param  USARTx: UART peripheral
 */
void UART_DisableRxTxInterrupts(USART_TypeDef *USARTx) {
    /* Disable all UART interrupts */
    UART_DisableInterrupt(USARTx, USART_CR1_RXNEIE | USART_CR1_TXEIE |
                                USART_CR1_TCIE | USART_CR1_PEIE | USART_CR1_IDLEIE);
    UART_DisableInterrupt(USARTx, USART_CR3_EIE | USART_CR3_CTSIE);
}

/*============================================================================
 * Main Function
 *============================================================================*/

/**
 * @brief  Main function - UART Interrupt Mode Example
 */
int main(void) {
    uint8_t txData[] = "UART Interrupt Mode Demo\r\n";
    uint32_t i;

    /* Configure System Clock */
    SystemClock_Config();

    /* Initialize UART for interrupt mode at 115200 baud */
    UART_InterruptInit(USART1, USART_BAUD_RATE);

    /* Enable UART interrupts in peripheral */
    UART_EnableRxTxInterrupts(USART1);

    /* Enable USART1 interrupt in NVIC */
    NVIC_EnableUARTInterrupt(UART1_IRQno);

    /* Copy transmission data to buffer */
    for (i = 0; i < sizeof(txData) - 1; i++) {
        txBuffer[txHead] = txData[i];
        txHead = (txHead + 1) % TX_BUFFER_SIZE;
        txCount++;
    }

    /* Enable TXE interrupt to start transmission */
    /* First character will be sent from the interrupt handler */
    UART_EnableInterrupt(USART1, USART_CR1_TXEIE);

    /* Main loop */
    while (1) {
        /* Process received data */
        while (UART_Available() > 0) {
            uint8_t ch;

            /* Get received character */
            if (UART_ReceiveDataIT(USART1, &ch)) {
                /* Echo back received character */
                UART_SendDataIT(USART1, ch);
            }
        }

        /* Check for transmission complete */
        if (txComplete) {
            txComplete = false;
            /* Transmission finished - could signal another task here */
        }

        /* Check for error */
        if (errorOccurred) {
            errorOccurred = false;
            /* Handle error - clear and continue */
            UART_ClearError(USART1);
        }
    }
}
