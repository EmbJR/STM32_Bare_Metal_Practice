/**
 * @file timerInputCaptureInterruptExample.c
 * @brief Example: Input capture with interrupt to measure pulse width.
 *
 * This example demonstrates how to use timer input capture with interrupts
 * to measure the width of incoming pulses on PB5 (TIM3_CH2). The measured
 * pulse width is used to calculate frequency or duty cycle.
 */

#include "h7timer.h"
#include "Bare_Metal_Drivers/GPIO/GPIOH743ZI.h"

/* Timer and GPIO definitions */
#define TIM3 ((TIM_TypeDef *)TIM3_BASE)
#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)

volatile uint32_t last_captured = 0;
volatile uint32_t pulse_width = 0;
volatile uint8_t capture_done = 0;

/* TIM3 interrupt handler */
void TIM3_IRQHandler(void)
{
    static uint32_t first_capture = 0;
    static uint32_t second_capture = 0;
    
    if (TIM3->SR & 0x00000002)  /* CC2 interrupt flag */
    {
        if (!first_capture)
        {
            first_capture = TIM3->CCR2;
            capture_done = 0;
        }
        else
        {
            second_capture = TIM3->CCR2;
            if (second_capture > first_capture)
            {
                pulse_width = second_capture - first_capture;
            }
            else
            {
                /* Handle counter overflow */
                pulse_width = (0xFFFF - first_capture) + second_capture;
            }
            first_capture = second_capture;
            capture_done = 1;
        }
        
        TIM3->SR &= ~0x00000002;  /* Clear CC2 interrupt flag */
    }
}

int main(void)
{
    /* GPIO initialization for PB5 as alternate function input */
    GPIO_PinConfig af_input_config = {
        .pin = 5,                    /* PB5 */
        .mode = GPIO_MODE_ALTERNATE, /* Alternate function mode */
        .otype = GPIO_OTYPE_PUSHPULL, /* Push-pull */
        .speed = GPIO_SPEED_VERYHIGH, /* Very high speed */
        .pull = GPIO_PULL_DOWN,      /* Pull-down for noise immunity */
        .alternate = 2               /* AF2 for TIM3_CH2 on PB5 */
    };
    GPIO_EnableClock(GPIOB);
    GPIO_Init(GPIOB, &af_input_config);
    
    /* Timer initialization for input capture */
    H7_TIM_Init(TIM3, 0, 0xFFFF, TIM_COUNTERMODE_UP);  // No prescaler, max period
    
    /* Configure channel 2 for input capture */
    H7_TIM_IC_ConfigChannel(TIM3, 2, 0x0F);  // Filter = 15 for debouncing
    
    /* Enable CC2 interrupt */
    H7_TIM_EnableInterrupt(TIM3, TIM_CC2_INTERRUPT);
    
    /* Enable the timer */
    H7_TIM_Start(TIM3);
    
    /* Main loop */
    while (1)
    {
        if (capture_done)
        {
            /* Process pulse width measurement */
            // frequency = timer_clock / pulse_width;
            // duty_cycle = (pulse_width / period) * 100;
            
            capture_done = 0;
        }
    }
    
    return 0;
}