/**
 * @file TimerIntExampleCC1.c
 * @brief Example: Timer capture/compare interrupt using NVIC driver.
 *
 * Demonstrates NVIC-based interrupt management for a timer peripheral.
 * TIM2 Channel 1 is configured for output compare interrupt. The ISR
 * toggles an LED on PA0 with a 1 Hz rate.
 *
 * Timer:  TIM2 (APB1L, IRQ 28)
 * GPIO:   PA0 (output, toggled by ISR)
 */

#include "Bare_Metal_Drivers/NVIC/h7nvic.h"
#include "Bare_Metal_Drivers/GPIO/GPIOH743ZI.h"

/* =====================================================================
 *  Timer register structure (subset needed for TIM2)
 * ===================================================================== */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RESERVED0;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
} TIMB_TypeDef;

#define TIM2_BASE               0x40000000UL
#define TIM2                    ((TIMB_TypeDef *)TIM2_BASE)

/* =====================================================================
 *  RCC clock enable for TIM2 (APB1L, bit 0)
 * ===================================================================== */
#define RCC_BASE                0x58024400UL
#define RCC_APB1LENR            (*(volatile uint32_t *)(RCC_BASE + 0xE8U))
#define RCC_APB1LENR_TIM2EN     (1U << 0)

/* =====================================================================
 *  Timer interrupt flags in SR and DIER
 * ===================================================================== */
#define TIM_SR_CC1IF            (1U << 1)
#define TIM_DIER_CC1IE          (1U << 1)

/* =====================================================================
 *  LED pin
 * ===================================================================== */
#define LED_PORT                GPIOA
#define LED_PIN                 0U

/* =====================================================================
 *  Timer configuration: 1 MHz count, 1 second period, 0.5s compare
 *  Assumes APB1 timer clock = 100 MHz (typical for 400 MHz sysclk)
 * ===================================================================== */
#define TIM_PRESCALER           99U     /* 100 MHz / 100 = 1 MHz  */
#define TIM_PERIOD              999U    /* 1 MHz / 1000 = 1 kHz  */
#define TIM_COMPARE             500U    /* 0.5s at 1 kHz         */

/* =====================================================================
 *  User callback for TIM2 interrupt
 * ===================================================================== */
void TIM2_Callback(void)
{
    if (TIM2->SR & TIM_SR_CC1IF)
    {
        TIM2->SR = ~TIM_SR_CC1IF;
        GPIO_TogglePin(LED_PORT, LED_PIN);
    }
}

/* =====================================================================
 *  LED initialisation
 * ===================================================================== */
static void LED_Init(void)
{
    GPIO_PinConfig cfg = {
        .pin   = LED_PIN,
        .mode  = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_LOW,
        .pull  = GPIO_PULL_NONE
    };
    GPIO_Init(LED_PORT, &cfg);
}

/* =====================================================================
 *  TIM2 initialisation for output compare interrupt
 * ===================================================================== */
static void TIM2_Init(void)
{
    RCC_APB1LENR |= RCC_APB1LENR_TIM2EN;

    TIM2->CR1  = 0U;
    TIM2->PSC  = TIM_PRESCALER;
    TIM2->ARR  = TIM_PERIOD;
    TIM2->CCR1 = TIM_COMPARE;

    /* Output compare mode, channel 1 as output */
    TIM2->CCMR1 &= ~0x03U;
    TIM2->CCMR1 |=  0x30U;

    /* Enable CC1 output */
    TIM2->CCER |= (1U << 0);

    /* Generate update event to load prescaler and auto-reload */
    TIM2->EGR  = 0x01U;

    /* Enable CC1 interrupt */
    TIM2->DIER |= TIM_DIER_CC1IE;

    /* Start counter */
    TIM2->CR1  |= 0x01U;
}

/* =====================================================================
 *  Main example
 * ===================================================================== */
int main(void)
{
    NVIC_Init();

    LED_Init();
    TIM2_Init();

    NVIC_RegisterCallback(NVIC_IRQ_TIM2, TIM2_Callback);
    NVIC_SetPriority(NVIC_IRQ_TIM2, 0x20U);
    NVIC_EnableIRQ(NVIC_IRQ_TIM2);

    while (1)
    {
        /* CPU free for other tasks */
    }

    return 0;
}
