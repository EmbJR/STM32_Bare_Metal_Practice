/*
 * Example: System clock = 4 MHz, source = HSI
 * ---------------------------------------------------------------
 *  Clock tree:
 *
 *      HSI (64 MHz)
 *         |
 *        HSIDIV = /8  --->  8 MHz
 *         |
 *        D1CPRE = /2  --->  4 MHz  -->  SYSCLK
 *
 *  NOTE: On STM32H743, the HSI oscillator is 64 MHz and the HSIDIV
 *  prescaler supports only /1, /2, /4 and /8, giving possible HSI
 *  frequencies of 64, 32, 16 and 8 MHz.  4 MHz cannot be obtained
 *  directly from the HSI block, so the system clock divider
 *  (D1CPRE) is used to halve the 8 MHz HSI output.
 *
 *  Bus prescalers are kept at /1 (HPRE, D1PPRE, D2PPRE1/2, D3PPRE = 1).
 *  Flash latency = 0 wait states (CPU clock <= 35 MHz at default VOS3).
 */

#include "RCCH743ZI.h"

void SystemClock_4MHz_HSI(void)
{
    /* HSI is ON by default after reset. Enable & wait for stability. */
    RCC_HSIEnable(1);
    while (!RCC_HSIRdy()) { /* wait */ }

    /* HSIDIV = /8  =>  hsi_ck = 8 MHz.
     * The helper takes the raw 2-bit field value: 3 = /8.
     */
    RCC_HSIConfig(3);

    /* Flash latency = 0 wait states */
    RCC_SetFlashLatency(0);

    /* All prescalers = /1 except D1CPRE which divides sys_ck by 2 */
    RCC_SetD1CPRE(AHB_PRESCALER_DIV2);
    RCC_SetAHBPrescaler(AHB_PRESCALER_DIV1);
    RCC_SetD1PPRE(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE1(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE2(APB_PRESCALER_DIV1);
    RCC_SetD3PPRE(APB_PRESCALER_DIV1);

    /* Select HSI as system clock source */
    RCC_SetSysClockSrc(RCC_SYSCLK_HSI);
}
