/**
 * @file timerGpioInitExample.c
 * @brief Example: GPIO initialization for timer capture/compare output.
 *
 * This example demonstrates how to configure a GPIO pin for timer capture/compare
 * output using the GPIO library. It shows the complete setup for PA0 as TIM2_CH1
 * output, including enabling clocks, setting alternate function mode, and output
 * parameters.
 */

#include "h7timer.h"
#include "Bare_Metal_Drivers/GPIO/GPIOH743ZI.h"

/* Timer and GPIO definitions */
#define TIM2 ((TIM_TypeDef *)TIM2_BASE)
#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)

int main(void)
{
    /* Step 1: Enable GPIOA clock */
    GPIO_EnableClock(GPIOA);
    
    /* Step 2: Configure PA0 for alternate function (TIM2_CH1) */
    GPIO_PinConfig af_config = {
        .pin = 0,                    /* PA0 */
        .mode = GPIO_MODE_ALTERNATE, /* Alternate function mode */
        .otype = GPIO_OTYPE_PUSHPULL, /* Push-pull output */
        .speed = GPIO_SPEED_VERYHIGH, /* Very high speed */
        .pull = GPIO_PULL_NONE,      /* No pull-up/pull-down */
        .alternate = 1               /* AF1 for TIM2_CH1 on PA0 */
    };
    GPIO_Init(GPIOA, &af_config);
    
    /* Step 3: Enable TIM2 clock (assuming RCC driver exists) */
    /* This would typically call H7_RCC_EnableClock(RCC_TIM2) */
    /* For now, we assume the timer clock is already enabled */
    
    /* Step 4: Initialize TIM2 for PWM output */
    H7_TIM_Init(TIM2, 79, 999, TIM_COUNTERMODE_UP);  // 80 MHz timer clock, 1 kHz frequency
    
    /* Step 5: Configure channel 1 for PWM */
    H7_TIM_PWM_ConfigChannel(TIM2, 1, 500, TIM_OCPOLARITY_HIGH, TIM_OCMODE_PWM1);
    
    /* Step 6: Start the timer */
    H7_TIM_Start(TIM2);
    
    /* Main loop */
    while (1)
    {
        // PWM signal is generated on PA0 (TIM2_CH1)
    }
    
    return 0;
}