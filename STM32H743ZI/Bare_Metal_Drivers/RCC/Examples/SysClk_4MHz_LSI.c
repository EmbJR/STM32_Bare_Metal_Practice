/*
 * Example: System clock = 4 MHz, source = LSI
 * ---------------------------------------------------------------
 *  NOTE: The LSI is a 32 kHz internal RC oscillator and cannot
 *  produce 4 MHz on its own.  This example is provided only for
 *  completeness - the "system clock" will actually be running at
 *  32 kHz.  Use the HSE, HSI or CSI example for an actual 4 MHz
 *  system clock.
 *
 *  Clock tree:
 *      LSI (32 kHz) --> SYSCLK
 *
 *  All bus prescalers = /1.
 *  Flash latency = 0 wait states.
 */

#include "RCCH743ZI.h"

void SystemClock_4MHz_LSI(void)
{
    /* Enable LSI and wait for it to be ready */
    RCC_LSIEnable(1);
    while (!RCC_LSIRdy()) { /* wait */ }

    RCC_SetFlashLatency(0);

    RCC_SetD1CPRE(AHB_PRESCALER_DIV1);
    RCC_SetAHBPrescaler(AHB_PRESCALER_DIV1);
    RCC_SetD1PPRE(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE1(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE2(APB_PRESCALER_DIV1);
    RCC_SetD3PPRE(APB_PRESCALER_DIV1);

    /*
     * LSI is not selectable as SYSCLK on H7.  The system clock switch
     * (SW[2:0]) can only be HSI, CSI, HSE or PLL1.  Therefore there is
     * no clean way to run the CPU directly off LSI.  See the application
     * note for clock-tree limitations.
     */
    /* RCC_SetSysClockSrc(...);  -- not supported by HW */
}
