/*
 * Example: System clock = 16 MHz, source = LSI
 * ---------------------------------------------------------------
 *  The LSI is a 32 kHz RC oscillator.  It cannot be selected as
 *  SYSCLK on H7 and it cannot reach 16 MHz.  This example is
 *  provided only for completeness - the CPU is left running on the
 *  reset default (HSI 64 MHz / 16 = 4 MHz) because the requested
 *  configuration is not achievable.
 */

#include "RCCH743ZI.h"

void SystemClock_16MHz_LSI(void)
{
    RCC_LSIEnable(1);
    while (!RCC_LSIRdy()) { /* wait */ }

    /* LSI is not a valid SYSCLK source on H7. */
    /* The CPU therefore continues to run on HSI. */
}
