/**
 * @file    UART_Interrupt_Mode.c
 * @brief   UART Interrupt Mode Example for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-03-06
 *
 * @note    This example demonstrates UART transmission and reception using interrupts
 *          Non-blocking communication with interrupt handlers using the UART API
 *          Uses Circular Buffer APIs for efficient data handling
 */

/*============================================================================
 * Includes
 *============================================================================*/
#include "uartF051.h"
#include "CirBuffer.h"
#include "rcc.h"
#include "crc.h"
#include "AT45DB.h"
#include "gpio.h"
#include "spiF051.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*============================================================================
 * Macros
 *============================================================================*/

/* SPI Configuration - using SPI1 */
#define AT45DB_SPI                SPI1

/* GPIO Configuration */
/* SCK: PA1 (check your hardware pinout)
   MISO: PA6
   MOSI: PA7
   CS: PA4 (GPIO output) */
#define AT45DB_GPIO_PORT          GPIOA
#define AT45DB_CS_PIN             GPIO_PIN_4
#define AT45DB_SCK_PIN            GPIO_PIN_1
#define AT45DB_MISO_PIN           GPIO_PIN_6
#define AT45DB_MOSI_PIN           GPIO_PIN_7

#define USART_BAUD_RATE    115200UL
#define TX_BUFFER_SIZE     264
#define RX_BUFFER_SIZE     264

#define Cmd_Read_Full	0x01u


#define TxRx_HEADER		0xA5

#define TxRx_CRC_POS			1	// 1 byte
#define TxRx_Len_POS			3	// 2 bytes
#define TxRx_CMD_POS			5	// 1 byte
#define TxRx_PA_POS				6	// 2 bytes
#define TxRx_Data_POS			8	// 264 bytes

#define LEN_DATA_TXRX			(272)
#define LEN_PAGE_SIZE			(264)
#define LEN_REPLY_CMD			(264 + 2 + 1)

#define DATA_HEADER 			0xA5

#define TXRX_ERROR_READ			0x70

enum
{
    FAIL,
    PASS
};

/*============================================================================
 * NVIC Definitions (STM32F051)
 *============================================================================*/
#define NVIC_BASE_ADDR     0xE000E100UL
//#define NVIC_ISER          (*(volatile uint32_t *)(NVIC_BASE_ADDR))
//#define NVIC_ICER          (*(volatile uint32_t *)(NVIC_BASE_ADDR + 0x80))
//#define NVIC_IPR           (*(volatile uint32_t *)(NVIC_BASE_ADDR + 0x300))

/*============================================================================
 * Private Variables
 *============================================================================*/
volatile bool txComplete = false;
volatile bool rxComplete = false;
volatile bool errorOccurred = false;

volatile uint8_t RxData[LEN_DATA_TXRX] = {0};
volatile uint8_t TxData[LEN_DATA_TXRX] = {0};
volatile uint8_t RxCound_Down = 0;

/* Circular Buffers for TX and RX */
static CircularBuffer *txBuffer = NULL;
static CircularBuffer *rxBuffer = NULL;

/* AT45DB Handle */
AT45DB_HandleTypeDef at45db_handle;
/*============================================================================
 * Private Function Prototypes
 *============================================================================*/
static void SystemClock_Config(void);
static void NVIC_EnableUARTInterrupt(uint8_t irq);
static void NVIC_DisableUARTInterrupt(uint8_t irq);
static void Buffer_Init(void);
static void Buffer_DeInit(void);

/*============================================================================
 * NVIC Helper Functions
 *============================================================================*/

/**
 * @brief  Enable UART interrupt in NVIC
 * @param  irq: IRQ number (UART1_IRQn = 27, UART2_IRQn = 28)
 */
static void NVIC_EnableUARTInterrupt(uint8_t irq)
{
    if (irq < 32)
    {
        NVIC_ISER = (1 << irq);
    }
}

/**
 * @brief  Disable UART interrupt in NVIC
 * @param  irq: IRQ number (UART1_IRQn = 27, UART2_IRQn = 28)
 */
static void NVIC_DisableUARTInterrupt(uint8_t irq)
{
    if (irq < 32)
    {
        NVIC_ICER = (1 << irq);
    }
}

/*============================================================================
 * Buffer Management Functions
 *============================================================================*/

/**
 * @brief  Initialize circular buffers for TX and RX
 */
static void Buffer_Init(void)
{
    /* Create TX circular buffer */
    txBuffer = circular_buffer_create(TX_BUFFER_SIZE);
    if (txBuffer == NULL)
    {
        /* Handle error - buffer creation failed */
        while (1);
    }

    /* Create RX circular buffer */
    rxBuffer = circular_buffer_create(RX_BUFFER_SIZE);
    if (rxBuffer == NULL)
    {
        /* Handle error - buffer creation failed */
        while (1);
    }
}

/**
 * @brief  De-initialize circular buffers
 */
static void Buffer_DeInit(void)
{
    if (txBuffer != NULL)
    {
        circular_buffer_destroy(txBuffer);
        txBuffer = NULL;
    }

    if (rxBuffer != NULL)
    {
        circular_buffer_destroy(rxBuffer);
        rxBuffer = NULL;
    }
}

/*============================================================================
 * Interrupt Service Functions
 *============================================================================*/

/**
 * @brief  UART1 Interrupt Handler
 * @note   Handles TXE, RXNE, PE, and error interrupts
 */
void USART1_IRQHandler(void)
{
    uint32_t isr = USART1->ISR;
    uint8_t data;

    /* Check for Parity Error */
    if (isr & USART_ISR_PE)
    {
        errorOccurred = true;
        UART_ClearInterruptFlag(USART1, USART_ICR_PECF);
    }

    /* Check for Frame Error, Noise, or Overrun */
    if (isr & (USART_ISR_FE | USART_ISR_NE | USART_ISR_ORE))
    {
        errorOccurred = true;
        /* Clear the error flags */
        UART_ClearInterruptFlag(USART1, USART_ICR_FECF | USART_ICR_NCF | USART_ICR_ORECF);
    }

    /* Check for RXNE - Data Received */
    if (isr & USART_ISR_RXNE)
    {
        RxCound_Down = 200;
        /* Read received data */
        data = (uint8_t)(USART1->RDR & 0xFF);

        /* Store in circular buffer using API */
        if (circular_buffer_write(rxBuffer, (char)data) == BUFFER_OK)
        {
            rxComplete = true;
        }
        else
        {
            /* Buffer overflow - error */
            errorOccurred = true;
        }
    }

    /* Check for TXE - Transmit Buffer Empty */
    if (isr & USART_ISR_TXE)
    {
        /* Check if there's data to send using API */
        if (!circular_buffer_is_empty(txBuffer))
        {
            /* Read next character from buffer */
            char txData;
            if (circular_buffer_read(txBuffer, &txData) == BUFFER_OK)
            {
                /* Send character */
                USART1->TDR = (uint8_t)txData;
            }
        }
        else
        {
            /* No more data - disable TXE interrupt to prevent continuous interrupts */
            UART_DisableInterrupt(USART1, USART_CR1_TXEIE);

            /* Enable TC interrupt to know when transmission is truly complete */
            UART_EnableInterrupt(USART1, USART_CR1_TCIE);
        }
    }

    /* Check for Transmission Complete */
    if (isr & USART_ISR_TC)
    {
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
static void SystemClock_Config(void)
{
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
bool UART_SendDataIT(USART_TypeDef *USARTx, uint8_t data)
{
    /* Check if buffer has space using API */
    if (circular_buffer_is_full(txBuffer))
    {
        return false;  /* Buffer full */
    }

    /* Write data to circular buffer using API */
    if (circular_buffer_write(txBuffer, (char)data) != BUFFER_OK)
    {
        return false;  /* Write failed */
    }

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
bool UART_ReceiveDataIT(USART_TypeDef *USARTx, volatile uint8_t *data)
{
    (void)USARTx;  /* Not needed for buffer access */

    /* Check if buffer has data using API */
    if (circular_buffer_is_empty(rxBuffer))
    {
        return false;  /* Buffer empty */
    }

    /* Read data from circular buffer using API */
    if (circular_buffer_read(rxBuffer, (char *)data) != BUFFER_OK)
    {
        return false;  /* Read failed */
    }

    return true;
}

/**
 * @brief  Get number of bytes available in RX buffer
 */
uint32_t UART_Available(void)
{
    return (uint32_t)circular_buffer_available(rxBuffer);
}

/**
 * @brief  Get number of bytes in TX buffer waiting to be sent
 */
uint32_t UART_TxWaiting(void)
{
    return (uint32_t)circular_buffer_available(txBuffer);
}

/**
 * @brief  Send string via interrupt
 * @param  USARTx: UART peripheral
 * @param  str: string to send
 * @return true if all data queued
 */
bool UART_SendStringIT(USART_TypeDef *USARTx, const char *str)
{
    while (*str)
    {
        if (!UART_SendDataIT(USARTx, (uint8_t)*str))
        {
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
void UART_InterruptInit(USART_TypeDef *USARTx, uint32_t baudRate)
{
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
void UART_EnableRxTxInterrupts(USART_TypeDef *USARTx)
{
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
void UART_DisableRxTxInterrupts(USART_TypeDef *USARTx)
{
    /* Disable all UART interrupts */
    UART_DisableInterrupt(USARTx, USART_CR1_RXNEIE | USART_CR1_TXEIE |
                          USART_CR1_TCIE | USART_CR1_PEIE | USART_CR1_IDLEIE);
    UART_DisableInterrupt(USARTx, USART_CR3_EIE | USART_CR3_CTSIE);
}

uint8_t identifyReceiveData(uint8_t *Str)
{
    bool retVal = FAIL;
    uint8_t dataval = 0;
    uint16_t length = 0;
    uint16_t crc16 = 0, crc16_2 = 0;

    if(Str == NULL)
        return FAIL;

    UART_ReceiveDataIT(USART1, &dataval);

    if(dataval != DATA_HEADER)
        return FAIL;

    UART_ReceiveDataIT(USART1, &dataval);
    crc16 = dataval;
    UART_ReceiveDataIT(USART1, &dataval);
    crc16 |= (dataval << 8);
    UART_ReceiveDataIT(USART1, &dataval);
    Str[0] = dataval;   // to calculate the CRC
    length = dataval;
    UART_ReceiveDataIT(USART1, &dataval);
    Str[1] = dataval;
    length |= (dataval << 8);

    if(length > 0)
    {
        for(uint16_t i = 0; i < length; i++)
        {
            UART_ReceiveDataIT(USART1, &Str[i+2]);
        }
        crc16_2 = CalculateCRC16(Str, length+2);
        if(crc16_2 == crc16)
        {
            //StrData[length] = 0;
            retVal = PASS;
        }
        else
        {
            retVal = FAIL;
        }
    }
    else
    {
        retVal = FAIL;
    }
    return retVal;
}

/**
 * @brief  Configure GPIO pins for SPI and CS
 */
static void AT45_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_EnableClock(GPIOA);

    /* Configure CS pin as output */
    GPIO_InitStruct.Pin = AT45DB_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_Init(AT45DB_GPIO_PORT, &GPIO_InitStruct);

    /* Set CS high (deselect) */
    GPIO_SetPin(AT45DB_GPIO_PORT, AT45DB_CS_PIN);

    /* Configure SCK pin (Alternate Function) */
    GPIO_InitStruct.Pin = AT45DB_SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_Init(AT45DB_GPIO_PORT, &GPIO_InitStruct);

    /* Configure MISO pin (Alternate Function) */
    GPIO_InitStruct.Pin = AT45DB_MISO_PIN;
    GPIO_Init(AT45DB_GPIO_PORT, &GPIO_InitStruct);

    /* Configure MOSI pin (Alternate Function) */
    GPIO_InitStruct.Pin = AT45DB_MOSI_PIN;
    GPIO_Init(AT45DB_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @brief  Configure SPI peripheral
 */
static void SPI_Config(void)
{
    SPI_InitTypeDef SPI_InitStruct;

    /* SPI configuration:
       - Mode: Master
       - Clock Polarity: Low (CPOL = 0)
       - Clock Phase: 1 Edge (CPHA = 1) - for AT45DB use mode 0 or 3
       - Baud Rate: fPCLK/16 (adjust for your needs)
       - Data Size: 8-bit
       - Frame Format: MSB first
    */

    /* Use Mode 0: CPOL=0, CPHA=0 (clock idle low, data captured on rising edge) */
    /* Or Mode 3: CPOL=1, CPHA=1 (clock idle high, data captured on falling edge) */
    /* AT45DB supports both Mode 0 and Mode 3 */

    SPI_InitStruct.Mode = SPI_MODE_MASTER;
    SPI_InitStruct.ClockPolarity = SPI_CPOL_LOW;   /* Clock idle low */
    SPI_InitStruct.ClockPhase = SPI_CPHA_1EDGE;    /* Data captured on first edge */
    SPI_InitStruct.BaudRate = SPI_BAUDRATEPRESCALER_16;  /* Adjust as needed */
    SPI_InitStruct.FrameFormat = SPI_FRAME_FORMAT_MSBFIRST;
    SPI_InitStruct.DataSize = SPI_DATASIZE_8BIT;
    SPI_InitStruct.NSS = SPI_NSS_SOFT;  /* Software NSS control */
    SPI_InitStruct.FIFOThreshold = true;

    /* Initialize SPI */
    SPI_Init(AT45DB_SPI, &SPI_InitStruct);

    /* Enable SPI */
    SPI_Enable(AT45DB_SPI);
}

bool AT45_mem_Init(void)
{
    AT45_GPIO_Config();

    SPI_Config();

    /* Initialize AT45DB handle */
    at45db_handle.SPIx = (void*)AT45DB_SPI;
    at45db_handle.CS_Port = (void*)AT45DB_GPIO_PORT;
    at45db_handle.CS_Pin = AT45DB_CS_PIN;

    /* Initialize AT45DB device */
    if (AT45DB_Init(&at45db_handle))
    {
        /* Device initialized successfully */
        return true;
    }
    else
    {
        /* Initialization failed */
        return false;
    }
}

uint16_t encodeData(volatile uint8_t *encodedData, uint16_t PageAdd)
{
    uint16_t length = 0;
    uint16_t crc16 = 0;
    bool success = false;

    if(encodedData == NULL)
        return 0;

    // Data formate == [Start Byte = 0xAA][CRC of (data)][Length of data][data]
    encodedData[0] = DATA_HEADER;

    encodedData[TxRx_Len_POS] = (uint8_t)(LEN_REPLY_CMD & 0x00FF);
    encodedData[TxRx_Len_POS + 1] = (uint8_t)((LEN_REPLY_CMD & 0xFF00) >> 8);
    length = length + 2;

    encodedData[TxRx_CMD_POS] = Cmd_Read_Full;
    length = length + 1;

    encodedData[TxRx_PA_POS] = (uint8_t)(PageAdd & 0x00FF);
    encodedData[TxRx_PA_POS + 1] = (uint8_t)((PageAdd & 0xFF00) >> 8);
    length = length + 2;

    success = AT45DB_PageRead(&at45db_handle,
                              (uint16_t)PageAdd,
                              (uint8_t *)&encodedData[TxRx_Data_POS]);
    length = length + 264;
    if(success == false)
    {
        return 0;
    }

    crc16 = CalculateCRC16((uint8_t *)&encodedData[TxRx_Len_POS], length);
    encodedData[TxRx_CRC_POS] = (uint8_t)(crc16 & 0x00FF);
    encodedData[TxRx_CRC_POS + 1] = (uint8_t)((crc16 & 0xFF00) >> 8);

    return LEN_DATA_TXRX;
}

uint16_t encodeErrro(volatile uint8_t *encodedData, uint8_t errorVal)
{
    uint16_t crc16 = 0;
    uint16_t length = 0;

    if(encodedData == NULL)
        return 0;

    // Data formate == [Start Byte = 0xAA][CRC of (data)][Length of data][data]
    encodedData[0] = DATA_HEADER;


    encodedData[TxRx_Len_POS] = (uint8_t)(0x0001 & 0x00FF);
    encodedData[TxRx_Len_POS + 1] = (uint8_t)((0x0001 & 0xFF00) >> 8);
    length = length + 2;

    encodedData[TxRx_CMD_POS] = errorVal;
    length = length + 1;

    crc16 = CalculateCRC16((uint8_t *)&encodedData[TxRx_Len_POS], length);
    encodedData[TxRx_CRC_POS] = (uint8_t)(crc16 & 0x00FF);
    encodedData[TxRx_CRC_POS + 1] = (uint8_t)((crc16 & 0xFF00) >> 8);

    return LEN_DATA_TXRX;
}

/**
 * @brief  Example: Read and Write operations
 */
static void AT45DB_Example_ReadWrite(void) {
    uint16_t page_num = 30;
    uint16_t i;
    bool success;


    /* Prepare test data */
    for (i = 0; i < at45db_handle.PageSize; i++) {
    			TxData[i] = (uint8_t)(i & 0xFF);
    }

    success = AT45DB_ProgramPage(&at45db_handle, page_num,
    		(uint8_t *)TxData, at45db_handle.PageSize, true);
    if (success) {
        printf("Write successful!\r\n");
    } else {
        printf("Write FAILED!\r\n");
        return;
    }

    /* Clear read buffer */
    for (i = 0; i < at45db_handle.PageSize; i++) {
    			TxData[i] = 0x00;
    }

    //--------------------------------------------------------------------------
//    page_num = 158;
//    /* Prepare test data */
//    for (i = 0; i < at45db_handle.PageSize; i++) {
//    			TxData[i] = (uint8_t)(0xFF);
//    }
//
//    success = AT45DB_ProgramPage(&at45db_handle, page_num,
//    			(uint8_t *)TxData, at45db_handle.PageSize, true);
//    if (success) {
//        printf("Write successful!\r\n");
//    } else {
//        printf("Write FAILED!\r\n");
//        return;
//    }
//
//    /* Clear read buffer */
//    for (i = 0; i < at45db_handle.PageSize; i++) {
//    			TxData[i] = 0x00;
//    }

}


/*============================================================================
 * Main Function
 *============================================================================*/

/**
 * @brief  Main function - UART Interrupt Mode Example
 */
int main(void)
{
    uint8_t txData[] = "UART Interrupt Mode Demo\r\n";
    uint16_t MemPagecnt = 0, len = 0;
    volatile uint8_t At45available = false, retryCnt = 0;;

    /* Initialize circular buffers */
    Buffer_Init();

    /* Configure System Clock */
    SystemClock_Config();

    /* Initialize UART for interrupt mode at 115200 baud */
    UART_InterruptInit(USART1, USART_BAUD_RATE);

    if(AT45_mem_Init())
    {
    	At45available = true;
/*
    	for(uint16_t pgnum = 0; pgnum < 2048; pgnum++)
    	{
    		int err = AT45DB_PageErase(&at45db_handle, pgnum);
    		if(err == 0)
    		{
    			At45available = 0;
    		}
    		else
    		{
    			At45available = 2;
    		}
    	}
    	*/
    	AT45DB_Example_ReadWrite();		// write sample data to memory

        // successfully configured AT45DB
    }
    else
    {
    	At45available = false;
        // Failed to configure AT45DB
    }

    /* Enable UART interrupts in peripheral */
    UART_EnableRxTxInterrupts(USART1);

    /* Enable USART1 interrupt in NVIC */
    NVIC_EnableUARTInterrupt(UART1_IRQno);

    /* Queue transmission data to buffer using batch write API */
    for (int i = 0; i < sizeof(txData) - 1; i++)
    {
        circular_buffer_write(txBuffer, (char)txData[i]);
    }

    /* Enable TXE interrupt to start transmission */
    /* First character will be sent from the interrupt handler */
    UART_EnableInterrupt(USART1, USART_CR1_TXEIE);

    /* Main loop */
    while (1)
    {
        /* Process received data */
        memset((uint8_t*)RxData, 0x00, sizeof(RxData));
        while (UART_Available() > 0)
        {
            RxCound_Down--;
            if(RxCound_Down == 0)
            {
                if(identifyReceiveData(RxData))
                {
                    if(RxData[2] == Cmd_Read_Full)
                    {
                        //UART_SendDataIT(USART1, i);

                        for(MemPagecnt = 0; MemPagecnt < 2048; MemPagecnt++)
                        {
                            memset(TxData, 0x00, sizeof(TxData));
                            if(len == 0)
                            {
                            	retryCnt++;
                            	if(retryCnt > 10)
                            	{
                                	encodeErrro(TxData, TXRX_ERROR_READ);
                                    for(len=0; len < 5; len++)
                                        UART_SendDataIT(USART1, TxData[len]);
                            		break;
                            	}
                            	if(MemPagecnt > 0)
                            		MemPagecnt--;
                            }
                            len = encodeData(TxData, MemPagecnt);
                            if(len)
                            {
                            	retryCnt = 0;
                                for(len=0; len < LEN_DATA_TXRX; len++)
                                    UART_SendDataIT(USART1, TxData[len]);

                                while (UART_Available() == 0);
                                UART_ReceiveDataIT(USART1, &RxData[0]);
                                while (UART_Available() == 0);
                                UART_ReceiveDataIT(USART1, &RxData[1]);
                                while (UART_Available() == 0);
                                UART_ReceiveDataIT(USART1, &RxData[2]);

                                if((RxData[0] != TxData[5]) || (RxData[1] != TxData[1]) || (RxData[2] != TxData[2]))
                                {
                                	encodeErrro(TxData, TXRX_ERROR_READ);
                                    for(len=0; len < 5; len++)
                                        UART_SendDataIT(USART1, TxData[len]);
                                    break;
                                }
                                for(int l; l < 4000; l++);
                            }
                        }
                    }
                }
            }
        }

        /* Check for transmission complete */
        if (txComplete)
        {
            txComplete = false;
            /* Transmission finished - could signal another task here */
        }

        /* Check for error */
        if (errorOccurred)
        {
            errorOccurred = false;
            /* Handle error - clear and continue */
            UART_ClearError(USART1);
        }
    }
    for(int l; l < 2000; l++);
    /* Cleanup buffers (never reached in infinite loop) */
    Buffer_DeInit();
}
