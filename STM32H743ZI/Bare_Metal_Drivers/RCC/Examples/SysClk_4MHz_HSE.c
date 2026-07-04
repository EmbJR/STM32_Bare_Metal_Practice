/*
 * Example: System clock = 4 MHz, source = HSE
 * ---------------------------------------------------------------
 *  Assumes a 4 MHz external crystal or HSE bypass clock is present
 *  on OSC_IN/OSC_OUT.
 *
 *  Clock tree:
 *      HSE (4 MHz)  -->  SYSCLK
 *
 *  All bus prescalers = /1.
 *  Flash latency = 0 wait states.
 */

#include "RCCH743ZI.h"

void SystemClock_4MHz_HSE(void)
{
    /* Enable HSE and wait for it to be ready */
    RCC_HSEEnable(1);
    while (!RCC_HSERdy()) { /* wait */ }

    /* Flash latency = 0 wait states */
    RCC_SetFlashLatency(0);

    /* All prescalers = /1 */
    RCC_SetD1CPRE(AHB_PRESCALER_DIV1);
    RCC_SetAHBPrescaler(AHB_PRESCALER_DIV1);
    RCC_SetD1PPRE(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE1(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE2(APB_PRESCALER_DIV1);
    RCC_SetD3PPRE(APB_PRESCALER_DIV1);

    /* Switch SYSCLK to HSE */
    RCC_SetSysClockSrc(RCC_SYSCLK_HSE);
}
