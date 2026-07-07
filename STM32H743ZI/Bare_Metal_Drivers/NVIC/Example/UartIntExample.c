/**
 * @file UartIntExample.c
 * @brief Example: UART RX interrupt using NVIC driver.
 *
 * Demonstrates NVIC-based UART receive interrupt. USART1 is configured
 * to receive data and trigger an interrupt on each received byte.
 * The callback echoes the received byte back.
 *
 * USART1: APB2 bus, IRQ 37
 * TX: PA9 (AF7), RX: PA10 (AF7)
 * Baud: 115200, 8N1
 *
 * Assumes system clock = 64 MHz (HSI), APB2 clock = 64 MHz.
 */

#include "Bare_Metal_Drivers/NVIC/h7nvic.h"
#include "Bare_Metal_Drivers/GPIO/GPIOH743ZI.h"

/* =====================================================================
 *  USART1 register structure
 * ===================================================================== */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t BRR;
    volatile uint32_t GTPR;
    volatile uint32_t RTOR;
    volatile uint32_t RQR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t RDR;
    volatile uint32_t TDR;
} USART_TypeDef;

#define USART1_BASE             0x40011000UL
#define USART1                  ((USART_TypeDef *)USART1_BASE)

/* USART1 CR1 bits */
#define USART_CR1_UE            (1U << 0)
#define USART_CR1_RE            (1U << 2)
#define USART_CR1_TE            (1U << 3)
#define USART_CR1_RXNEIE        (1U << 5)

/* USART1 ISR bits */
#define USART_ISR_RXNE          (1U << 5)
#define USART_ISR_TXE           (1U << 7)

/* =====================================================================
 *  RCC clock enables
 * ===================================================================== */
#define RCC_BASE                0x58024400UL
#define RCC_APB2ENR             (*(volatile uint32_t *)(RCC_BASE + 0xF0U))
#define RCC_APB2ENR_USART1EN    (1U << 4)

/* =====================================================================
 *  Baud rate: BRR = APB2_CLK / baudrate
 *  For 64 MHz / 115200 = 555.55 => DIV_Mantissa=555, DIV_Fraction=9
 * ===================================================================== */
#define USART_BRR_115200_64MHZ  ((555U << 4) | 9U)

/* =====================================================================
 *  UART RX callback
 * ===================================================================== */
void USART1_RX_Callback(void)
{
    if (USART1->ISR & USART_ISR_RXNE)
    {
        uint8_t rx_data = (uint8_t)(USART1->RDR & 0xFFU);

        /* Echo: wait until TX empty, then send */
        while (!(USART1->ISR & USART_ISR_TXE)) { /* spin */ }
        USART1->TDR = rx_data;
    }
}

/* =====================================================================
 *  GPIO initialisation for USART1 TX/RX (AF7)
 * ===================================================================== */
static void USART1_GPIO_Init(void)
{
    GPIO_PinConfig tx_cfg = {
        .pin       = 9U,
        .mode      = GPIO_MODE_ALTERNATE,
        .otype     = GPIO_OTYPE_PUSHPULL,
        .speed     = GPIO_SPEED_HIGH,
        .pull      = GPIO_PULL_NONE,
        .alternate = 7U
    };
    GPIO_PinConfig rx_cfg = {
        .pin       = 10U,
        .mode      = GPIO_MODE_ALTERNATE,
        .otype     = GPIO_OTYPE_PUSHPULL,
        .speed     = GPIO_SPEED_HIGH,
        .pull      = GPIO_PULL_NONE,
        .alternate = 7U
    };

    GPIO_Init(GPIOA, &tx_cfg);
    GPIO_Init(GPIOA, &rx_cfg);
}

/* =====================================================================
 *  USART1 initialisation
 * ===================================================================== */
static void USART1_Init(void)
{
    RCC_APB2ENR |= RCC_APB2ENR_USART1EN;

    USART1->CR1 = 0U;
    USART1->BRR = USART_BRR_115200_64MHZ;

    /* Enable RX, TX, and RX interrupt */
    USART1->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE;

    /* Enable USART */
    USART1->CR1 |= USART_CR1_UE;
}

/* =====================================================================
 *  Main example
 * ===================================================================== */
int main(void)
{
    NVIC_Init();

    USART1_GPIO_Init();
    USART1_Init();

    NVIC_RegisterCallback(NVIC_IRQ_USART1, USART1_RX_Callback);
    NVIC_SetPriority(NVIC_IRQ_USART1, 0x10U);
    NVIC_EnableIRQ(NVIC_IRQ_USART1);

    while (1)
    {
        /* CPU sleeps or does other work; RX handled by interrupt */
    }

    return 0;
}
