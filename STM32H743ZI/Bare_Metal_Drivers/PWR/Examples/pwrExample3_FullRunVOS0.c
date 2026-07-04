/* =====================================================================
 *  PWR Example 3: Full Run mode, VOS0 + over-drive + booster,
 *  ready for a 480 MHz system clock.
 *
 *  Steps:
 *      1) Enable PWR clock.
 *      2) Select VOS0 (1.35 V).
 *      3) Enable over-drive (required for 480 MHz).
 *      4) Enable the core booster.
 *      5) Enable backup access (for RTC / LSE configuration).
 *      6) Application then configures the RCC clock tree.
 * ===================================================================== */
#include "h7pwr.h"
#include "pwr_examples.h"

void PWR_Example3_FullRunVOS0(void)
{
    PWR_Examples_EnablePwrClock();

    PWR_SetVoltageScale(PWR_VOS_0);
    PWR_OverDriveEnable(1);
    PWR_BoostEnable(1);
    PWR_EnableBackupAccess();

    if (PWR_RegulatorReady() == 0U) { /* VOS0 not ready; abort */ return; }
    if (PWR_GetVoltageScale() != PWR_VOS_0) { return; }

    while (1) { /* main loop */ }
}

int main(void)
{
    PWR_Example3_FullRunVOS0();
    return 0;
}
