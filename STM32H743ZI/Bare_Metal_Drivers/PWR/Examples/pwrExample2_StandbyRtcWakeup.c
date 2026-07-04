/* =====================================================================
 *  PWR Example 2: Standby mode with RTC wake-up.
 *
 *  Assumes the user has already configured the RTC and an alarm.
 *
 *  Flow:
 *      1) Enable PWR clock, enable backup domain access (required
 *         before touching RTC / LSE / backup registers).
 *      2) Keep D3 supplied in Standby so the RTC keeps counting.
 *      3) Clear the standby flag, then enter Standby.
 *      4) On wake-up the CPU resets; the application should call
 *         PWR_GetStandbyFlag() and clear it, and (if needed)
 *         inspect RCC->RSR for the reset cause.
 * ===================================================================== */
#include "h7pwr.h"
#include "pwr_examples.h"

void PWR_Example2_StandbyRtcWakeup(void)
{
    PWR_Examples_EnablePwrClock();
    PWR_EnableBackupAccess();

    /* Standby with D3 retained (so backup domain / RTC keep state). */
    PWR_EnterStandbyMode(/*retain_d3=*/1);

    /* Not reached - execution restarts from Reset on RTC alarm. */
}

/* Provide main() in this TU so building only Example 2 gives a
 * complete program.                                              */
int main(void)
{
    PWR_Example2_StandbyRtcWakeup();
    return 0;
}
