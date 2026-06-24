/*
 * Example: System clock = 16 MHz, source = HSI
 * ---------------------------------------------------------------
 *  HSI on H7 is 64 MHz; HSIDIV supports /1, /2, /4 and /8.
 *  HSIDIV = /4 gives 16 MHz hsi_ck.
 *
 *  Clock tree:
 *      HSI (64 MHz) --HSIDIV/4--> 16 MHz --> SYSCLK
 *
 *  All bus prescalers = /1.
 *  Flash latency = 0 wait states (CPU clock <= 35 MHz at VOS3).
 */

#include "RCCH743ZI.h"

void SystemClock_16MHz_HSI(void)
{
    RCC_HSIEnable(1);
    while (!RCC_HSIReady()) { /* wait */ }

    /* HSIDIV = /4  =>  hsi_ck = 16 MHz */
    RCC_HSIConfig(2);          /* 2 = divide by 4 */

    RCC_SetFlashLatency(0);

    RCC_SetD1CPRE(AHB_PRESCALER_DIV1);
    RCC_SetAHBPrescaler(AHB_PRESCALER_DIV1);
    RCC_SetD1PPRE(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE1(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE2(APB_PRESCALER_DIV1);
    RCC_SetD3PPRE(APB_PRESCALER_DIV1);

    RCC_SetSysClockSrc(RCC_SYSCLK_HSI);
}
