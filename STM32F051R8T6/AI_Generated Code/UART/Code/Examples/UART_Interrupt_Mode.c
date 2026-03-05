/**
 * @file    UART_Interrupt_Mode.c
 * @brief   UART Interrupt Mode Example for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-03-05
 * 
 * @note    This example demonstrates UART transmission and reception using interrupts
 *          Non-blocking communication with interrupt handlers
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
static void UART_EnableInterrupt(USART_TypeDef *USARTx);
static void UART_DisableInterrupt(USART_TypeDef *USARTx);

/**
 * @brief  Main function - UART Interrupt Mode Example
 */
int main(void) {
    UART_InitTypeDef UART_InitStruct;
    uint8_t txData[] = "UART Interrupt Mode Demo\r\n";
    uint32_t i;
    
    /* Configure System Clock */
    SystemClock_Config();
    
    /* Initialize UART Configuration */
    UART_StructInit(&UART_InitStruct);
    UART_InitStruct.BaudRate = USART_BAUD_RATE;
    UART_InitStruct.WordLength = UART_WORDLENGTH_8B;
    UART_InitStruct.StopBits = UART_STOPBITS_1;
    UART_InitStruct.Parity = UART_PARITY_NONE;
    UART_InitStruct.Mode = UART_MODE_TX_RX;
    UART_InitStruct.HwFlowCtl = UART_HWCONTROL_NONE;
    UART_InitStruct.OverSampling = UART_OVERSAMPLING_16;
    UART_InitStruct.OneBitSampling = false;
    
    /* Initialize UART */
    UART_Init(USART1, &UART_InitStruct);
    
    /* Enable UART interrupts */
    /* RXNE interrupt: Receive data register not empty */
    /* TXE interrupt: Transmit data register empty */
    /* PE interrupt: Parity error */
    /* Error interrupt: ORE, NE, FE */
    
    /* Enable RXNE, TXE, and PE interrupts via CR1 */
    USART1->CR1 |= (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE);
    
    /* Enable Error interrupts via CR3 */
    USART1->CR3 |= USART_CR3_EIE;  /* Error interrupt enable */
    
    /* Enable USART1 interrupt in NVIC */
    /* Note: USART1_IRQn = 27 for STM32F0 */
    /* This would typically be done via NVIC_EnableIRQ() or similar */
    /* For now, we assume the NVIC is configured elsewhere */
    
    /* Copy transmission data to buffer */
    for (i = 0; i < sizeof(txData) - 1; i++) {
        txBuffer[txHead] = txData[i];
        txHead = (txHead + 1) % TX_BUFFER_SIZE;
        txCount++;
    }
    
    /* Enable TXE interrupt to start transmission */
    /* First character will be sent by checking if TXE and sending from buffer */
    if (txCount > 0 && (USART1->ISR & USART_ISR_TXE)) {
        USART1->TDR = txBuffer[txTail];
        txTail = (txTail + 1) % TX_BUFFER_SIZE;
        txCount--;
    }
    
    /* Main loop */
    while (1) {
        /* Check if data received */
        if (rxCount > 0) {
            /* Process received data */
            uint8_t ch = rxBuffer[rxTail];
            rxTail = (rxTail + 1) % RX_BUFFER_SIZE;
            rxCount--;
            
            /* Echo back received character */
            while (((txHead + 1) % TX_BUFFER_SIZE) == txTail) {
                /* Buffer full - wait */
            }
            
            txBuffer[txHead] = ch;
            txHead = (txHead + 1) % TX_BUFFER_SIZE;
            txCount++;
            
            /* Enable TXE interrupt to transmit */
            USART1->CR1 |= USART_CR1_TXEIE;
        }
        
        /* Check for transmission complete */
        if (txComplete) {
            txComplete = false;
            /* Could signal another task here */
        }
        
        /* Check for error */
        if (errorOccurred) {
            errorOccurred = false;
            /* Handle error - could clear and continue */
            UART_ClearError(USART1);
        }
    }
}

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
            USART1->CR1 &= ~USART_CR1_TXEIE;
            
            /* Enable TC interrupt to know when transmission is truly complete */
            USART1->CR1 |= USART_CR1_TCIE;
        }
    }
    
    /* Check for Transmission Complete */
    if (isr & USART_ISR_TC) {
        /* Clear TC flag */
        UART_ClearInterruptFlag(USART1, USART_ICR_TCCF);
        
        /* Disable TC interrupt */
        USART1->CR1 &= ~USART_CR1_TCIE;
        
        /* Signal transmission complete */
        txComplete = true;
    }
}

/**
 * @brief  Configure system clock
 */
static void SystemClock_Config(void) {
    /* Enable HSI and use as system clock */
    RCC_EnableHSI();
    RCC_SetSystemClockSource(CLOCK_SOURCE_HSI);
}

/**
 * @brief  Enable UART interrupts (NVIC level)
 * @note   This is typically handled by NVIC_EnableIRQ in CMSIS
 */
static void UART_EnableInterrupt(USART_TypeDef *USARTx) {
    /* For STM32F051, USART1_IRQn = 27 */
    /* This would be: NVIC_EnableIRQ(USART1_IRQn); */
    /* Since we're not using CMSIS, this would be a direct register write */
    /* Example: */
    /* *(volatile uint32_t *)0xE000E100 = (1 << (USART1_IRQn % 32)); */
}

/**
 * @brief  Disable UART interrupts (NVIC level)
 */
static void UART_DisableInterrupt(USART_TypeDef *USARTx) {
    /* This would be: NVIC_DisableIRQ(USART1_IRQn); */
}

/*============================================================================
 * Alternative: Simple Interrupt-Driven TX/RX Functions
 *============================================================================*/

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
    USARTx->CR1 |= USART_CR1_TXEIE;
    
    return true;
}

/**
 * @brief  Receive data from interrupt buffer
 * @param  USARTx: UART peripheral
 * @param  data: pointer to store received data
 * @return true if data available, false if buffer empty
 */
bool UART_ReceiveDataIT(USART_TypeDef *USARTx, uint8_t *data) {
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
