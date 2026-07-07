/**
 * @file timerInputCaptureExample.c
 * @brief Example: Input capture using TIM3 to measure frequency.
 *
 * This example configures TIM3 to measure the frequency of a square wave input
 * on PB5 (TIM3_CH2). It demonstrates input capture configuration and interrupt handling.
 */

#include "h7timer.h"

/* Define TIM3 base address */
#define TIM3 ((TIM_TypeDef *)TIM3_BASE)

volatile uint32_t capture = 0;
volatile uint32_t count = 0;
volatile uint32_t frequency = 0;

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & 0x00000002)  /* CC2 interrupt flag */
    {
        capture = TIM3->CCR2;    /* Read captured value */
        count++;
        frequency = 84000000 / (capture + 1);  /* Calculate frequency (assuming 84 MHz timer clock) */
        TIM3->SR &= ~0x00000002;  /* Clear interrupt flag */
    }
}

int main(void)
{
    /* Initialize TIM3 for input capture on channel 2 */
    H7_TIM_Init(TIM3, 0, 0xFFFF, TIM_COUNTERMODE_UP);  // No prescaler, max period

    /* Configure channel 2 for input capture */
    H7_TIM_IC_ConfigChannel(TIM3, 2, 0x0F);  // Enable filter

    /* Enable CC2 interrupt */
    H7_TIM_EnableInterrupt(TIM3, TIM_CC2_INTERRUPT);

    /* Enable the timer */
    H7_TIM_Start(TIM3);

    /* Main loop */
    while (1)
    {
        // Frequency measurement results are updated in interrupt handler
    }

    return 0;
}