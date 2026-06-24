#include "RCCH743ZI.h"

/* =========================================================================
 *  PWR / Flash helpers
 * ========================================================================= */
static void pwr_unlock_backup_domain(void)
{
    SET_BIT(PWR_REG(PWR_CR1), PWR_CR1_DBP);
}

void RCC_SetFlashLatency(uint32_t wait_states)
{
    uint32_t reg = FLASH_REG(FLASH_ACR);
    reg &= ~FLASH_ACR_LATENCY_Msk;
    reg |= (wait_states << FLASH_ACR_LATENCY_Pos) & FLASH_ACR_LATENCY_Msk;
    FLASH_REG(FLASH_ACR) = reg;
}

/* =========================================================================
 *  LSE / LSI / RTC
 * ========================================================================= */
void RCC_LSEConfig(uint8_t enable, uint8_t bypass, uint8_t drive)
{
    uint32_t reg;
    pwr_unlock_backup_domain();

    reg = RCC_REG(RCC_BDCR);
    reg &= ~(RCC_BDCR_LSEBYP | RCC_BDCR_LSEDRV_Msk | RCC_BDCR_LSEON);

    if (bypass) reg |= RCC_BDCR_LSEBYP;
    reg |= ((uint32_t)drive << RCC_BDCR_LSEDRV_Pos) & RCC_BDCR_LSEDRV_Msk;
    if (enable) reg |= RCC_BDCR_LSEON;

    RCC_REG(RCC_BDCR) = reg;
}

uint8_t RCC_LSEReady(void)
{
    return (RCC_REG(RCC_BDCR) & RCC_BDCR_LSERDY) ? 1U : 0U;
}

void RCC_LSIEnable(uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_CSR), RCC_CSR_LSION);
    else         CLEAR_BIT(RCC_REG(RCC_CSR), RCC_CSR_LSION);
}

uint8_t RCC_LSIRdy(void)
{
    return (RCC_REG(RCC_CSR) & RCC_CSR_LSIRDY) ? 1U : 0U;
}

void RCC_RTCConfig(uint32_t rtcsel)
{
    pwr_unlock_backup_domain();
    uint32_t reg = RCC_REG(RCC_BDCR);
    reg &= ~RCC_BDCR_RTCSEL_Msk;
    reg |= (rtcsel & RCC_BDCR_RTCSEL_Msk);
    RCC_REG(RCC_BDCR) = reg;
}

void RCC_RTCEnable(uint8_t enable)
{
    pwr_unlock_backup_domain();
    if (enable) SET_BIT(RCC_REG(RCC_BDCR), RCC_BDCR_RTCEN);
    else         CLEAR_BIT(RCC_REG(RCC_BDCR), RCC_BDCR_RTCEN);
}

/* =========================================================================
 *  HSI
 * ========================================================================= */
void RCC_HSIEnable(uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_CR), RCC_CR_HSION);
    else         CLEAR_BIT(RCC_REG(RCC_CR), RCC_CR_HSION);
}

uint8_t RCC_HSIRdy(void)
{
    return (RCC_REG(RCC_CR) & RCC_CR_HSIRDY) ? 1U : 0U;
}

void RCC_HSIConfig(uint32_t div_code)
{
    uint32_t reg = RCC_REG(RCC_CR);
    reg &= ~RCC_CR_HSIDIV_Msk;
    reg |= (div_code << RCC_CR_HSIDIV_Pos) & RCC_CR_HSIDIV_Msk;
    RCC_REG(RCC_CR) = reg;

    /* wait for HSIDIVF to confirm new ratio is in effect */
    while ((RCC_REG(RCC_CR) & RCC_CR_HSIDIVF) == 0U) { /* spin */ }
}

/* =========================================================================
 *  CSI
 * ========================================================================= */
void RCC_CSIEnable(uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_CR), RCC_CR_CSION);
    else         CLEAR_BIT(RCC_REG(RCC_CR), RCC_CR_CSION);
}

uint8_t RCC_CSIRdy(void)
{
    return (RCC_REG(RCC_CR) & RCC_CR_CSIRDY) ? 1U : 0U;
}

/* =========================================================================
 *  HSE
 * ========================================================================= */
void RCC_HSEBypass(uint8_t bypass)
{
    if (bypass) SET_BIT(RCC_REG(RCC_CR), RCC_CR_HSEBYP);
    else         CLEAR_BIT(RCC_REG(RCC_CR), RCC_CR_HSEBYP);
}

void RCC_HSEEnable(uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_CR), RCC_CR_HSEON);
    else         CLEAR_BIT(RCC_REG(RCC_CR), RCC_CR_HSEON);
}

uint8_t RCC_HSERdy(void)
{
    return (RCC_REG(RCC_CR) & RCC_CR_HSERDY) ? 1U : 0U;
}

/* =========================================================================
 *  PLL1
 * ========================================================================= */
void RCC_PLL1Config(const RCC_PLL1_Init_t *cfg)
{
    /* 1) select PLL source and prescaler (PLL must be disabled) */
    uint32_t sel = RCC_REG(RCC_PLLCKSELR);
    sel &= ~(RCC_PLLCKSELR_PLLSRC_Msk |
             RCC_PLLCKSELR_DIVM1_Msk);
    sel |= (cfg->pll_src & RCC_PLLCKSELR_PLLSRC_Msk);
    sel |= ((cfg->divm << RCC_PLLCKSELR_DIVM1_Pos) & RCC_PLLCKSELR_DIVM1_Msk);
    RCC_REG(RCC_PLLCKSELR) = sel;

    /* 2) program PLL1 VCO settings (RGE / VCOSEL / FRACEN / output enables) */
    uint32_t cf = RCC_REG(RCC_PLLCFGR);
    cf &= ~(RCC_PLLCFGR_PLL1RGE_Msk |
            RCC_PLLCFGR_PLL1VCOSEL |
            RCC_PLLCFGR_PLL1FRACEN |
            RCC_PLLCFGR_DIVP1EN |
            RCC_PLLCFGR_DIVQ1EN |
            RCC_PLLCFGR_DIVR1EN);
    cf |= ((cfg->pll_rge    << RCC_PLLCFGR_PLL1RGE_Pos) & RCC_PLLCFGR_PLL1RGE_Msk);
    if (cfg->pll_vcosel)   cf |= RCC_PLLCFGR_PLL1VCOSEL;
    /* no fractional mode for our examples */
    if (cfg->enable_p)     cf |= RCC_PLLCFGR_DIVP1EN;
    if (cfg->enable_q)     cf |= RCC_PLLCFGR_DIVQ1EN;
    if (cfg->enable_r)     cf |= RCC_PLLCFGR_DIVR1EN;
    RCC_REG(RCC_PLLCFGR) = cf;

    /* 3) program PLL1 dividers (DIVN, DIVP, DIVQ, DIVR) */
    uint32_t d = 0;
    d |= ((cfg->divp << RCC_PLLDIVR_DIVP_Pos) & RCC_PLLDIVR_DIVP_Msk);
    d |= ((cfg->divq << RCC_PLLDIVR_DIVQ_Pos) & RCC_PLLDIVR_DIVQ_Msk);
    d |= ((cfg->divr << RCC_PLLDIVR_DIVR_Pos) & RCC_PLLDIVR_DIVR_Msk);
    d |= ((cfg->divn << RCC_PLLDIVR_DIVN_Pos) & RCC_PLLDIVR_DIVN_Msk);
    RCC_REG(RCC_PLL1DIVR) = d;

    /* 4) disable fractional latch */
    RCC_REG(RCC_PLL1FRACR) = 0U;
}

void RCC_PLL1Enable(uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_CR), RCC_CR_PLL1ON);
    else         CLEAR_BIT(RCC_REG(RCC_CR), RCC_CR_PLL1ON);
}

uint8_t RCC_PLL1Ready(void)
{
    return (RCC_REG(RCC_CR) & RCC_CR_PLL1RDY) ? 1U : 0U;
}

/* =========================================================================
 *  Bus prescalers
 * ========================================================================= */
void RCC_SetAHBPrescaler(uint32_t code)
{
    uint32_t r = RCC_REG(RCC_D1CFGR);
    r &= ~RCC_D1CFGR_HPRE_Msk;
    r |= (code << RCC_D1CFGR_HPRE_Pos) & RCC_D1CFGR_HPRE_Msk;
    RCC_REG(RCC_D1CFGR) = r;
}

void RCC_SetD1CPRE(uint32_t code)
{
    uint32_t r = RCC_REG(RCC_D1CFGR);
    r &= ~RCC_D1CFGR_D1CPRE_Msk;
    r |= (code << RCC_D1CFGR_D1CPRE_Pos) & RCC_D1CFGR_D1CPRE_Msk;
    RCC_REG(RCC_D1CFGR) = r;
}

void RCC_SetD1PPRE(uint32_t code)
{
    uint32_t r = RCC_REG(RCC_D1CFGR);
    r &= ~RCC_D1CFGR_D1PPRE_Msk;
    r |= (code << RCC_D1CFGR_D1PPRE_Pos) & RCC_D1CFGR_D1PPRE_Msk;
    RCC_REG(RCC_D1CFGR) = r;
}

void RCC_SetD2PPRE1(uint32_t code)
{
    uint32_t r = RCC_REG(RCC_D2CFGR);
    r &= ~RCC_D2CFGR_D2PPRE1_Msk;
    r |= (code << RCC_D2CFGR_D2PPRE1_Pos) & RCC_D2CFGR_D2PPRE1_Msk;
    RCC_REG(RCC_D2CFGR) = r;
}

void RCC_SetD2PPRE2(uint32_t code)
{
    uint32_t r = RCC_REG(RCC_D2CFGR);
    r &= ~RCC_D2CFGR_D2PPRE2_Msk;
    r |= (code << RCC_D2CFGR_D2PPRE2_Pos) & RCC_D2CFGR_D2PPRE2_Msk;
    RCC_REG(RCC_D2CFGR) = r;
}

void RCC_SetD3PPRE(uint32_t code)
{
    uint32_t r = RCC_REG(RCC_D3CFGR);
    r &= ~RCC_D3CFGR_D3PPRE_Msk;
    r |= (code << RCC_D3CFGR_D3PPRE_Pos) & RCC_D3CFGR_D3PPRE_Msk;
    RCC_REG(RCC_D3CFGR) = r;
}

/* =========================================================================
 *  System clock switch
 * ========================================================================= */
void RCC_SetSysClockSrc(RCC_SysClkSrc_t src)
{
    uint32_t r = RCC_REG(RCC_CFGR);
    r &= ~RCC_CFGR_SW_Msk;
    r |= ((uint32_t)src << RCC_CFGR_SW_Pos) & RCC_CFGR_SW_Msk;
    RCC_REG(RCC_CFGR) = r;

    /* wait until SWS reflects the new source */
    while (((RCC_REG(RCC_CFGR) & RCC_CFGR_SWS_Msk) >> RCC_CFGR_SWS_Pos) != (uint32_t)src)
    { /* spin */ }
}

RCC_SysClkSrc_t RCC_GetSysClockSrc(void)
{
    return (RCC_SysClkSrc_t)((RCC_REG(RCC_CFGR) & RCC_CFGR_SWS_Msk) >> RCC_CFGR_SWS_Pos);
}

/* =========================================================================
 *  MCO configuration
 * ========================================================================= */
void RCC_MCO1Config(uint32_t sel, uint32_t prescaler)
{
    uint32_t r = RCC_REG(RCC_CFGR);
    r &= ~(RCC_CFGR_MCO1_Msk | RCC_CFGR_MCO1PRE_Msk);
    r |= ((sel & 7U) << RCC_CFGR_MCO1_Pos);
    r |= ((prescaler & 0xFU) << RCC_CFGR_MCO1PRE_Pos);
    RCC_REG(RCC_CFGR) = r;
}

void RCC_MCO2Config(uint32_t sel, uint32_t prescaler)
{
    uint32_t r = RCC_REG(RCC_CFGR);
    r &= ~(RCC_CFGR_MCO2_Msk | RCC_CFGR_MCO2PRE_Msk);
    r |= ((sel & 7U) << RCC_CFGR_MCO2_Pos);
    r |= ((prescaler & 0xFU) << RCC_CFGR_MCO2PRE_Pos);
    RCC_REG(RCC_CFGR) = r;
}

/* =========================================================================
 *  Generic AHB / APB clock enable / reset
 * ========================================================================= */
void RCC_AHB1_ClkEnable(uint32_t mask, uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_AHB1ENR), mask);
    else         CLEAR_BIT(RCC_REG(RCC_AHB1ENR), mask);
}
void RCC_AHB1_ClkReset(uint32_t mask)
{
    SET_BIT(RCC_REG(RCC_AHB1RSTR), mask);
    CLEAR_BIT(RCC_REG(RCC_AHB1RSTR), mask);
}

void RCC_AHB2_ClkEnable(uint32_t mask, uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_AHB2ENR), mask);
    else         CLEAR_BIT(RCC_REG(RCC_AHB2ENR), mask);
}
void RCC_AHB2_ClkReset(uint32_t mask)
{
    SET_BIT(RCC_REG(RCC_AHB2RSTR), mask);
    CLEAR_BIT(RCC_REG(RCC_AHB2RSTR), mask);
}

void RCC_AHB3_ClkEnable(uint32_t mask, uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_AHB3ENR), mask);
    else         CLEAR_BIT(RCC_REG(RCC_AHB3ENR), mask);
}
void RCC_AHB3_ClkReset(uint32_t mask)
{
    SET_BIT(RCC_REG(RCC_AHB3RSTR), mask);
    CLEAR_BIT(RCC_REG(RCC_AHB3RSTR), mask);
}

void RCC_AHB4_ClkEnable(uint32_t mask, uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_AHB4ENR), mask);
    else         CLEAR_BIT(RCC_REG(RCC_AHB4ENR), mask);
}
void RCC_AHB4_ClkReset(uint32_t mask)
{
    SET_BIT(RCC_REG(RCC_AHB4RSTR), mask);
    CLEAR_BIT(RCC_REG(RCC_AHB4RSTR), mask);
}

void RCC_APB1L_ClkEnable(uint32_t mask, uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_APB1LENR), mask);
    else         CLEAR_BIT(RCC_REG(RCC_APB1LENR), mask);
}
void RCC_APB1L_ClkReset(uint32_t mask)
{
    SET_BIT(RCC_REG(RCC_APB1LRSTR), mask);
    CLEAR_BIT(RCC_REG(RCC_APB1LRSTR), mask);
}

void RCC_APB1H_ClkEnable(uint32_t mask, uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_APB1HENR), mask);
    else         CLEAR_BIT(RCC_REG(RCC_APB1HENR), mask);
}
void RCC_APB1H_ClkReset(uint32_t mask)
{
    SET_BIT(RCC_REG(RCC_APB1HRSTR), mask);
    CLEAR_BIT(RCC_REG(RCC_APB1HRSTR), mask);
}

void RCC_APB2_ClkEnable(uint32_t mask, uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_APB2ENR), mask);
    else         CLEAR_BIT(RCC_REG(RCC_APB2ENR), mask);
}
void RCC_APB2_ClkReset(uint32_t mask)
{
    SET_BIT(RCC_REG(RCC_APB2RSTR), mask);
    CLEAR_BIT(RCC_REG(RCC_APB2RSTR), mask);
}

void RCC_APB3_ClkEnable(uint32_t mask, uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_APB3ENR), mask);
    else         CLEAR_BIT(RCC_REG(RCC_APB3ENR), mask);
}
void RCC_APB3_ClkReset(uint32_t mask)
{
    SET_BIT(RCC_REG(RCC_APB3RSTR), mask);
    CLEAR_BIT(RCC_REG(RCC_APB3RSTR), mask);
}

void RCC_APB4_ClkEnable(uint32_t mask, uint8_t enable)
{
    if (enable) SET_BIT(RCC_REG(RCC_APB4ENR), mask);
    else         CLEAR_BIT(RCC_REG(RCC_APB4ENR), mask);
}
void RCC_APB4_ClkReset(uint32_t mask)
{
    SET_BIT(RCC_REG(RCC_APB4RSTR), mask);
    CLEAR_BIT(RCC_REG(RCC_APB4RSTR), mask);
}

/* =========================================================================
 *  Per-peripheral dispatch (RCC_Periph_t -> bus)
 *  Each peripheral in the enum is tagged with a small bit-band marker:
 *      bit 31 set  => use the AHB1..AHB4 / APB.. dispatch
 *  We use the numeric *value* of the enum (i.e. the (1U << n) bit) and
 *  compare against each bus' own "is-this-mine" check implemented in the
 *  function below.  The simplest robust approach: try each bus and accept
 *  the first that contains the bit.
 * ========================================================================= */
static uint8_t bit_in_mask(uint32_t mask, uint32_t bit)
{
    return (mask & bit) ? 1U : 0U;
}

void RCC_PeriphEnable(RCC_Periph_t p)
{
    uint32_t b = (uint32_t)p;

    if      (bit_in_mask(b, RCC_AHB1_DMA1))    RCC_AHB1_ClkEnable (b, 1);
    else if (bit_in_mask(b, RCC_AHB2_DCMI))    RCC_AHB2_ClkEnable (b, 1);
    else if (bit_in_mask(b, RCC_AHB3_MDMA))    RCC_AHB3_ClkEnable (b, 1);
    else if (bit_in_mask(b, RCC_AHB4_GPIOA))   RCC_AHB4_ClkEnable (b, 1);
    else if (bit_in_mask(b, RCC_APB1L_TIM2))   RCC_APB1L_ClkEnable(b, 1);
    else if (bit_in_mask(b, RCC_APB1H_CRS))    RCC_APB1H_ClkEnable(b, 1);
    else if (bit_in_mask(b, RCC_APB2_TIM1))    RCC_APB2_ClkEnable (b, 1);
    else if (bit_in_mask(b, RCC_APB3_LTDC))    RCC_APB3_ClkEnable (b, 1);
    else if (bit_in_mask(b, RCC_APB4_SYSCFG))  RCC_APB4_ClkEnable (b, 1);
}

void RCC_PeriphDisable(RCC_Periph_t p)
{
    uint32_t b = (uint32_t)p;

    if      (bit_in_mask(b, RCC_AHB1_DMA1))    RCC_AHB1_ClkEnable (b, 0);
    else if (bit_in_mask(b, RCC_AHB2_DCMI))    RCC_AHB2_ClkEnable (b, 0);
    else if (bit_in_mask(b, RCC_AHB3_MDMA))    RCC_AHB3_ClkEnable (b, 0);
    else if (bit_in_mask(b, RCC_AHB4_GPIOA))   RCC_AHB4_ClkEnable (b, 0);
    else if (bit_in_mask(b, RCC_APB1L_TIM2))   RCC_APB1L_ClkEnable(b, 0);
    else if (bit_in_mask(b, RCC_APB1H_CRS))    RCC_APB1H_ClkEnable(b, 0);
    else if (bit_in_mask(b, RCC_APB2_TIM1))    RCC_APB2_ClkEnable (b, 0);
    else if (bit_in_mask(b, RCC_APB3_LTDC))    RCC_APB3_ClkEnable (b, 0);
    else if (bit_in_mask(b, RCC_APB4_SYSCFG))  RCC_APB4_ClkEnable (b, 0);
}

void RCC_PeriphReset(RCC_Periph_t p)
{
    uint32_t b = (uint32_t)p;

    if      (bit_in_mask(b, RCC_AHB1_DMA1))    RCC_AHB1_ClkReset (b);
    else if (bit_in_mask(b, RCC_AHB2_DCMI))    RCC_AHB2_ClkReset (b);
    else if (bit_in_mask(b, RCC_AHB3_MDMA))    RCC_AHB3_ClkReset (b);
    else if (bit_in_mask(b, RCC_AHB4_GPIOA))   RCC_AHB4_ClkReset (b);
    else if (bit_in_mask(b, RCC_APB1L_TIM2))   RCC_APB1L_ClkReset(b);
    else if (bit_in_mask(b, RCC_APB1H_CRS))    RCC_APB1H_ClkReset(b);
    else if (bit_in_mask(b, RCC_APB2_TIM1))    RCC_APB2_ClkReset (b);
    else if (bit_in_mask(b, RCC_APB3_LTDC))    RCC_APB3_ClkReset (b);
    else if (bit_in_mask(b, RCC_APB4_SYSCFG))  RCC_APB4_ClkReset (b);
}
