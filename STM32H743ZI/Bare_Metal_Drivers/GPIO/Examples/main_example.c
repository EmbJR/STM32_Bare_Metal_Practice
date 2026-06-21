#include "GPIO_Examples.h"

/* =====================================================================
 *  GPIO examples - common SysTick millisecond delay + main entry.
 *
 *  The SysTick timer is part of the Cortex-M7 core (not a peripheral),
 *  so using it here keeps the examples free of any RCC/HAL dependency.
 *
 *  The delay polls the SysTick COUNTFLAG instead of using an interrupt,
 *  so it does not depend on the startup vector table wiring up a
 *  SysTick_Handler. This keeps the examples self-contained.
 *
 *  SYSTICK_HZ must match the actual core clock for accurate timing.
 *  The STM32H743 boots on the 64 MHz HSI, which is the default below.
 * ===================================================================== */

#define SYSTICK_HZ          64000000UL   /* default HSI core clock */

/* Cortex-M7 SysTick registers */
#define SYSTICK_BASE        0xE000E010UL
#define SYSTICK_CTRL        (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_LOAD        (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_VAL         (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))

#define SYSTICK_CTRL_ENABLE     (1U << 0)   /* counter enable           */
#define SYSTICK_CTRL_CLKSOURCE  (1U << 2)   /* use core clock           */
#define SYSTICK_CTRL_COUNTFLAG  (1U << 16)  /* set when counter reaches 0 */

void Delay_Init(void) {
    SYSTICK_LOAD = (SYSTICK_HZ / 1000U) - 1U;   /* 1 ms reload */
    SYSTICK_VAL  = 0;
    /* Enable counter on the core clock, no interrupt (polled). */
    SYSTICK_CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_ENABLE;
}

void Delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        /* Wait for one 1 ms period: COUNTFLAG sets when the timer wraps. */
        while ((SYSTICK_CTRL & SYSTICK_CTRL_COUNTFLAG) == 0U) {
            /* wait */
        }
    }
}

/* ---------------------------------------------------------------------
 *  Shared pin setup helpers, used by all examples to avoid duplicating
 *  the configuration descriptors (which could otherwise drift apart).
 * ------------------------------------------------------------------- */
void LED_Init(void) {
    GPIO_PinConfig led;
    led.pin       = LED_PIN;
    led.mode      = GPIO_MODE_OUTPUT;
    led.otype     = GPIO_OTYPE_PUSHPULL;
    led.speed     = GPIO_SPEED_LOW;
    led.pull      = GPIO_PULL_NONE;
    led.alternate = 0;
    GPIO_Init(LED_PORT, &led);
}

void SW_Init(void) {
    GPIO_PinConfig sw;
    sw.pin       = SW_PIN;
    sw.mode      = GPIO_MODE_INPUT;
    sw.otype     = GPIO_OTYPE_PUSHPULL;
    sw.speed     = GPIO_SPEED_LOW;
    sw.pull      = GPIO_PULL_UP;          /* idle HIGH, pressed -> LOW */
    sw.alternate = 0;
    GPIO_Init(SW_PORT, &sw);
}

int main(void) {
    Delay_Init();

    /* Pick ONE example to run. Each example contains its own loop. */

    Example_1_BlinkLED_1s();      /* PB14 toggles every 1 second        */
    /* Example_2_SwitchPolling();    PG0 polled -> PB14                  */
    /* Example_3_SwitchInterrupt();  PG0 EXTI   -> PB14                  */

    while (1) {
    }

    return 0;
}
