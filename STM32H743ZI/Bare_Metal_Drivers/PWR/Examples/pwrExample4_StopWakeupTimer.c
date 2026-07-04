/* =====================================================================
 *  PWR Example 4: Enter Stop mode and wake on a basic timer (TIM6).
 *
 *  TIM6 is the only basic timer that keeps clocking through Stop-0
 *  on H7.  This example focuses on the PWR side: it assumes the
 *  timer is configured to overflow every 1 s and that its update
 *  interrupt is enabled in the NVIC.
 * ===================================================================== */
#include "h7pwr.h"
#include "pwr_examples.h"

static void EnterStop(void)
{
    /* Use the main regulator for the fastest wake-up; the timer
     * keeps running, fires the IRQ, and the WFI returns.            */
    PWR_EnterStopMode(PWR_STOP_0, 0);
}

void PWR_Example4_StopWakeupTimer(void)
{
    PWR_Examples_EnablePwrClock();
    PWR_Init();

    PWR_SetVoltageScale(PWR_VOS_3);
    PWR_ClearStopFlag();
    PWR_ClearStandbyFlag();

    while (1)
    {
        EnterStop();
        /* TIM6 update IRQ has fired - do work, then back to Stop. */
    }
}

int main(void)
{
    PWR_Example4_StopWakeupTimer();
    return 0;
}
