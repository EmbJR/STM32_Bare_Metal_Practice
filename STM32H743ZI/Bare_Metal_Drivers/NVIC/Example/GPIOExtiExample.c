/**
 * @file GPIOExtiExample.c
 * @brief Example: GPIO external interrupt configuration using NVIC driver.
 *
 * Demonstrates how to use the NVIC driver API to configure EXTI interrupts
 * for a GPIO pin. A button on PG0 triggers an interrupt on falling edge,
 * toggling an LED on PB14. Uses NVIC_RegisterCallback for the ISR handler.
 *
 * Hardware:
 *  - PG0:  Push button (active low, pull-up)
 *  - PB14: LED (active high)
 *  - EXTI line 0, IRQ 6 (EXTI0)
 */

#include "Bare_Metal_Drivers/NVIC/h7nvic.h"
#include "Bare_Metal_Drivers/GPIO/GPIOH743ZI.h"

/* =====================================================================
 *  Pin definitions
 * ===================================================================== */
#define BTN_PORT                GPIOG
#define BTN_PIN                 0U
#define LED_PORT                GPIOB
#define LED_PIN                 14U

/* =====================================================================
 *  EXTI base address (for flag management)
 * ===================================================================== */
#define EXTI_BASE               0x58000000UL
#define EXTI_CPUPR1             (*(volatile uint32_t *)(EXTI_BASE + 0x88U))
#define EXTI_CPUIMR1            (*(volatile uint32_t *)(EXTI_BASE + 0x80U))
#define EXTI_RTSR1              (*(volatile uint32_t *)(EXTI_BASE + 0x00U))
#define EXTI_FTSR1              (*(volatile uint32_t *)(EXTI_BASE + 0x04U))

/* =====================================================================
 *  RCC registers (for SYSCFG clock enable)
 * ===================================================================== */
#define RCC_BASE                0x58024400UL
#define RCC_APB4ENR             (*(volatile uint32_t *)(RCC_BASE + 0xF4U))
#define RCC_APB4ENR_SYSCFGEN    (1U << 1)

/* =====================================================================
 *  SYSCFG registers (to map EXTI line to port)
 * ===================================================================== */
#define SYSCFG_BASE             0x58000400UL
#define SYSCFG_EXTICR1          (*(volatile uint32_t *)(SYSCFG_BASE + 0x08U))

/* =====================================================================
 *  User callback for EXTI0 interrupt
 * ===================================================================== */
void EXTI0_Callback(void)
{
    EXTI_CPUPR1 = (1U << BTN_PIN);
    GPIO_TogglePin(LED_PORT, LED_PIN);
}

/* =====================================================================
 *  Button and LED initialisation
 * ===================================================================== */
static void BTN_Init(void)
{
    GPIO_PinConfig cfg = {
        .pin  = BTN_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull = GPIO_PULL_UP
    };
    GPIO_Init(BTN_PORT, &cfg);
}

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
 *  EXTI configuration for GPIO pin
 * ===================================================================== */
static void EXTI_Config(uint8_t pin)
{
    /* Enable SYSCFG clock */
    RCC_APB4ENR |= RCC_APB4ENR_SYSCFGEN;

    /* Map EXTI line 0 to port G (port index = 6 for GPIOG) */
    uint32_t shift = (pin & 0x3U) * 4U;
    SYSCFG_EXTICR1 &= ~(0xFU << shift);
    SYSCFG_EXTICR1 |=  (6U << shift);

    /* Configure falling edge trigger */
    EXTI_FTSR1 |= (1U << pin);

    /* Unmask EXTI line in the CPU interrupt mask */
    EXTI_CPUIMR1 |= (1U << pin);
}

/* =====================================================================
 *  Main example
 * ===================================================================== */
int main(void)
{
    NVIC_Init();

    LED_Init();
    BTN_Init();

    EXTI_Config(BTN_PIN);

    NVIC_RegisterCallback(NVIC_IRQ_EXTI0, EXTI0_Callback);
    NVIC_SetPriority(NVIC_IRQ_EXTI0, 0x10U);
    NVIC_EnableIRQ(NVIC_IRQ_EXTI0);

    while (1)
    {
        /* CPU can sleep or do other tasks */
    }

    return 0;
}
