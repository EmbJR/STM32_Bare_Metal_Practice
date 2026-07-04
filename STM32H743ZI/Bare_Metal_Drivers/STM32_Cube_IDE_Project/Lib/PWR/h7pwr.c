#include "h7pwr.h"

/* =====================================================================
 *  Cortex-M7 System Control Space (no CMSIS dependency).
 *  SCR is at SCB + 0x10, used for SLEEPDEEP / SLEEPONEXIT.
 * ===================================================================== */
#define SCB_BASE                  0xE000ED00UL
typedef struct
{
    volatile uint32_t RESERVED0[4];
    volatile uint32_t SCR;
} SCB_Slim_TypeDef;
#define SCB                       ((SCB_Slim_TypeDef *)SCB_BASE)

/* SYSCFG is used only for VOS0 (VCORE boost).  RM0433 states that VOS0 is
 * selected with SYSCFG_PWRCR.ODEN after PWR_D3CR.VOS is set to VOS1. */
#define RCC_BASE                  0x58024400UL
#define RCC_APB4ENR_OFFSET        0xF4U
#define RCC_APB4ENR_SYSCFGEN      (1U << 1)
#define RCC_REG(off)              (*(volatile uint32_t *)(RCC_BASE + (off)))

#define SYSCFG_BASE               0x58000400UL
#define SYSCFG_PWRCR_OFFSET       0x2CU
#define SYSCFG_PWRCR_ODEN         (1U << 0)
#define SYSCFG_REG(off)           (*(volatile uint32_t *)(SYSCFG_BASE + (off)))

/* ARM architecture memory barriers (no CMSIS dependency). */
#ifndef __DSB
#define __DSB()  __asm__ volatile("dsb 0xF" ::: "memory")
#endif
#ifndef __ISB
#define __ISB()  __asm__ volatile("isb 0xF" ::: "memory")
#endif

/* Bound for the regulator-ready and over-drive-ready polls.
 * 20000 iterations of an LDR is ~1 ms at 64 MHz, more than enough
 * for VOSRDY (typ <100 us) and ODEN settling. */
#define PWR_POLL_ITER_MAX         20000U

/* =====================================================================
 *  Internal helpers
 * ===================================================================== */
static void pwr_wait_for_interrupt(void)
{
    __DSB();
    __ISB();
    __asm__ volatile("wfi");
}

static uint8_t pwr_cpucr_flag_set(uint32_t mask)
{
    return (PWR_REG(PWR_CPUCR_OFFSET) & mask) ? 1U : 0U;
}

/* =====================================================================
 *  Init / de-init
 * ===================================================================== */
void PWR_Init(void)
{
    PWR_SetVoltageScale(PWR_VOS_1);
    PWR_EnableBackupAccess();
}

void PWR_DeInit(void)
{
    PWR_SetVoltageScale(PWR_VOS_3);

    for (uint8_t p = 0; p < 6U; p++)
    {
        PWR_DisableWakeUpPin(p);
        PWR_ClearWakeUpFlag(p);
    }

    PWR_ClearStopFlag();
    PWR_ClearStandbyFlag();
    PWR_DisableBackupAccess();
}

/* =====================================================================
 *  Voltage scaling
 *
 *  VOS1..VOS3 live in PWR_D3CR[15:14] on H743. VOS0 uses VOS1 plus
 *  SYSCFG_PWRCR.ODEN. The system clock must not exceed the new scale's
 *  max frequency. Polls PWR_D3CR.VOSRDY with a bounded loop.
 * ===================================================================== */
void PWR_SetVoltageScale(PWR_VOS_t scale)
{
    uint32_t run_vos = (uint32_t)scale;

    if (scale == PWR_VOS_0)
    {
        run_vos = (uint32_t)PWR_VOS_1;
    }
    else if ((run_vos < (uint32_t)PWR_VOS_3) || (run_vos > (uint32_t)PWR_VOS_1))
    {
        return;
    }

    if (scale != PWR_VOS_0)
    {
        PWR_OverDriveEnable(0U);
    }

    if ((run_vos == ((PWR_REG(PWR_D3CR_OFFSET) & PWR_D3CR_VOS_Msk) >> PWR_D3CR_VOS_Pos)) &&
        ((scale != PWR_VOS_0) || (SYSCFG_REG(SYSCFG_PWRCR_OFFSET) & SYSCFG_PWRCR_ODEN)))
    {
        return;
    }

    /* Stop-mode voltage scaling uses SVOS3..SVOS5, not the run-mode VOS field.
     * Keep the preselected Stop scale at least as capable as the requested run scale. */
    uint32_t cr1 = PWR_REG(PWR_CR1_OFFSET);
    cr1 &= ~PWR_CR1_SVOS_Msk;
    if (run_vos == (uint32_t)PWR_VOS_3)      { cr1 |= PWR_CR1_SVOS_5; }
    else if (run_vos == (uint32_t)PWR_VOS_2) { cr1 |= PWR_CR1_SVOS_4; }
    else                                     { cr1 |= PWR_CR1_SVOS_3; }
    PWR_REG(PWR_CR1_OFFSET) = cr1;

    /* Now program the actual VOS field in D3CR. */
    uint32_t d3 = PWR_REG(PWR_D3CR_OFFSET);
    d3 &= ~PWR_D3CR_VOS_Msk;
    d3 |= (run_vos << PWR_D3CR_VOS_Pos) & PWR_D3CR_VOS_Msk;
    PWR_REG(PWR_D3CR_OFFSET) = d3;

    /* Bounded wait for the hardware to finish the transition. */
    for (uint32_t i = 0; i < PWR_POLL_ITER_MAX; i++)
    {
        if (PWR_REG(PWR_D3CR_OFFSET) & PWR_D3CR_VOSRDY) { break; }
    }

    if (scale == PWR_VOS_0)
    {
        PWR_OverDriveEnable(1U);
    }
}

PWR_VOS_t PWR_GetVoltageScale(void)
{
    if (SYSCFG_REG(SYSCFG_PWRCR_OFFSET) & SYSCFG_PWRCR_ODEN)
    {
        return PWR_VOS_0;
    }

    uint32_t vos = (PWR_REG(PWR_D3CR_OFFSET) & PWR_D3CR_VOS_Msk) >> PWR_D3CR_VOS_Pos;
    if (vos == (PWR_D3CR_VOS_RESERVED >> PWR_D3CR_VOS_Pos))
    {
        return PWR_VOS_3;
    }
    return (PWR_VOS_t)vos;
}

uint8_t PWR_RegulatorReady(void)
{
    return (PWR_REG(PWR_D3CR_OFFSET) & PWR_D3CR_VOSRDY) ? 1U : 0U;
}

/* =====================================================================
 *  Low-power mode entry
 * ===================================================================== */

void PWR_EnterSleepMode(uint8_t sleep_on_exit)
{
    SCB->SCR &= ~0x04U;                       /* clear SLEEPDEEP  */
    if (sleep_on_exit) SCB->SCR |=  0x02U;    /* set SLEEPONEXIT   */
    else               SCB->SCR &= ~0x02U;
    pwr_wait_for_interrupt();
}

void PWR_EnterStopMode(PWR_StopLevel_t level, uint8_t low_power_regulator)
{
    /* Stop mode on H743: clear all PDDS bits in CPUCR.  The
     * "Stop level" parameter is accepted for API compatibility
     * (Stop 0/1/2 correspond to different voltage ranges on some
     * parts; on H743 it is mainly a hint to the caller to pick the
     * right VOS in advance).  The Stop mode is entered via the
     * Cortex-M7 SLEEPDEEP bit.                                    */
    (void)level;
    uint32_t cpucr = PWR_REG(PWR_CPUCR_OFFSET);
    cpucr &= ~PWR_CPUCR_PDDS;
    PWR_REG(PWR_CPUCR_OFFSET) = cpucr;

    /* Low-power regulator selection (LPDS in CR1). */
    if (low_power_regulator) SET_BIT  (PWR_REG(PWR_CR1_OFFSET), PWR_CR1_LPDS);
    else                     CLEAR_BIT(PWR_REG(PWR_CR1_OFFSET), PWR_CR1_LPDS);

    SCB->SCR |= 0x04U;
    pwr_wait_for_interrupt();
    SCB->SCR &= ~0x04U;
}

void PWR_EnterStandbyMode(uint8_t retain_d3)
{
    /* Clear any pending wake-up flag so a fresh event can be seen. */
    PWR_REG(PWR_WKUPCR_OFFSET) = 0x3FU;

    /* Standby entry: PDDS_D1 | PDDS_D2 set unconditionally (D1 and
     * D2 lose power).  PDDS_D3 is set only if the caller does NOT
     * want to retain D3 - if retain_d3 is 1, D3 stays powered
     * which is what the RTC and the backup domain need to keep
     * running.                                                     */
    uint32_t cpucr = PWR_REG(PWR_CPUCR_OFFSET);
    cpucr &= ~PWR_CPUCR_PDDS;
    cpucr |=  PWR_CPUCR_PDDS_D1 | PWR_CPUCR_PDDS_D2;
    if (!retain_d3) cpucr |=  PWR_CPUCR_PDDS_D3;
    else            cpucr &= ~PWR_CPUCR_PDDS_D3;
    PWR_REG(PWR_CPUCR_OFFSET) = cpucr;

    SCB->SCR |= 0x04U;     /* SLEEPDEEP */
    pwr_wait_for_interrupt();
    /* The CPU resets on wake-up from Standby. */
}

/* =====================================================================
 *  Wake-up pins
 *  Polarities follow the ST convention: 0 = rising, 1 = falling.
 *  Enable / polarity / pull all live in PWR_WKUPEPR.
 * ===================================================================== */
void PWR_EnableWakeUpPin(uint8_t pin, uint8_t polarity_falling)
{
    if (pin > 5U) return;

    uint32_t mask_en  = 1U << pin;
    uint32_t mask_pol = 1U << (pin + 8U);
    uint32_t reg = PWR_REG(PWR_WKUPEPR_OFFSET);
    reg |= mask_en;
    if (polarity_falling) reg |=  mask_pol;
    else                  reg &= ~mask_pol;
    PWR_REG(PWR_WKUPEPR_OFFSET) = reg;

    PWR_ClearWakeUpFlag(pin);
}

void PWR_DisableWakeUpPin(uint8_t pin)
{
    if (pin > 5U) return;
    PWR_REG(PWR_WKUPEPR_OFFSET) &= ~(1U << pin);
}

uint8_t PWR_GetWakeUpFlag(uint8_t pin)
{
    if (pin > 5U) return 0U;
    return (PWR_REG(PWR_WKUPFR_OFFSET) & (1U << pin)) ? 1U : 0U;
}

void PWR_ClearWakeUpFlag(uint8_t pin)
{
    if (pin > 5U) return;
    PWR_REG(PWR_WKUPCR_OFFSET) = (1U << pin);
}

/* =====================================================================
 *  Backup domain access
 * ===================================================================== */
void PWR_EnableBackupAccess(void)
{
    SET_BIT(PWR_REG(PWR_CR1_OFFSET), PWR_CR1_DBP);
}

void PWR_DisableBackupAccess(void)
{
    CLEAR_BIT(PWR_REG(PWR_CR1_OFFSET), PWR_CR1_DBP);
}

/* =====================================================================
 *  Regulator control
 * ===================================================================== */
void PWR_LowPowerRegulatorEnable(uint8_t enable)
{
    if (enable) SET_BIT  (PWR_REG(PWR_CR1_OFFSET), PWR_CR1_LPDS);
    else        CLEAR_BIT(PWR_REG(PWR_CR1_OFFSET), PWR_CR1_LPDS);
}

void PWR_BackupRegulatorEnable(uint8_t enable)
{
    if (enable)
    {
        PWR_EnableBackupAccess();
        SET_BIT(PWR_REG(PWR_CR2_OFFSET), PWR_CR2_BREN);
    }
    else
    {
        CLEAR_BIT(PWR_REG(PWR_CR2_OFFSET), PWR_CR2_BREN);
    }
}

void PWR_OverDriveEnable(uint8_t enable)
{
    SET_BIT(RCC_REG(RCC_APB4ENR_OFFSET), RCC_APB4ENR_SYSCFGEN);

    if (!enable)
    {
        CLEAR_BIT(SYSCFG_REG(SYSCFG_PWRCR_OFFSET), SYSCFG_PWRCR_ODEN);
        return;
    }

    uint32_t d3 = PWR_REG(PWR_D3CR_OFFSET);
    d3 &= ~PWR_D3CR_VOS_Msk;
    d3 |= PWR_D3CR_VOS_1;
    PWR_REG(PWR_D3CR_OFFSET) = d3;

    SET_BIT(SYSCFG_REG(SYSCFG_PWRCR_OFFSET), SYSCFG_PWRCR_ODEN);
    for (uint32_t i = 0; i < PWR_POLL_ITER_MAX; i++)
    {
        if (PWR_REG(PWR_D3CR_OFFSET) & PWR_D3CR_VOSRDY) { return; }
    }
}

void PWR_BoostEnable(uint8_t enable)
{
    PWR_OverDriveEnable(enable);
}

/* =====================================================================
 *  PVD
 *  PVDE and PLS[2:0] live in PWR_CR1; PVDO is in PWR_CSR1 (status
 *  register at offset 0x04).
 * ===================================================================== */
void PWR_PVDEnable(uint8_t enable)
{
    if (enable) SET_BIT  (PWR_REG(PWR_CR1_OFFSET), PWR_CR1_PVDE);
    else        CLEAR_BIT(PWR_REG(PWR_CR1_OFFSET), PWR_CR1_PVDE);
}

void PWR_PVDSetLevel(uint8_t pls)
{
    uint32_t reg = PWR_REG(PWR_CR1_OFFSET);
    reg &= ~PWR_CR1_PLS_Msk;
    reg |= ((uint32_t)(pls & 7U) << PWR_CR1_PLS_Pos) & PWR_CR1_PLS_Msk;
    PWR_REG(PWR_CR1_OFFSET) = reg;
}

uint8_t PWR_PVDStatus(void)
{
    return (PWR_REG(PWR_CSR1_OFFSET) & PWR_CSR1_PVDO) ? 1U : 0U;
}

/* =====================================================================
 *  Status / flags
 *  SBF and STOPF live in PWR_CPUCR; CSSF clears them (write 1).
 * ===================================================================== */
uint8_t PWR_GetStandbyFlag(void) { return pwr_cpucr_flag_set(PWR_CPUCR_SBF); }
uint8_t PWR_GetStopFlag   (void) { return pwr_cpucr_flag_set(PWR_CPUCR_STOPF); }

void PWR_ClearStandbyFlag(void) { PWR_REG(PWR_CPUCR_OFFSET) = PWR_CPUCR_CSSF; }
void PWR_ClearStopFlag   (void) { PWR_REG(PWR_CPUCR_OFFSET) = PWR_CPUCR_CSSF; }
