/*
 * main.c - demonstration project entry point
 * ---------------------------------------------------------------
 *  Brings the system up to 480 MHz on HSE, enables a few peripheral
 *  clocks and sits in a tight loop.
 *
 *  This file is a complete, self-contained example intended to be
 *  built together with RCCH743ZI.c/.h.  No CMSIS, HAL or LL is used.
 *
 *  Build/link model:
 *      Startup_<board>.s   (vector table, Reset_Handler)
 *      RCCH743ZI.c         (clock + peripheral enable)
 *      main.c              (this file)
 */

#include "RCCH743ZI.h"

extern void SystemClock_480MHz_HSE(void);
extern void SystemClock_480MHz_HSI(void);
extern void PeripheralClockEnable_Example(void);

int main(void)
{
    /* 1) Configure the system clock to 480 MHz from HSE */
    SystemClock_480MHz_HSI();

    /* 2) Enable the peripherals used by the application */
    PeripheralClockEnable_Example();

    /* 3) Application code goes here ... */

    while (1) { __asm__ volatile("wfi"); }
    return 0;
}
