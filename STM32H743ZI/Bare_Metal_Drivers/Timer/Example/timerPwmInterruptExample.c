/**
 * @file timerPwmInterruptExample.c
 * @brief Example: PWM generation using timer capture/compare interrupt.
 *
 * This example demonstrates how to generate a PWM signal using the timer's
 * capture/compare interrupt. The timer is configured to trigger an interrupt
 * on CC1, and the ISR toggles a GPIO pin. This approach allows PWM generation
 * on any GPIO pin (not limited to timer-specific alternate function pins).
 *
 * Timer: TIM2 (general-purpose)
 * GPIO: PA0 (connected to TIM2_CH1 alternate function, but used here via GPIO)
 */

#include "h7timer.h"
#include "Bare_Metal_Drivers/GPIO/GPIOH743ZI.h"

/* Timer and GPIO definitions */
#define TIM2 ((TIM_TypeDef *)TIM2_BASE)
#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)

/* PWM frequency and duty cycle configuration */
#define PWM_PERIOD    1000    /* Timer period for ~1 kHz PWM */
#define DUTY_CYCLE    500     /* 50% duty cycle */

volatile uint8_t pin_state = 0;

/* Timer2 CC1 interrupt handler */
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & 0x00000002)  /* Check CC1 interrupt flag */
    {
        /* Toggle the GPIO pin */
        if (pin_state)
        {
            GPIO_ResetPin(GPIOA, 0);  /* PA0 = low */
            pin_state = 0;
        }
        else
        {
            GPIO_SetPin(GPIOA, 0);    /* PA0 = high */
            pin_state = 1;
        }
        
        TIM2->SR &= ~0x00000002;  /* Clear CC1 interrupt flag */
    }
}

int main(void)
{
    /* GPIO initialization for PA0 as output */
    GPIO_PinConfig led_config = {
        .pin = 0,
        .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_LOW,
        .pull = GPIO_PULL_NONE,
        .alternate = 0
    };
    GPIO_EnableClock(GPIOA);
    GPIO_Init(GPIOA, &led_config);
    
    /* Timer initialization */
    H7_TIM_Init(TIM2, 79, PWM_PERIOD, TIM_COUNTERMODE_UP);  // 80 MHz timer clock
    
    /* Configure channel 1 for output compare (not PWM mode) */
    TIM2->CCMR1 = (TIM2->CCMR1 & ~0xFF) | (0x00 << 0);  /* CC1S = 00 (output) */
    TIM2->CCMR1 = (TIM2->CCMR1 & ~(1 << 3)) | (0 << 3); /* OC1PE = 0 */
    TIM2->CCR1 = DUTY_CYCLE;  /* Compare value */
    
    /* Enable output on channel 1 */
    TIM2->CCER |= (1 << 0);  /* CC1E = 1 */
    
    /* Enable CC1 interrupt */
    H7_TIM_EnableInterrupt(TIM2, TIM_CC1_INTERRUPT);
    
    /* Enable the timer */
    H7_TIM_Start(TIM2);
    
    /* Main loop */
    while (1)
    {
        // Other tasks can be done here while PWM runs via interrupt
    }
    
    return 0;
}