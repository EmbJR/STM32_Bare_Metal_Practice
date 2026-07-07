/**
 * @file PriorityGroupingExample.c
 * @brief Example: NVIC priority grouping and nested interrupt demonstration.
 *
 * Demonstrates NVIC priority grouping, setting different priorities
 * for multiple interrupts, and showing how higher-priority interrupts
 * preempt lower-priority ones.
 *
 * Uses TIM2 (lower priority) and EXTI0 (higher priority) to demonstrate
 * preemption. EXTI0 interrupt preempts the TIM2 ISR.
 */

#include "Bare_Metal_Drivers/NVIC/h7nvic.h"
#include "Bare_Metal_Drivers/GPIO/GPIOH743ZI.h"

/* =====================================================================
 *  EXTI and SYSCFG registers (for EXTI0 configuration)
 * ===================================================================== */
#define EXTI_BASE               0x58000000UL
#define EXTI_CPUIMR1            (*(volatile uint32_t *)(EXTI_BASE + 0x80U))
#define EXTI_CPUPR1             (*(volatile uint32_t *)(EXTI_BASE + 0x88U))
#define EXTI_FTSR1              (*(volatile uint32_t *)(EXTI_BASE + 0x04U))

#define SYSCFG_BASE             0x58000400UL
#define SYSCFG_EXTICR1          (*(volatile uint32_t *)(SYSCFG_BASE + 0x08U))
#define RCC_BASE                0x58024400UL
#define RCC_APB4ENR             (*(volatile uint32_t *)(RCC_BASE + 0xF4U))
#define RCC_APB4ENR_SYSCFGEN    (1U << 1)

/* Timer (TIM2) base */
#define TIM2_BASE               0x40000000UL

/* =====================================================================
 *  LED pins for visual indication of ISR execution
 * ===================================================================== */
#define LED1_PORT               GPIOB
#define LED1_PIN                0U
#define LED2_PORT               GPIOB
#define LED2_PIN                7U

static void LED1_Init(void)
{
    GPIO_PinConfig cfg = {
        .pin = LED1_PIN, .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSHPULL, .speed = GPIO_SPEED_LOW,
        .pull = GPIO_PULL_NONE
    };
    GPIO_Init(LED1_PORT, &cfg);
}

static void LED2_Init(void)
{
    GPIO_PinConfig cfg = {
        .pin = LED2_PIN, .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSHPULL, .speed = GPIO_SPEED_LOW,
        .pull = GPIO_PULL_NONE
    };
    GPIO_Init(LED2_PORT, &cfg);
}

/* =====================================================================
 *  EXTI0 callback (HIGHER priority: preempts timer ISR)
 * ===================================================================== */
void HighPri_Callback(void)
{
    EXTI_CPUPR1 = (1U << 0U);
    GPIO_TogglePin(LED2_PORT, LED2_PIN);

    /* Simulate longer processing */
    for (volatile uint32_t i = 0U; i < 100000U; i++) { }
}

/* =====================================================================
 *  TIM2 callback (LOWER priority: gets preempted)
 * ===================================================================== */
void LowPri_Callback(void)
{
    GPIO_TogglePin(LED1_PORT, LED1_PIN);

    /* Simulate longer processing - EXTI0 can preempt here */
    for (volatile uint32_t i = 0U; i < 500000U; i++) { }
}

/* =====================================================================
 *  EXTI0 basic setup (PG0 on EXTI0 line, falling edge)
 * ===================================================================== */
static void EXTI0_Setup(void)
{
    RCC_APB4ENR |= RCC_APB4ENR_SYSCFGEN;

    /* Map EXTI0 to port G (index 6) */
    SYSCFG_EXTICR1 &= ~0x000FU;
    SYSCFG_EXTICR1 |=  0x0006U;

    /* Falling edge trigger */
    EXTI_FTSR1 |= (1U << 0U);

    /* Unmask interrupt */
    EXTI_CPUIMR1 |= (1U << 0U);
}

/* =====================================================================
 *  TIM2 basic setup (just enable the interrupt and set preload)
 * ===================================================================== */
static void TIM2_Setup(void)
{
    #define RCC_APB1LENR        (*(volatile uint32_t *)(RCC_BASE + 0xE8U))
    RCC_APB1LENR |= (1U << 0U);

    TIM_TypeDef *tim = (TIM_TypeDef *)TIM2_BASE;
    tim->PSC  = 63999U;
    tim->ARR  = 999U;
    tim->DIER = (1U << 0);
    tim->EGR  = 0x01U;
    tim->CR1  = 0x01U;
}

/* Timer struct for TIM2 */
typedef struct {
    volatile uint32_t CR1, CR2, SMCR, DIER, SR, EGR, CCMR1, CCMR2, CCER, CNT;
    volatile uint32_t PSC, ARR;
    uint32_t RESERVED0;
    volatile uint32_t CCR1, CCR2, CCR3, CCR4;
    uint32_t RESERVED1[7];
} TIM_TypeDef;

/* =====================================================================
 *  Main example
 * ===================================================================== */
int main(void)
{
    NVIC_Init();

    /* Priority grouping: 3 bits group, 1 bit subpriority */
    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_3);

    LED1_Init();
    LED2_Init();

    EXTI0_Setup();
    TIM2_Setup();

    /* EXTI0: higher priority (0x00) */
    NVIC_RegisterCallback(NVIC_IRQ_EXTI0, HighPri_Callback);
    NVIC_SetPriority(NVIC_IRQ_EXTI0, 0x00U);
    NVIC_EnableIRQ(NVIC_IRQ_EXTI0);

    /* TIM2: lower priority (0x80) */
    NVIC_RegisterCallback(NVIC_IRQ_TIM2, LowPri_Callback);
    NVIC_SetPriority(NVIC_IRQ_TIM2, 0x80U);
    NVIC_EnableIRQ(NVIC_IRQ_TIM2);

    /* Check priorities */
    uint32_t exti0_prio = NVIC_GetPriority(NVIC_IRQ_EXTI0);
    uint32_t tim2_prio  = NVIC_GetPriority(NVIC_IRQ_TIM2);

    if (exti0_prio < tim2_prio)
    {
        /* EXTI0 has higher priority: will preempt TIM2 ISR */
    }

    while (1)
    {
        /* Interrupts preempt as expected:
         *  - TIM2 fires periodically (lower priority)
         *  - Pressing the button (PG0) triggers EXTI0 which preempts TIM2
         *  - LED1 shows TIM2 execution, LED2 shows EXTI0 execution
         */
    }

    return 0;
}
