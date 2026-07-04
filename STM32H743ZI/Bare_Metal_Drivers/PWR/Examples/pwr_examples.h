#ifndef PWR_EXAMPLES_H
#define PWR_EXAMPLES_H

#include <stdint.h>

/* Single source of truth for the PWR peripheral clock-enable
 * boilerplate.  Lives in pwr_examples.c.                          */
void     PWR_Examples_EnablePwrClock(void);

/* Each example is a complete, self-contained demo. */
void     PWR_Example1_StopWakeupPin   (void);
void     PWR_Example2_StandbyRtcWakeup(void);
void     PWR_Example3_FullRunVOS0     (void);
void     PWR_Example4_StopWakeupTimer (void);

#endif /* PWR_EXAMPLES_H */
