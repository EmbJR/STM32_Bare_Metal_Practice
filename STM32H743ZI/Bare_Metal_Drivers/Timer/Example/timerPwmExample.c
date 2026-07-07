/**
 * @file timerPwmExample.c
 * @brief Example: PWM generation with Capture/Compare interrupt indication.
 *
 * This example configures TIM2 to generate a 1 kHz PWM signal with 50% duty cycle
 * on PA0 (TIM2_CH1). It demonstrates basic timer initialization, PWM channel
 * configuration, and Capture/Compare interrupt handling to indicate PWM events.
 * 
 * An LED on PA1 toggles each time the CC1 interrupt occurs, providing a visual
 * indication that the PWM compare event has been processed.
 * 
 * Note: This example assumes the system clock and global interrupts are already
 * enabled in the startup code. The NVIC interrupt for TIM2 is explicitly enabled
 * using the NVIC driver API.
 */

#include "h7timer.h"
#include "Bare_Metal_Drivers/GPIO/GPIOH743ZI.h"
#include "Bare_Metal_Drivers/NVIC/nvic.h"

/* Define TIM2 base address */
#define TIM2 ((TIM_TypeDef *)TIM2_BASE)
#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)

/* LED pin for interrupt indication */
#define LED_PIN     1

int main(void)
{
    /* GPIO initialization for PA0 (PWM output) and PA1 (LED indicator) */
    GPIO_PinConfig led_config = {
        .pin = LED_PIN,
        .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_LOW,
        .pull = GPIO_PULL_NONE,
        .alternate = 0
    };
    GPIO_EnableClock(GPIOA);
    GPIO_Init(GPIOA, &led_config);
    
    /* Enable TIM2 interrupt in NVIC */
    NVIC_EnableIRQ(TIM2_IRQn);

    /* Initialize TIM2 for PWM output */
    H7_TIM_Init(TIM2, 79, 999, TIM_COUNTERMODE_UP);  // 80 MHz timer clock, 1 kHz frequency

    /* Configure channel 1 for PWM output, 50% duty cycle */
    H7_TIM_PWM_ConfigChannel(TIM2, 1, 500, TIM_OCPOLARITY_HIGH, TIM_OCMODE_PWM1);

    /* Enable CC1 interrupt */
    H7_TIM_EnableInterrupt(TIM2, TIM_CC1_INTERRUPT);

    /* Enable the timer */
    H7_TIM_Start(TIM2);

    /* Main loop - PWM and interrupt handling run in background */
    while (1)
    {
        // Main application can perform other tasks here
    }

    return 0;
}