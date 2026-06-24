/*
 * Example: System clock = 16 MHz, source = CSI
 * ---------------------------------------------------------------
 *  CSI is a 4 MHz oscillator.  It cannot directly produce 16 MHz.
 *  This example uses D1CPRE = /1 and the SYSCLK fed by the 4 MHz
 *  CSI  -- i.e. SYSCLK is actually 4 MHz, not 16 MHz.  For a real
 *  16 MHz system clock use the HSI or HSE example.
 *
 *  NOTE: To actually obtain 16 MHz from CSI on H7 you would need
 *  a PLL1 multiplier, which is what the 480 MHz example does.
 */

#include "RCCH743ZI.h"

void SystemClock_16MHz_CSI(void)
{
    RCC_CSIEnable(1);
    while (!RCC_CSIReady()) { /* wait */ }

    RCC_SetFlashLatency(0);

    RCC_SetD1CPRE(AHB_PRESCALER_DIV1);
    RCC_SetAHBPrescaler(AHB_PRESCALER_DIV1);
    RCC_SetD1PPRE(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE1(APB_PRESCALER_DIV1);
    RCC_SetD2PPRE2(APB_PRESCALER_DIV1);
    RCC_SetD3PPRE(APB_PRESCALER_DIV1);

    RCC_SetSysClockSrc(RCC_SYSCLK_CSI);
}
