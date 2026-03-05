/**
 * @file    uartF051.h
 * @brief   UART/USART Driver for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-03-05
 */

#ifndef UARTF051_H
#define UARTF051_H

#include <stdint.h>
#include <stdbool.h>

/*============================================================================
 * UART Base Addresses
 *============================================================================*/
#define USART1_BASE    (0x40013800UL)  /*!< USART1 base address (APB2) */
#define USART2_BASE    (0x40004400UL)  /*!< USART2 base address (APB1) */

/*============================================================================
 * UART Register Structure
 *============================================================================*/
typedef struct {
    volatile uint32_t CR1;       /*!< Control Register 1,         Address offset: 0x00 */
    volatile uint32_t CR2;       /*!< Control Register 2,         Address offset: 0x04 */
    volatile uint32_t CR3;       /*!< Control Register 3,         Address offset: 0x08 */
    volatile uint32_t BRR;       /*!< Baud Rate Register,        Address offset: 0x0C */
    volatile uint32_t GTPR;      /*!< Guard Time Register,       Address offset: 0x10 */
    volatile uint32_t RTOR;      /*!< Receiver Timeout Register, Address offset: 0x14 */
    volatile uint32_t RQR;       /*!< Request Register,           Address offset: 0x18 */
    volatile uint32_t ISR;        /*!< Interrupt & Status Reg,    Address offset: 0x1C */
    volatile uint32_t ICR;        /*!< Interrupt Flag Clear Reg,  Address offset: 0x20 */
    volatile uint32_t RDR;        /*!< Receive Data Register,     Address offset: 0x24 */
    volatile uint32_t TDR;        /*!< Transmit Data Register,    Address offset: 0x28 */
} USART_TypeDef;

/* USART Base Address Macros */
#define USART1    ((USART_TypeDef *)USART1_BASE)
#define USART2    ((USART_TypeDef *)USART2_BASE)

/*============================================================================
 * UART Register Bit Definitions
 *============================================================================*/

/* CR1 Register Bits */
#define USART_CR1_UE         (0x00000001UL)  /*!< USART Enable */
#define USART_CR1_UESM       (0x00000002UL)  /*!< USART Enable in Stop Mode */
#define USART_CR1_RE         (0x00000004UL)  /*!< Receiver Enable */
#define USART_CR1_TE         (0x00000008UL)  /*!< Transmitter Enable */
#define USART_CR1_IDLEIE     (0x00000010UL)  /*!< IDLE Interrupt Enable */
#define USART_CR1_RXNEIE     (0x00000020UL)  /*!< RXNE Interrupt Enable */
#define USART_CR1_TCIE       (0x00000040UL)  /*!< Transmission Complete Interrupt Enable */
#define USART_CR1_TXEIE      (0x00000080UL)  /*!< TXE Interrupt Enable */
#define USART_CR1_PEIE       (0x00000100UL)  /*!< PE Interrupt Enable */
#define USART_CR1_PS         (0x00000200UL)  /*!< Parity Selection */
#define USART_CR1_PCE        (0x00000400UL)  /*!< Parity Control Enable */
#define USART_CR1_WAKE       (0x00000800UL)  /*!< Wakeup Method */
#define USART_CR1_M0         (0x00001000UL)  /*!< Word Length Bit 0 */
#define USART_CR1_MME        (0x00002000UL)  /*!< Mute Mode Enable */
#define USART_CR1_CMIE       (0x00004000UL)  /*!< Character Match Interrupt Enable */
#define USART_CR1_OVER8      (0x00008000UL)  /*!< Oversampling Mode (0=16, 1=8) */
#define USART_CR1_DEDT       (0x001F0000UL)  /*!< Driver Enable De-Assertion Time [4:0] */
#define USART_CR1_DEAT       (0x03E00000UL)  /*!< Driver Enable Assertion Time [4:0] */
#define USART_CR1_RTOIE      (0x04000000UL)  /*!< Receiver Timeout Interrupt Enable */
#define USART_CR1_EOBIE      (0x08000000UL)  /*!< End of Block Interrupt Enable */
#define USART_CR1_M1         (0x10000000UL)  /*!< Word Length Bit 1 */

/* CR1 Word Length */
#define USART_CR1_M_8BIT     (0x00000000UL)  /*!< 8 Data bits */
#define USART_CR1_M_9BIT     (0x00001000UL)  /*!< 9 Data bits */
#define USART_CR1_M_7BIT     (0x10000000UL)  /*!< 7 Data bits (M1=1, M0=0) */

/* CR2 Clock Polarity and Phase (for Synchronous mode) */
#define USART_CR2_CPHA       (0x00000200UL)  /*!< Clock Phase */
#define USART_CR2_CPOL       (0x00000400UL)  /*!< Clock Polarity */

/* CR2 Register Bits */
#define USART_CR2_ADDM       (0x0000000FUL)  /*!< Address of the USART node [3:0] */
#define USART_CR2_LBDL       (0x00000020UL)  /*!< LIN Break Detection Length */
#define USART_CR2_LBDIE      (0x00000040UL)  /*!< LIN Break Detection Interrupt Enable */
#define USART_CR2_LBCL       (0x00000100UL)  /*!< Last Bit Clock Pulse */
#define USART_CR2_CPHA       (0x00000200UL)  /*!< Clock Phase */
#define USART_CR2_CPOL       (0x00000400UL)  /*!< Clock Polarity */
#define USART_CR2_CLKEN      (0x00000800UL)  /*!< Clock Enable */
#define USART_CR2_STOP       (0x00003000UL)  /*!< STOP Bits [1:0] */
#define USART_CR2_STOP_1BIT  (0x00000000UL)  /*!< 1 Stop Bit */
#define USART_CR2_STOP_0_5BIT (0x00001000UL) /*!< 0.5 Stop Bit */
#define USART_CR2_STOP_2BIT  (0x00002000UL)  /*!< 2 Stop Bits */
#define USART_CR2_STOP_1_5BIT (0x00003000UL) /*!< 1.5 Stop Bits */
#define USART_CR2_SWAP       (0x00004000UL)  /*!< SWAP TX/RX Pins */
#define USART_CR2_RXINV      (0x00008000UL)  /*!< RX Pin Inversion */
#define USART_CR2_TXINV      (0x00010000UL)  /*!< TX Pin Inversion */
#define USART_CR2_DATAINV    (0x00020000UL)  /*!< Binary Data Inversion */
#define USART_CR2_MSBFIRST   (0x00040000UL)  /*!< Most Significant Bit First */
#define USART_CR2_ABREN      (0x00080000UL)  /*!< Auto Baud Rate Enable */
#define USART_CR2_ABRMOD     (0x00180000UL)  /*!< Auto Baud Rate Mode [1:0] */
#define USART_CR2_RTOEN      (0x00200000UL)  /*!< Receiver Timeout Enable */
#define USART_CR2_ADD        (0xFF000000UL)  /*!< Address of the USART node [7:0] */

/* CR3 Register Bits */
#define USART_CR3_EIE        (0x00000001UL)  /*!< Error Interrupt Enable */
#define USART_CR3_IREN       (0x00000002UL)  /*!< IrDA Mode Enable */
#define USART_CR3_IRLP       (0x00000004UL)  /*!< IrDA Low Power */
#define USART_CR3_HDSEL      (0x00000008UL)  /*!< Half-Duplex Selection */
#define USART_CR3_NACK       (0x00000010UL)  /*!< Smartcard NACK Enable */
#define USART_CR3_SCEN       (0x00000020UL)  /*!< Smartcard Mode Enable */
#define USART_CR3_DMAR       (0x00000040UL)  /*!< DMA Enable Receiver */
#define USART_CR3_DMAT       (0x00000080UL)  /*!< DMA Enable Transmitter */
#define USART_CR3_RTSE       (0x00000100UL)  /*!< RTS Enable */
#define USART_CR3_CTSE       (0x00000200UL)  /*!< CTS Enable */
#define USART_CR3_CTSIE      (0x00000400UL)  /*!< CTS Interrupt Enable */
#define USART_CR3_ONEBIT     (0x00000800UL)  /*!< One Bit Method */
#define USART_CR3_OVRDIS     (0x00001000UL)  /*!< Overrun Disable */
#define USART_CR3_DDRE       (0x00002000UL)  /*!< DMA Disable on Reception Error */
#define USART_CR3_DEM        (0x00004000UL)  /*!< Driver Enable Mode */
#define USART_CR3_DEP        (0x00008000UL)  /*!< Driver Enable Polarity */
#define USART_CR3_SCARCNT    (0x000E0000UL)  /*!< Smartcard Auto Retry Count [2:0] */
#define USART_CR3_WUS        (0x00300000UL)  /*!< Wakeup from Stop Mode [1:0] */
#define USART_CR3_WUS_START  (0x00000000UL)  /*!< Wakeup on START bit */
#define USART_CR3_WUS_FF     (0x00100000UL)  /*!< Wakeup on FFIELD */
#define USART_CR3_WUS_ADDR   (0x00200000UL)  /*!< Wakeup on Address match */
#define USART_CR3_TXFTIE     (0x01000000UL)  /*!< TXFIFO threshold interrupt enable */
#define USART_CR3_TCBGTIE    (0x02000000UL)  /*!< Transmission complete before guard time interrupt enable */
#define USART_CR3_RXFTIE     (0x04000000UL)  /*!< RXFIFO threshold interrupt enable */

/* RQR Register Bits */
#define USART_RQR_ABRRQ      (0x00000001UL)  /*!< Auto Baud Rate Request */
#define USART_RQR_SBKRQ      (0x00000002UL)  /*!< Send Break Request */
#define USART_RQR_MMRQ       (0x00000004UL)  /*!< Mute Mode Request */
#define USART_RQR_RXFRQ      (0x00000008UL)  /*!< Receive Data Flush Request */
#define USART_RQR_TXFRQ      (0x00000010UL)  /*!< Transmit Data Flush Request */

/* ISR Register Bits */
#define USART_ISR_PE         (0x00000001UL)  /*!< Parity Error */
#define USART_ISR_FE         (0x00000002UL)  /*!< Framing Error */
#define USART_ISR_NE         (0x00000004UL)  /*!< Noise Detected Flag */
#define USART_ISR_ORE        (0x00000008UL)  /*!< Overrun Error */
#define USART_ISR_IDLE       (0x00000010UL)  /*!< IDLE Line Detected */
#define USART_ISR_RXNE       (0x00000020UL)  /*!< Read Data Register Not Empty */
#define USART_ISR_TC         (0x00000040UL)  /*!< Transmission Complete */
#define USART_ISR_TXE        (0x00000080UL)  /*!< Transmit Data Register Empty */
#define USART_ISR_LBDF       (0x00000100UL)  /*!< LIN Break Detection Flag */
#define USART_ISR_CTSIF      (0x00000200UL)  /*!< CTS Interrupt Flag */
#define USART_ISR_CTS        (0x00000400UL)  /*!< CTS Flag */
#define USART_ISR_RTOF       (0x00000800UL)  /*!< Receiver Timeout Flag */
#define USART_ISR_EOBF       (0x00001000UL)  /*!< End of Block Flag */
#define USART_ISR_ABRE       (0x00004000UL)  /*!< Auto Baud Rate Error */
#define USART_ISR_ABR        (0x00008000UL)  /*!< Auto Baud Rate Flag */
#define USART_ISR_BUSY       (0x00010000UL)  /*!< USART Busy Flag */
#define USART_ISR_CMF        (0x00020000UL)  /*!< Character Match Flag */
#define USART_ISR_SBKF       (0x00040000UL)  /*!< Send Break Flag */
#define USART_ISR_RXFF       (0x00080000UL)  /*!< RXFIFO Full */
#define USART_ISR_TXFF       (0x00100000UL)  /*!< TXFIFO Full */
#define USART_ISR_RXFE       (0x00200000UL)  /*!< RXFIFO Empty */
#define USART_ISR_TXFE       (0x00400000UL)  /*!< TXFIFO Empty */
#define USART_ISR_TCBGT      (0x00800000UL)  /*!< Transmission Complete Before Guard Time Flag */

/* ICR Register Bits */
#define USART_ICR_PECF       (0x00000001UL)  /*!< Parity Error Clear */
#define USART_ICR_FECF       (0x00000002UL)  /*!< Framing Error Clear */
#define USART_ICR_NCF        (0x00000004UL)  /*!< Noise Detected Flag Clear */
#define USART_ICR_ORECF      (0x00000008UL)  /*!< Overrun Error Clear */
#define USART_ICR_IDLECF     (0x00000010UL)  /*!< IDLE Line Detected Clear */
#define USART_ICR_TCCF       (0x00000040UL)  /*!< Transmission Complete Clear */
#define USART_ICR_LBDCF      (0x00000100UL)  /*!< LIN Break Detection Clear */
#define USART_ICR_CTSCF      (0x00000200UL)  /*!< CTS Interrupt Clear */
#define USART_ICR_RTOCF      (0x00000800UL)  /*!< Receiver Timeout Clear */
#define USART_ICR_EOBCF      (0x00001000UL)  /*!< End of Block Clear */
#define USART_ICR_CMCF       (0x00020000UL)  /*!< Character Match Clear */
#define USART_ICR_TCBGTCF    (0x00800000UL)  /*!< Transmission Complete Before Guard Time Clear */

/*============================================================================
 * UART Configuration Enumerations
 *============================================================================*/

/* UART Word Length */
typedef enum {
    UART_WORDLENGTH_8B = 0x00,  /*!< 8 Data bits */
    UART_WORDLENGTH_9B = 0x01,  /*!< 9 Data bits */
    UART_WORDLENGTH_7B = 0x03   /*!< 7 Data bits */
} UART_WordLengthTypeDef;

/* UART Stop Bits */
typedef enum {
    UART_STOPBITS_1     = 0x00,  /*!< 1 Stop bit */
    UART_STOPBITS_0_5  = 0x01,  /*!< 0.5 Stop bit */
    UART_STOPBITS_2    = 0x02,  /*!< 2 Stop bits */
    UART_STOPBITS_1_5  = 0x03   /*!< 1.5 Stop bits */
} UART_StopBitsTypeDef;

/* UART Parity */
typedef enum {
    UART_PARITY_NONE = 0x00,    /*!< No Parity */
    UART_PARITY_EVEN = 0x02,    /*!< Even Parity */
    UART_PARITY_ODD  = 0x03    /*!< Odd Parity */
} UART_ParityTypeDef;

/* UART Hardware Flow Control */
typedef enum {
    UART_HWCONTROL_NONE   = 0x00,  /*!< No hardware flow control */
    UART_HWCONTROL_RTS    = 0x01,  /*!< RTS flow control */
    UART_HWCONTROL_CTS    = 0x02,  /*!< CTS flow control */
    UART_HWCONTROL_RTS_CTS = 0x03  /*!< RTS and CTS flow control */
} UART_HwFlowCtrlTypeDef;

/* UART Mode */
typedef enum {
    UART_MODE_RX = 0x04,          /*!< Receiver mode */
    UART_MODE_TX = 0x08,          /*!< Transmitter mode */
    UART_MODE_TX_RX = 0x0C        /*!< Transmitter and Receiver mode */
} UART_ModeTypeDef;

/* UART Oversampling */
typedef enum {
    UART_OVERSAMPLING_16 = 0x00,  /*!< Oversampling by 16 */
    UART_OVERSAMPLING_8  = 0x01   /*!< Oversampling by 8 */
} UART_OverSamplingTypeDef;

/* UART Clock Polarity */
typedef enum {
    UART_CLOCKPOLARITY_LOW  = 0x00,  /*!< Clock polarity: low */
    UART_CLOCKPOLARITY_HIGH = 0x01   /*!< Clock polarity: high */
} UART_ClockPolarityTypeDef;

/* UART Clock Phase */
typedef enum {
    UART_CLOCKPHASE_1EDGE = 0x00,   /*!< Clock phase: first edge */
    UART_CLOCKPHASE_2EDGE = 0x01    /*!< Clock phase: second edge */
} UART_ClockPhaseTypeDef;

/* UART Clock Last Bit */
typedef enum {
    UART_CLOCKLASTBIT_DISABLE = 0x00,  /*!< Clock pulse of last data bit not output */
    UART_CLOCKLASTBIT_ENABLE  = 0x01   /*!< Clock pulse of last data bit output */
} UART_ClockLastBitTypeDef;

/* UART State */
typedef enum {
    UART_STATE_RESET      = 0x00,  /*!< Peripheral not initialized */
    UART_STATE_READY      = 0x01,  /*!< Peripheral ready */
    UART_STATE_BUSY       = 0x02,  /*!< Busy: one of TX/RX in progress */
    UART_STATE_BUSY_TX    = 0x12,  /*!< Busy: TX in progress */
    UART_STATE_BUSY_RX    = 0x22,  /*!< Busy: RX in progress */
    UART_STATE_BUSY_TX_RX = 0x32,  /*!< Busy: TX and RX in progress */
    UART_STATE_ERROR      = 0x04   /*!< Error */
} UART_StateTypeDef;

/* UART Error Codes */
typedef enum {
    UART_ERROR_NONE    = 0x00,  /*!< No error */
    UART_ERROR_PE      = 0x01,  /*!< Parity error */
    UART_ERROR_FE      = 0x02,  /*!< Framing error */
    UART_ERROR_NE      = 0x04,  /*!< Noise error */
    UART_ERROR_ORE     = 0x08,  /*!< Overrun error */
    UART_ERROR_DMA     = 0x10   /*!< DMA error */
} UART_ErrorTypeDef;

/* UART Transfer Direction */
typedef enum {
    UART_DIRECTION_NONE = 0x00,
    UART_DIRECTION_RX   = 0x01,
    UART_DIRECTION_TX   = 0x02,
    UART_DIRECTION_TX_RX = 0x03
} UART_TransferDirectionTypeDef;

/*============================================================================
 * UART Init Structure Definition
 *============================================================================*/
typedef struct {
    uint32_t BaudRate;                  /*!< This member configures the UART communication baud rate */
    UART_WordLengthTypeDef WordLength;  /*!< Specifies the number of data bits transmitted or received */
    UART_StopBitsTypeDef StopBits;      /*!< Specifies the number of stop bits transmitted */
    UART_ParityTypeDef Parity;          /*!< Specifies the parity mode */
    UART_HwFlowCtrlTypeDef HwFlowCtl;  /*!< Specifies whether the hardware flow control mode is enabled or disabled */
    UART_ModeTypeDef Mode;              /*!< Specifies the RX/TX mode */
    UART_OverSamplingTypeDef OverSampling; /*!< Specifies whether the oversampling mode is 8 or 16 */
    UART_ClockPolarityTypeDef ClockPolarity; /*!< Specifies the steady state clock polarity */
    UART_ClockPhaseTypeDef ClockPhase;     /*!< Specifies the clock phase */
    UART_ClockLastBitTypeDef ClockLastBit; /*!< Specifies whether the clock pulse of the last data bit is output or not */
    bool OneBitSampling;                 /*!< Enable/Disable one bit sampling method */
} UART_InitTypeDef;

/*============================================================================
 * DMA Channel Mappings for UART (STM32F051)
 *============================================================================*/
#define UART_DMA_TX_CHANNEL_1   1  /*!< DMA Channel for USART1 TX */
#define UART_DMA_RX_CHANNEL_2   2  /*!< DMA Channel for USART1 RX */
#define UART_DMA_TX_CHANNEL_3   3  /*!< DMA Channel for USART2 TX */
#define UART_DMA_RX_CHANNEL_4   4  /*!< DMA Channel for USART2 RX */

/*============================================================================
 * UART Driver API Functions
 *============================================================================*/

/* Initialization and Configuration */
void UART_Init(USART_TypeDef *USARTx, UART_InitTypeDef *UART_InitStruct);
void UART_DeInit(USART_TypeDef *USARTx);
void UART_StructInit(UART_InitTypeDef *UART_InitStruct);

/* Peripheral Control */
void UART_Enable(USART_TypeDef *USARTx);
void UART_Disable(USART_TypeDef *USARTx);
void UART_EnableTx(USART_TypeDef *USARTx);
void UART_DisableTx(USART_TypeDef *USARTx);
void UART_EnableRx(USART_TypeDef *USARTx);
void UART_DisableRx(USART_TypeDef *USARTx);

/* Polling Transmission */
void UART_SendData(USART_TypeDef *USARTx, uint16_t Data);
uint16_t UART_ReceiveData(USART_TypeDef *USARTx);
bool UART_WaitForFlag(USART_TypeDef *USARTx, uint32_t Flag, bool State, uint32_t Timeout);
bool UART_IsTxComplete(USART_TypeDef *USARTx);

/* Interrupt Functions */
void UART_EnableInterrupt(USART_TypeDef *USARTx, uint32_t Interrupt);
void UART_DisableInterrupt(USART_TypeDef *USARTx, uint32_t Interrupt);
uint32_t UART_GetInterruptStatus(USART_TypeDef *USARTx);
uint32_t UART_GetStatus(USART_TypeDef *USARTx);
void UART_ClearInterruptFlag(USART_TypeDef *USARTx, uint32_t Flag);
bool UART_GetFlag(USART_TypeDef *USARTx, uint32_t Flag);

/* DMA Functions */
void UART_EnableDMA(USART_TypeDef *USARTx, uint32_t DMARequest);
void UART_DisableDMA(USART_TypeDef *USARTx, uint32_t DMARequest);

/* Status and Error Management */
UART_StateTypeDef UART_GetState(USART_TypeDef *USARTx);
UART_ErrorTypeDef UART_GetError(USART_TypeDef *USARTx);
void UART_ClearError(USART_TypeDef *USARTx);

/* Synchronization */
bool UART_IsReadyToTransmit(USART_TypeDef *USARTx);
bool UART_IsReadyToReceive(USART_TypeDef *USARTx);
bool UART_IsBusy(USART_TypeDef *USARTx);

/* Break and Special Characters */
void UART_SendBreak(USART_TypeDef *USARTx);
void UART_RequestAutoBaudRate(USART_TypeDef *USARTx);

/* Baud Rate Calculation */
void UART_SetBaudRate(USART_TypeDef *USARTx, uint32_t BaudRate, uint32_t APBClock);

/* FIFO Functions (for STM32F0 with FIFO) */
void UART_EnableFIFO(USART_TypeDef *USARTx);
void UART_DisableFIFO(USART_TypeDef *USARTx);

/*============================================================================
 * DMA Request Definitions for UART
 *============================================================================*/
#define UART_DMA_REQ_TX   USART_CR3_DMAT  /*!< DMA Request for TX */
#define UART_DMA_REQ_RX   USART_CR3_DMAR  /*!< DMA Request for RX */

#endif /* UARTF051_H */
