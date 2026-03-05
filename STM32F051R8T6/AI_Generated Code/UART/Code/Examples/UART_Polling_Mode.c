/**
 * @file    UART_Polling_Mode.c
 * @brief   UART Polling Mode Example for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-03-05
 * 
 * @note    This example demonstrates UART transmission and reception in polling mode
 *          No DMA or interrupts are used - all operations block until complete
 */

/*============================================================================
 * Includes
 *============================================================================*/
#include "uartF051.h"
#include "../../DeepSeek_Generated/RCC/rcc.h"
#include <stdio.h>

/*============================================================================
 * Macros
 *============================================================================*/
#define USART_BAUD_RATE    115200UL
#define TX_TIMEOUT         10000UL
#define RX_TIMEOUT         10000UL

/*============================================================================
 * Private Function Prototypes
 *============================================================================*/
static void SystemClock_Config(void);
static void UART_ConfigurePins(void);

/**
 * @brief  Main function - UART Polling Mode Example
 */
int main(void) {
    UART_InitTypeDef UART_InitStruct;
    uint8_t txData[] = "Hello from STM32F051!\r\n";
    uint8_t rxData[64];
    uint8_t rxIndex = 0;
    uint8_t ch;
    
    /* Configure System Clock */
    SystemClock_Config();
    
    /* Configure UART pins (already done in UART_Init, but shown for clarity) */
    UART_ConfigurePins();
    
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
    
    /* Initialize UART with configuration */
    UART_Init(USART1, &UART_InitStruct);
    
    /* Send string in polling mode */
    for (uint32_t i = 0; i < sizeof(txData) - 1; i++) {
        /* Wait for TXE (Transmit Data Register Empty) */
        if (UART_WaitForFlag(USART1, USART_ISR_TXE, true, TX_TIMEOUT)) {
            /* Send data */
            UART_SendData(USART1, (uint16_t)txData[i]);
        }
    }
    
    /* Wait for transmission to complete (TC flag) */
    UART_WaitForFlag(USART1, USART_ISR_TC, true, TX_TIMEOUT);
    
    /* Receive data in polling mode */
    rxIndex = 0;
    while (rxIndex < (sizeof(rxData) - 1)) {
        /* Wait for RXNE (Receive Data Register Not Empty) */
        if (UART_WaitForFlag(USART1, USART_ISR_RXNE, true, RX_TIMEOUT)) {
            /* Read received data */
            ch = (uint8_t)UART_ReceiveData(USART1);
            
            /* Store received data */
            rxData[rxIndex++] = ch;
            
            /* Echo back received data */
            UART_SendData(USART1, (uint16_t)ch);
            
            /* Wait for TXE to be set before sending next character */
            UART_WaitForFlag(USART1, USART_ISR_TXE, true, TX_TIMEOUT);
            
            /* Check for carriage return or newline - end of input */
            if (ch == '\r' || ch == '\n') {
                break;
            }
        } else {
            /* Timeout occurred */
            break;
        }
    }
    
    /* Null-terminate received string */
    rxData[rxIndex] = '\0';
    
    /* Wait for final transmission to complete */
    UART_WaitForFlag(USART1, USART_ISR_TC, true, TX_TIMEOUT);
    
    /* Infinite loop */
    while (1) {
        /* Toggle LED or do other work */
    }
}

/**
 * @brief  Configure system clock
 * @note   This function configures the system clock for UART operation
 */
static void SystemClock_Config(void) {
    /* Configure system clock for USART operation */
    /* This would typically use HSI (8MHz) or PLL to get higher speeds */
    
    /* Example: Enable HSI and use it as system clock */
    RCC_EnableHSI();
    RCC_SetSystemClockSource(CLOCK_SOURCE_HSI);
    
    /* System clock is now 8MHz */
    /* For 115200 baud with 16x oversampling: 8MHz / 16 = 500kHz / 115200 = ~4.3 */
    /* BRR = 0x2A9 for 115200 at 8MHz */
}

/**
 * @brief  Configure UART pins
 * @note   This function is typically called within UART_Init
 *         Shown here for documentation purposes
 */
static void UART_ConfigurePins(void) {
    /* UART pins are configured in UART_ConfigGPIO() within uartF051.c
     * For USART1:
     *   TX = PA9 (Alternate Function 1)
     *   RX = PA10 (Alternate Function 1)
     * 
     * For USART2:
     *   TX = PA2 (Alternate Function 1)
     *   RX = PA3 (Alternate Function 1)
     */
}

/*============================================================================
 * Alternative Polling Functions (Direct Register Access)
 *============================================================================*/

/**
 * @brief  Simple transmit string in polling mode
 */
void UART_TransmitString(USART_TypeDef *USARTx, uint8_t *str) {
    while (*str) {
        /* Wait for TXE flag */
        while (!UART_GetFlag(USARTx, USART_ISR_TXE));
        
        /* Send character */
        USARTx->TDR = *str++;
    }
    
    /* Wait for transmission complete */
    while (!UART_GetFlag(USARTx, USART_ISR_TC));
}

/**
 * @brief  Simple receive character in polling mode
 * @return Received character
 */
uint8_t UART_ReceiveChar(USART_TypeDef *USARTx) {
    /* Wait for RXNE flag */
    while (!UART_GetFlag(USARTx, USART_ISR_RXNE));
    
    /* Return received data */
    return (uint8_t)(USARTx->RDR & 0xFF);
}

/**
 * @brief  Receive string in polling mode (until newline or buffer full)
 * @param  buffer: pointer to receive buffer
 * @param  maxLen: maximum number of characters to receive
 * @return number of characters received
 */
uint32_t UART_ReceiveString(USART_TypeDef *USARTx, uint8_t *buffer, uint32_t maxLen) {
    uint32_t count = 0;
    uint8_t ch;
    
    while (count < (maxLen - 1)) {
        /* Wait for RXNE flag with timeout */
        if (!UART_WaitForFlag(USARTx, USART_ISR_RXNE, true, RX_TIMEOUT)) {
            break;
        }
        
        /* Read character */
        ch = (uint8_t)(USARTx->RDR & 0xFF);
        
        /* Check for newline */
        if (ch == '\r' || ch == '\n') {
            break;
        }
        
        /* Store character */
        buffer[count++] = ch;
    }
    
    /* Null-terminate */
    buffer[count] = '\0';
    
    return count;
}
