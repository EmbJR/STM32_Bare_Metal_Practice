/*
 * Example: System clock = 4 MHz, source = CSI
 * ---------------------------------------------------------------
 *  The CSI (Low-power Internal oscillator) is a 4 MHz RC oscillator
 *  available on STM32H7. It is the only internal source that delivers
 *  exactly 4 MHz.
 *
 *  Clock tree:
 *      CSI (4 MHz) --> SYSCLK
 *
 *  All bus prescalers = /1.
 *  Flash latency = 0 wait states.
 */

#include "RCCH743ZI.h"

void SystemClock_4MHz_CSI(void)
{
    /* Enable CSI and wait for it to be ready */
    RCC_CSIEnable(1);
    while (!RCC_CSIReady()) { /* wait */ }

    /* Flash latency = 0 wait states */
    RCC_SetFlashLatency(0);

    /* All prescalers = /1 */
    RCC_SetD1CPRE(AHB_PRESCALER_DIV1);
    RCC_SetAHBPrescaler(AHB_PRESCALER_DIV1);
    RCC_SetD1PPRE(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE1(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE2(APB_PRESCALER_DIV1);
    RCC_SetD3PPRE(APB_PRESCALER_DIV1);

    /* Switch SYSCLK to CSI */
    RCC_SetSysClockSrc(RCC_SYSCLK_CSI);
}
