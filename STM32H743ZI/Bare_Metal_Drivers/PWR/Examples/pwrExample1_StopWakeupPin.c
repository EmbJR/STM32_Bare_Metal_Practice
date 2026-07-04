/* =====================================================================
 *  PWR Example 1: Enter Stop mode and wake on a WKUP pin event.
 *
 *  Hardware setup (typical Nucleo-H743ZI):
 *      - WKUP1 pin (PA0) connected to the user button (B1, blue,
 *        active low).
 *
 *  Flow:
 *      1) Enable PWR clock.
 *      2) Select voltage scale VOS3 (1.0 V, 200 MHz max).
 *      3) Enable the low-power regulator (LPDS=1) for lowest Stop
 *         current.
 *      4) Enable WKUP1 on a falling edge (polarity_falling=1).
 *      5) Clear any pending Stop / Standby flags.
 *      6) Enter Stop mode.  CPU resumes here after the wake-up.
 *      7) Clear the wake-up flag, restore the application clock.
 * ===================================================================== */
#include "h7pwr.h"
#include "pwr_examples.h"

void PWR_Example1_StopWakeupPin(void)
{
    PWR_Examples_EnablePwrClock();
    PWR_Init();

    /* Use the low-power regulator while in Stop. */
    PWR_LowPowerRegulatorEnable(1);

    /* WKUP1 (PA0) on a falling edge, active-low button. */
    PWR_EnableWakeUpPin(0 /*WKUP1*/, 1 /*falling edge*/);

    /* Enter Stop mode 0 (full SRAM retention, fast wake-up). */
    PWR_EnterStopMode(PWR_STOP_0, 1);

    /* ----- CPU resumes here after the WKUP1 event ----- */
    if (PWR_GetWakeUpFlag(0)) { /* WKUP1 fired */ }
    PWR_ClearWakeUpFlag(0);
    PWR_LowPowerRegulatorEnable(0);

    while (1) { /* application continues */ }
}

int main(void)
{
    PWR_Example1_StopWakeupPin();
    return 0;
}
