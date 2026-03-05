/**
 * @file    UART_DMA_Mode.c
 * @brief   UART DMA Mode Example for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-03-05
 * 
 * @note    This example demonstrates UART transmission and reception using DMA
 *          Non-blocking communication with DMA controllers
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
 * DMA Register Definitions (for STM32F051)
 *============================================================================*/
#define DMA1_BASE           (0x40020000UL)
#define DMA_ISR             (*(volatile uint32_t *)(DMA1_BASE + 0x00))
#define DMA_IFCR            (*(volatile uint32_t *)(DMA1_BASE + 0x04))

/* DMA Channel Registers */
#define DMA_CCR(ch)        (*(volatile uint32_t *)(DMA1_BASE + 0x08 + ((ch-1) * 0x0C)))
#define DMA_CNDTR(ch)      (*(volatile uint32_t *)(DMA1_BASE + 0x0C + ((ch-1) * 0x0C)))
#define DMA_CPAR(ch)       (*(volatile uint32_t *)(DMA1_BASE + 0x10 + ((ch-1) * 0x0C)))
#define DMA_CMAR(ch)       (*(volatile uint32_t *)(DMA1_BASE + 0x14 + ((ch-1) * 0x0C)))

/* DMA Channel 1 (USART1_TX) */
#define DMA1_CCR1          DMA_CCR(1)
#define DMA1_CNDTR1        DMA_CNDTR(1)
#define DMA1_CPAR1         DMA_CPAR(1)
#define DMA1_CMAR1         DMA_CMAR(1)

/* DMA Channel 2 (USART1_RX) */
#define DMA1_CCR2          DMA_CCR(2)
#define DMA1_CNDTR2        DMA_CNDTR(2)
#define DMA1_CPAR2         DMA_CPAR(2)
#define DMA1_CMAR2         DMA_CMAR(2)

/* DMA Channel 3 (USART2_TX) */
#define DMA1_CCR3          DMA_CCR(3)
#define DMA1_CNDTR3        DMA_CNDTR(3)
#define DMA1_CPAR3         DMA_CPAR(3)
#define DMA1_CMAR3         DMA_CMAR(3)

/* DMA Channel 4 (USART2_RX) */
#define DMA1_CCR4          DMA_CCR(4)
#define DMA1_CNDTR4        DMA_CNDTR(4)
#define DMA1_CPAR4         DMA_CPAR(4)
#define DMA1_CMAR4         DMA_CMAR(4)

/* DMA ISR/IFCR Bits */
#define DMA_ISR_TCIF1       (0x00000002UL)
#define DMA_ISR_HTIF1       (0x00000001UL)
#define DMA_ISR_TCIF2       (0x00000020UL)
#define DMA_ISR_HTIF2       (0x00000010UL)
#define DMA_ISR_TCIF3       (0x00000800UL)
#define DMA_ISR_HTIF3       (0x00000400UL)
#define DMA_ISR_TCIF4       (0x00020000UL)
#define DMA_ISR_HTIF4       (0x00010000UL)

#define DMA_IFCR_CTCIF1      (0x00000002UL)
#define DMA_IFCR_CHTIF1      (0x00000001UL)
#define DMA_IFCR_CTCIF2      (0x00000020UL)
#define DMA_IFCR_CHTIF2      (0x00000010UL)
#define DMA_IFCR_CTCIF3      (0x00000800UL)
#define DMA_IFCR_CHTIF3      (0x00000400UL)
#define DMA_IFCR_CTCIF4      (0x00020000UL)
#define DMA_IFCR_CHTIF4      (0x00010000UL)

/* DMA CCR Bits */
#define DMA_CCR_EN          (0x00000001UL)
#define DMA_CCR_TCIE        (0x00000002UL)
#define DMA_CCR_HTIE        (0x00000004UL)
#define DMA_CCR_TEIE        (0x00000008UL)
#define DMA_CCR_DIR         (0x00000010UL)  /* 0=Read from peripheral, 1=Read from memory */
#define DMA_CCR_CIRC        (0x00000020UL)
#define DMA_CCR_PINC        (0x00000040UL)
#define DMA_CCR_MINC        (0x00000080UL)
#define DMA_CCR_PSIZE       (0x00000100UL)  /* Peripheral size: 0=8-bit, 1=16-bit, 2=32-bit */
#define DMA_CCR_MSIZE       (0x00000200UL)  /* Memory size: 0=8-bit, 1=16-bit, 2=32-bit */
#define DMA_CCR_PSIZE_8BIT  (0x00000000UL)
#define DMA_CCR_MSIZE_8BIT  (0x00000000UL)
#define DMA_CCR_PSIZE_16BIT (0x00000100UL)
#define DMA_CCR_MSIZE_16BIT (0x00000200UL)
#define DMA_CCR_PL          (0x00000C00UL)  /* Priority: 0=Low, 1=Medium, 2=High, 3=Very High */

/*============================================================================
 * Private Variables
 *============================================================================*/
static uint8_t txBuffer[TX_BUFFER_SIZE];
static uint8_t rxBuffer[RX_BUFFER_SIZE];
static volatile bool txComplete = false;
static volatile bool rxComplete = false;
static volatile bool txError = false;
static volatile bool rxError = false;

/*============================================================================
 * Private Function Prototypes
 *============================================================================*/
static void SystemClock_Config(void);
static void DMA_ConfigTx(uint8_t channel, uint32_t periphAddr, uint32_t memAddr, uint16_t size);
static void DMA_ConfigRx(uint8_t channel, uint32_t periphAddr, uint32_t memAddr, uint16_t size);
static void DMA_EnableChannel(uint8_t channel);
static void DMA_DisableChannel(uint8_t channel);
static void DMA_ClearFlags(uint8_t channel);

/**
 * @brief  Main function - UART DMA Mode Example
 */
int main(void) {
    UART_InitTypeDef UART_InitStruct;
    uint8_t txData[] = "UART DMA Mode Demo\r\n";
    uint32_t i;
    
    /* Configure System Clock */
    SystemClock_Config();
    
    /* Enable DMA1 Clock */
    /* Enable DMA using direct register access */
    *(volatile uint32_t *)(0x40021014UL) |= 0x00000100UL;
    
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
    
    /* Configure DMA for TX (Channel 1) */
    /* USART1 TX: DMA Channel 1 */
    DMA_ConfigTx(1, (uint32_t)(&USART1->TDR), (uint32_t)txData, (uint16_t)(sizeof(txData) - 1));
    
    /* Enable DMA for TX in USART */
    UART_EnableDMA(USART1, UART_DMA_REQ_TX);
    
    /* Enable DMA TX Channel */
    DMA_EnableChannel(1);
    
    /* Wait for TX completion (polling) */
    while (!txComplete) {
        /* Check for TX error */
        if (txError) {
            txError = false;
            /* Handle error */
            break;
        }
    }
    
    /* Clear TX complete flag */
    txComplete = false;
    
    /* Disable DMA TX */
    DMA_DisableChannel(1);
    UART_DisableDMA(USART1, UART_DMA_REQ_TX);
    
    /* Now demonstrate RX with DMA */
    /* Configure DMA for RX (Channel 2) */
    DMA_ConfigRx(2, (uint32_t)(&USART1->RDR), (uint32_t)rxBuffer, (uint16_t)(RX_BUFFER_SIZE - 1));
    
    /* Enable DMA for RX in USART */
    UART_EnableDMA(USART1, UART_DMA_REQ_RX);
    
    /* Enable DMA RX Channel */
    DMA_EnableChannel(2);
    
    /* Wait for RX completion (polling) - wait for half-transfer or full-transfer */
    while (!rxComplete) {
        /* Check for RX error */
        if (rxError) {
            rxError = false;
            /* Handle error */
            break;
        }
    }
    
    /* Clear RX complete flag */
    rxComplete = false;
    
    /* Disable DMA RX */
    DMA_DisableChannel(2);
    UART_DisableDMA(USART1, UART_DMA_REQ_RX);
    
    /* Echo received data via DMA TX */
    if (rxComplete || rxBuffer[0] != 0) {
        /* Configure DMA for TX of received data */
        DMA_ConfigTx(1, (uint32_t)(&USART1->TDR), (uint32_t)rxBuffer, (uint16_t)(RX_BUFFER_SIZE - 1));
        
        /* Enable DMA for TX */
        UART_EnableDMA(USART1, UART_DMA_REQ_TX);
        
        /* Enable DMA TX Channel */
        DMA_EnableChannel(1);
        
        /* Wait for TX completion */
        while (!txComplete) {
            if (txError) {
                txError = false;
                break;
            }
        }
        
        txComplete = false;
        
        /* Disable DMA TX */
        DMA_DisableChannel(1);
        UART_DisableDMA(USART1, UART_DMA_REQ_TX);
    }
    
    /* Infinite loop */
    while (1) {
        /* Could add more communication here */
    }
}

/**
 * @brief  Configure DMA for TX (Memory to Peripheral)
 * @param  channel: DMA channel number (1-7)
 * @param  periphAddr: Peripheral address (USART TDR)
 * @param  memAddr: Memory address (TX buffer)
 * @param  size: Number of bytes to transfer
 */
static void DMA_ConfigTx(uint8_t channel, uint32_t periphAddr, uint32_t memAddr, uint16_t size) {
    /* Disable channel first */
    DMA_DisableChannel(channel);
    
    /* Clear flags */
    DMA_ClearFlags(channel);
    
    /* Configure DMA channel */
    /* Memory increment, Direction: Memory to Peripheral, 8-bit size */
    DMA_CCR(channel) = DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE | DMA_CCR_TEIE;
    
    /* Set peripheral address */
    DMA_CPAR(channel) = periphAddr;
    
    /* Set memory address */
    DMA_CMAR(channel) = memAddr;
    
    /* Set number of data to transfer */
    DMA_CNDTR(channel) = size;
}

/**
 * @brief  Configure DMA for RX (Peripheral to Memory)
 * @param  channel: DMA channel number (1-7)
 * @param  periphAddr: Peripheral address (USART RDR)
 * @param  memAddr: Memory address (RX buffer)
 * @param  size: Number of bytes to transfer
 */
static void DMA_ConfigRx(uint8_t channel, uint32_t periphAddr, uint32_t memAddr, uint16_t size) {
    /* Disable channel first */
    DMA_DisableChannel(channel);
    
    /* Clear flags */
    DMA_ClearFlags(channel);
    
    /* Configure DMA channel */
    /* Memory increment, Direction: Peripheral to Memory, 8-bit size */
    DMA_CCR(channel) = DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_TEIE;
    
    /* Set peripheral address */
    DMA_CPAR(channel) = periphAddr;
    
    /* Set memory address */
    DMA_CMAR(channel) = memAddr;
    
    /* Set number of data to transfer */
    DMA_CNDTR(channel) = size;
}

/**
 * @brief  Enable DMA channel
 * @param  channel: DMA channel number
 */
static void DMA_EnableChannel(uint8_t channel) {
    DMA_CCR(channel) |= DMA_CCR_EN;
}

/**
 * @brief  Disable DMA channel
 * @param  channel: DMA channel number
 */
static void DMA_DisableChannel(uint8_t channel) {
    DMA_CCR(channel) &= ~DMA_CCR_EN;
}

/**
 * @brief  Clear DMA flags for channel
 * @param  channel: DMA channel number
 */
static void DMA_ClearFlags(uint8_t channel) {
    switch(channel) {
        case 1:
            DMA_IFCR = DMA_IFCR_CTCIF1 | DMA_IFCR_CHTIF1;
            break;
        case 2:
            DMA_IFCR = DMA_IFCR_CTCIF2 | DMA_IFCR_CHTIF2;
            break;
        case 3:
            DMA_IFCR = DMA_IFCR_CTCIF3 | DMA_IFCR_CHTIF3;
            break;
        case 4:
            DMA_IFCR = DMA_IFCR_CTCIF4 | DMA_IFCR_CHTIF4;
            break;
        default:
            break;
    }
}

/**
 * @brief  Check DMA transfer complete flag
 * @param  channel: DMA channel number
 * @return true if transfer complete
 */
static bool DMA_IsTransferComplete(uint8_t channel) {
    uint32_t flag = 0;
    
    switch(channel) {
        case 1:
            flag = DMA_ISR_TCIF1;
            break;
        case 2:
            flag = DMA_ISR_TCIF2;
            break;
        case 3:
            flag = DMA_ISR_TCIF3;
            break;
        case 4:
            flag = DMA_ISR_TCIF4;
            break;
        default:
            return false;
    }
    
    return ((DMA_ISR & flag) != 0);
}

/**
 * @brief  Configure system clock
 */
static void SystemClock_Config(void) {
    /* Enable HSI and use as system clock */
    RCC_EnableHSI();
    RCC_SetSystemClockSource(CLOCK_SOURCE_HSI);
}

/*============================================================================
 * DMA Interrupt Handlers (to be placed in startup file or interrupt vector)
 *============================================================================*/

/**
 * @brief  DMA1 Channel 1 Interrupt Handler (USART1 TX)
 */
void DMA1_Channel1_IRQHandler(void) {
    /* Check transfer complete */
    if (DMA_ISR & DMA_ISR_TCIF1) {
        /* Clear flag */
        DMA_IFCR = DMA_IFCR_CTCIF1;
        
        /* Signal completion */
        txComplete = true;
    }
    
    /* Check half transfer */
    if (DMA_ISR & DMA_ISR_HTIF1) {
        DMA_IFCR = DMA_IFCR_CHTIF1;
        /* Could handle half-transfer here */
    }
}

/**
 * @brief  DMA1 Channel 2 Interrupt Handler (USART1 RX)
 */
void DMA1_Channel2_3_IRQHandler(void) {
    /* Check transfer complete for channel 2 */
    if (DMA_ISR & DMA_ISR_TCIF2) {
        /* Clear flag */
        DMA_IFCR = DMA_IFCR_CTCIF2;
        
        /* Signal completion */
        rxComplete = true;
    }
    
    /* Check half transfer */
    if (DMA_ISR & DMA_ISR_HTIF2) {
        DMA_IFCR = DMA_IFCR_CHTIF2;
        /* Could handle half-transfer here */
    }
}

/**
 * @brief  DMA1 Channel 3 Interrupt Handler (USART2 TX)
 */
void DMA1_Channel4_5_IRQHandler(void) {
    /* Check transfer complete for channel 3 */
    if (DMA_ISR & DMA_ISR_TCIF3) {
        DMA_IFCR = DMA_IFCR_CTCIF3;
        txComplete = true;
    }
    
    if (DMA_ISR & DMA_ISR_HTIF3) {
        DMA_IFCR = DMA_IFCR_CHTIF3;
    }
}

/**
 * @brief  DMA1 Channel 4 Interrupt Handler (USART2 RX)
 */
void DMA1_Channel6_7_IRQHandler(void) {
    /* Check transfer complete for channel 4 */
    if (DMA_ISR & DMA_ISR_TCIF4) {
        DMA_IFCR = DMA_IFCR_CTCIF4;
        rxComplete = true;
    }
    
    if (DMA_ISR & DMA_ISR_HTIF4) {
        DMA_IFCR = DMA_IFCR_CHTIF4;
    }
}

/*============================================================================
 * High-Level DMA UART Functions
 *============================================================================*/

/**
 * @brief  Send data via DMA (non-blocking)
 * @param  USARTx: UART peripheral
 * @param  data: pointer to data buffer
 * @param  length: number of bytes to send
 * @return true if DMA started successfully
 */
bool UART_SendDataDMA(USART_TypeDef *USARTx, uint8_t *data, uint16_t length) {
    uint8_t channel;
    
    /* Determine DMA channel based on USART */
    if (USARTx == USART1) {
        channel = 1;  /* USART1 TX uses DMA Channel 1 */
    } else if (USARTx == USART2) {
        channel = 3;  /* USART2 TX uses DMA Channel 3 */
    } else {
        return false;
    }
    
    /* Configure DMA for TX */
    DMA_ConfigTx(channel, (uint32_t)(&USARTx->TDR), (uint32_t)data, length);
    
    /* Enable DMA in USART */
    UART_EnableDMA(USARTx, UART_DMA_REQ_TX);
    
    /* Enable DMA channel */
    DMA_EnableChannel(channel);
    
    return true;
}

/**
 * @brief  Receive data via DMA (non-blocking)
 * @param  USARTx: UART peripheral
 * @param  data: pointer to data buffer
 * @param  length: maximum bytes to receive
 * @return true if DMA started successfully
 */
bool UART_ReceiveDataDMA(USART_TypeDef *USARTx, uint8_t *data, uint16_t length) {
    uint8_t channel;
    
    /* Determine DMA channel based on USART */
    if (USARTx == USART1) {
        channel = 2;  /* USART1 RX uses DMA Channel 2 */
    } else if (USARTx == USART2) {
        channel = 4;  /* USART2 RX uses DMA Channel 4 */
    } else {
        return false;
    }
    
    /* Configure DMA for RX */
    DMA_ConfigRx(channel, (uint32_t)(&USARTx->RDR), (uint32_t)data, length);
    
    /* Enable DMA in USART */
    UART_EnableDMA(USARTx, UART_DMA_REQ_RX);
    
    /* Enable DMA channel */
    DMA_EnableChannel(channel);
    
    return true;
}

/**
 * @brief  Check if DMA TX is complete
 * @param  USARTx: UART peripheral
 * @return true if DMA transfer complete
 */
bool UART_IsDMATxComplete(USART_TypeDef *USARTx) {
    (void)USARTx;
    return txComplete;
}

/**
 * @brief  Check if DMA RX is complete
 * @param  USARTx: UART peripheral
 * @return true if DMA transfer complete
 */
bool UART_IsDMARxComplete(USART_TypeDef *USARTx) {
    (void)USARTx;
    return rxComplete;
}
