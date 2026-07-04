/*
 * Example: System clock = 16 MHz, source = HSE
 * ---------------------------------------------------------------
 *  Assumes a 16 MHz external crystal / clock on OSC_IN.
 *
 *  Clock tree:
 *      HSE (16 MHz) --> SYSCLK
 *
 *  All bus prescalers = /1.
 *  Flash latency = 0 wait states.
 */

#include "RCCH743ZI.h"

void SystemClock_16MHz_HSE(void)
{
    RCC_HSEEnable(1);
    while (!RCC_HSERdy()) { /* wait */ }

    RCC_SetFlashLatency(0);

    RCC_SetD1CPRE(AHB_PRESCALER_DIV1);
    RCC_SetAHBPrescaler(AHB_PRESCALER_DIV1);
    RCC_SetD1PPRE(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE1(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE2(APB_PRESCALER_DIV1);
    RCC_SetD3PPRE(APB_PRESCALER_DIV1);

    RCC_SetSysClockSrc(RCC_SYSCLK_HSE);
}
