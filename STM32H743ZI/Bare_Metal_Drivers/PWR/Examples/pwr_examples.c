#include "h7pwr.h"
#include "pwr_examples.h"

/* =====================================================================
 *  Shared PWR peripheral clock enable.  All four examples need this;
 *  the address / bit position live here, in one place, so the
 *  examples cannot drift out of sync.
 * ===================================================================== */
#define RCC_BASE_ADDR        0x58024400UL
#define RCC_APB4ENR_REG      (*(volatile uint32_t *)(RCC_BASE_ADDR + 0xF4U))
#define RCC_APB4ENR_PWREN    (1U << 4)

void PWR_Examples_EnablePwrClock(void)
{
    RCC_APB4ENR_REG |= RCC_APB4ENR_PWREN;
    /* Allow clock to propagate through the bus matrix. */
    __asm__ volatile("nop; nop; nop");
}
