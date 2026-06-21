#include "RCCH743ZI.h"

#define TIMEOUT_MAX             0xFFFFU

static volatile uint32_t s_delay_counter = 0;

static void delay(volatile uint32_t cycles) {
    while (cycles--) {
        __asm__ volatile ("nop");
    }
}

void RCC_EnableHSI(void) {
    RCC_CR |= CR_HSION;
}

void RCC_DisableHSI(void) {
    RCC_CR &= ~CR_HSION;
}

uint32_t RCC_IsHSIReady(void) {
    return (RCC_CR & CR_HSIRDY) != 0;
}

void RCC_EnableCSI(void) {
    RCC_CR |= CR_CSION;
}

void RCC_DisableCSI(void) {
    RCC_CR &= ~CR_CSION;
}

uint32_t RCC_IsCSIReady(void) {
    return (RCC_CR & CR_CSIRDY) != 0;
}

void RCC_EnableHSE(void) {
    RCC_CR |= CR_HSEON;
}

void RCC_DisableHSE(void) {
    RCC_CR &= ~CR_HSEON;
}

uint32_t RCC_IsHSEReady(void) {
    return (RCC_CR & CR_HSERDY) != 0;
}

void RCC_EnableLSI(void) {
    RCC_CSR |= CSR_LSION;
}

void RCC_DisableLSI(void) {
    RCC_CSR &= ~CSR_LSION;
}

uint32_t RCC_IsLSIReady(void) {
    return (RCC_CSR & CSR_LSIRDY) != 0;
}

void RCC_EnableLSE(void) {
    RCC_BDCR |= BDCR_LSEON;
}

void RCC_DisableLSE(void) {
    RCC_BDCR &= ~BDCR_LSEON;
}

uint32_t RCC_IsLSEReady(void) {
    return (RCC_BDCR & BDCR_LSERDY) != 0;
}

void RCC_EnableHSI48(void) {
    RCC_CR |= CR_HSI48ON;
}

void RCC_DisableHSI48(void) {
    RCC_CR &= ~CR_HSI48ON;
}

uint32_t RCC_IsHSI48Ready(void) {
    return (RCC_CR & CR_HSI48RDY) != 0;
}

void RCC_EnablePLL1(void) {
    RCC_CR |= CR_PLL1ON;
}

void RCC_DisablePLL1(void) {
    RCC_CR &= ~CR_PLL1ON;
}

uint32_t RCC_IsPLL1Ready(void) {
    return (RCC_CR & CR_PLL1RDY) != 0;
}

void RCC_EnablePLL2(void) {
    RCC_CR |= CR_PLL2ON;
}

void RCC_DisablePLL2(void) {
    RCC_CR &= ~CR_PLL2ON;
}

uint32_t RCC_IsPLL2Ready(void) {
    return (RCC_CR & CR_PLL2RDY) != 0;
}

void RCC_EnablePLL3(void) {
    RCC_CR |= CR_PLL3ON;
}

void RCC_DisablePLL3(void) {
    RCC_CR &= ~CR_PLL3ON;
}

uint32_t RCC_IsPLL3Ready(void) {
    return (RCC_CR & CR_PLL3RDY) != 0;
}

void RCC_ConfigurePLL1(const PLL_Config *config) {
    uint32_t reg = RCC_PLLCFGR;
    
    reg &= ~(PLLCFGR_PLLSRC_MASK << PLLCFGR_PLLSRC_Pos);
    reg |= (config->pll_source & PLLCFGR_PLLSRC_MASK) << PLLCFGR_PLLSRC_Pos;
    
    reg &= ~(PLLCFGR_PLL1M_MASK << PLLCFGR_PLL1M_Pos);
    reg |= (config->pll_m & PLLCFGR_PLL1M_MASK) << PLLCFGR_PLL1M_Pos;
    
    reg &= ~(PLLCFGR_PLL1N_MASK << PLLCFGR_PLL1N_Pos);
    reg |= (config->pll_n & PLLCFGR_PLL1N_MASK) << PLLCFGR_PLL1N_Pos;
    
    reg &= ~(PLLCFGR_PLL1P_MASK << PLLCFGR_PLL1P_Pos);
    reg |= (((config->pll_p >> 1) - 1) & PLLCFGR_PLL1P_MASK) << PLLCFGR_PLL1P_Pos;
    
    reg &= ~(0xFU << PLLCFGR_PLL1Q_Pos);
    reg |= ((config->pll_q & 0xFU) << PLLCFGR_PLL1Q_Pos);
    
    reg &= ~(0x7U << PLLCFGR_PLL1R_Pos);
    reg |= ((config->pll_r & 0x7U) << PLLCFGR_PLL1R_Pos);
    
    RCC_PLLCFGR = reg;
}

void RCC_ConfigurePLL2(const PLL_Config *config) {
    uint32_t reg = RCC_PLLCFGR;
    
    reg &= ~(0x3FU << 4);
    reg |= (config->pll_m & 0x3FU) << 4;
    
    reg &= ~(0x1FFU << 8);
    reg |= (config->pll_n & 0x1FFU) << 8;
    
    reg &= ~(0x7FU << 16);
    reg |= (((config->pll_p >> 1) - 1) & 0x7FU) << 16;
    
    reg &= ~(0xFU << 20);
    reg |= (config->pll_q & 0xFU) << 20;
    
    reg &= ~(0x7U << 24);
    reg |= (config->pll_r & 0x7U) << 24;
    
    RCC_PLLCFGR = reg;
}

void RCC_ConfigurePLL3(const PLL_Config *config) {
    uint32_t reg = RCC_PLLCFGR;
    
    reg &= ~(0x3FU << 4);
    reg |= (config->pll_m & 0x3FU) << 4;
    
    reg &= ~(0x1FFU << 8);
    reg |= (config->pll_n & 0x1FFU) << 8;
    
    reg &= ~(0x7FU << 16);
    reg |= (((config->pll_p >> 1) - 1) & 0x7FU) << 16;
    
    reg &= ~(0xFU << 20);
    reg |= (config->pll_q & 0xFU) << 20;
    
    reg &= ~(0x7U << 24);
    reg |= (config->pll_r & 0x7U) << 24;
    
    RCC_PLLCFGR = reg;
}

void RCC_SetSystemClock(SystemClockSource source) {
    uint32_t reg = RCC_CFGR;
    reg &= ~(CFGR_SW_MASK << CFGR_SW_Pos);
    reg |= (source & CFGR_SW_MASK) << CFGR_SW_Pos;
    RCC_CFGR = reg;
    
    while (((RCC_CFGR >> CFGR_SWS_Pos) & CFGR_SWS_MASK) != source) {
    }
}

SystemClockSource RCC_GetSystemClockSource(void) {
    return (SystemClockSource)((RCC_CFGR >> CFGR_SWS_Pos) & CFGR_SWS_MASK);
}

void RCC_SetAHBPrescaler(AHBPrescaler prescaler) {
    uint32_t reg = RCC_D1CFGR;
    reg &= ~(0xFU << D1CFGR_HPRE_Pos);
    reg |= (prescaler & 0xFU) << D1CFGR_HPRE_Pos;
    RCC_D1CFGR = reg;
}

void RCC_SetAPB1Prescaler(APBPrescaler prescaler) {
    uint32_t reg = RCC_D2CFGR;
    reg &= ~(D2CFGR_D2PPRE1_MASK << D2CFGR_D2PPRE1_Pos);
    reg |= (prescaler & D2CFGR_D2PPRE1_MASK) << D2CFGR_D2PPRE1_Pos;
    RCC_D2CFGR = reg;
}

void RCC_SetAPB2Prescaler(APBPrescaler prescaler) {
    uint32_t reg = RCC_D2CFGR;
    reg &= ~(D2CFGR_D2PPRE2_MASK << D2CFGR_D2PPRE2_Pos);
    reg |= (prescaler & D2CFGR_D2PPRE2_MASK) << D2CFGR_D2PPRE2_Pos;
    RCC_D2CFGR = reg;
}

void RCC_SetAPB3Prescaler(APBPrescaler prescaler) {
    uint32_t reg = RCC_D3CFGR;
    reg &= ~(D3CFGR_D3PPRE_MASK << D3CFGR_D3PPRE_Pos);
    reg |= (prescaler & D3CFGR_D3PPRE_MASK) << D3CFGR_D3PPRE_Pos;
    RCC_D3CFGR = reg;
}

void RCC_SetAPB4Prescaler(APBPrescaler prescaler) {
    uint32_t reg = RCC_D3CFGR;
    reg &= ~(D3CFGR_D3PPRE_MASK << D3CFGR_D3PPRE_Pos);
    reg |= (prescaler & D3CFGR_D3PPRE_MASK) << D3CFGR_D3PPRE_Pos;
    RCC_D3CFGR = reg;
}

uint32_t RCC_GetSYSCLKFrequency(void) {
    SystemClockSource source = RCC_GetSystemClockSource();
    uint32_t freq = 0;
    
    switch (source) {
        case CLOCK_SOURCE_HSI:
            freq = HSI_FREQUENCY;
            break;
        case CLOCK_SOURCE_CSI:
            freq = CSI_FREQUENCY;
            break;
        case CLOCK_SOURCE_HSE:
            freq = HSE_FREQUENCY_MIN;
            break;
        case CLOCK_SOURCE_PLL1:
            freq = RCC_GetSYSCLKFrequency();
            break;
    }
    
    return freq;
}

uint32_t RCC_GetHCLKFrequency(void) {
    uint32_t sysclk = RCC_GetSYSCLKFrequency();
    uint32_t hpre = (RCC_D1CFGR >> D1CFGR_HPRE_Pos) & 0xFU;
    
    if (hpre < 8) {
        return sysclk;
    }
    
    uint32_t divisor = 1 << (hpre - 7);
    return sysclk / divisor;
}

uint32_t RCC_GetPCLK1Frequency(void) {
    uint32_t hclk = RCC_GetHCLKFrequency();
    uint32_t ppre1 = (RCC_D2CFGR >> D2CFGR_D2PPRE1_Pos) & D2CFGR_D2PPRE1_MASK;
    
    if (ppre1 < 4) {
        return hclk;
    }
    
    uint32_t divisor = 1 << (ppre1 - 3);
    return hclk / divisor;
}

uint32_t RCC_GetPCLK2Frequency(void) {
    uint32_t hclk = RCC_GetHCLKFrequency();
    uint32_t ppre2 = (RCC_D2CFGR >> D2CFGR_D2PPRE2_Pos) & D2CFGR_D2PPRE2_MASK;
    
    if (ppre2 < 4) {
        return hclk;
    }
    
    uint32_t divisor = 1 << (ppre2 - 3);
    return hclk / divisor;
}

void RCC_SetVoltageScale(VoltageScale scale) {
    uint32_t reg = PWR_CR1;
    reg &= ~(PWR_CR1_VOS_MASK << PWR_CR1_VOS_Pos);
    reg |= (scale & PWR_CR1_VOS_MASK) << PWR_CR1_VOS_Pos;
    PWR_CR1 = reg;
}

VoltageScale RCC_GetVoltageScale(void) {
    return (VoltageScale)((PWR_CR1 >> PWR_CR1_VOS_Pos) & PWR_CR1_VOS_MASK);
}

void RCC_ConfigureFlashLatency(uint32_t sysclk_freq) {
    uint32_t wait_states = 0;
    
    if (sysclk_freq <= 20000000UL) {
        wait_states = FLASH_ACR_LATENCY_0WS;
    } else if (sysclk_freq <= 40000000UL) {
        wait_states = FLASH_ACR_LATENCY_1WS;
    } else if (sysclk_freq <= 60000000UL) {
        wait_states = FLASH_ACR_LATENCY_2WS;
    } else if (sysclk_freq <= 80000000UL) {
        wait_states = FLASH_ACR_LATENCY_3WS;
    } else if (sysclk_freq <= 100000000UL) {
        wait_states = FLASH_ACR_LATENCY_4WS;
    } else if (sysclk_freq <= 120000000UL) {
        wait_states = FLASH_ACR_LATENCY_5WS;
    } else if (sysclk_freq <= 140000000UL) {
        wait_states = FLASH_ACR_LATENCY_6WS;
    } else {
        wait_states = FLASH_ACR_LATENCY_7WS;
    }
    
    uint32_t reg = FLASH_ACR;
    reg &= ~(FLASH_ACR_LATENCY_MASK << FLASH_ACR_LATENCY_Pos);
    reg |= (wait_states & FLASH_ACR_LATENCY_MASK) << FLASH_ACR_LATENCY_Pos;
    FLASH_ACR = reg;
    
    reg = FLASH_ACR;
    reg &= ~(0x3U << FLASH_ACR_WRHIGHFREQ_Pos);
    reg |= (0x2U << FLASH_ACR_WRHIGHFREQ_Pos);
    FLASH_ACR = reg;
}

void RCC_SetPowerMode(VoltageScale voltage_scale) {
    RCC_SetVoltageScale(voltage_scale);
    
    while (RCC_GetVoltageScale() != voltage_scale) {
    }
    
    if (voltage_scale == VOLTAGE_SCALE1) {
        PWR_CR5 &= ~(1U << PWR_CR5_R1MODE_Pos);
    }
}

void RCC_SystemClockConfig_HSI(uint32_t sysclk) {
    RCC_EnableHSI();
    while (!RCC_IsHSIReady());
    
    RCC_SetAHBPrescaler(AHB_DIV_1);
    RCC_SetAPB1Prescaler(APB_DIV_1);
    RCC_SetAPB2Prescaler(APB_DIV_1);
    
    if (sysclk > HSI_FREQUENCY) {
        sysclk = HSI_FREQUENCY;
    }
    
    RCC_ConfigureFlashLatency(sysclk);
    RCC_SetSystemClock(CLOCK_SOURCE_HSI);
}

void RCC_SystemClockConfig_HSE(uint32_t sysclk) {
    RCC_EnableHSE();
    while (!RCC_IsHSEReady());
    
    RCC_SetAHBPrescaler(AHB_DIV_1);
    RCC_SetAPB1Prescaler(APB_DIV_1);
    RCC_SetAPB2Prescaler(APB_DIV_1);
    
    RCC_ConfigureFlashLatency(sysclk);
    RCC_SetSystemClock(CLOCK_SOURCE_HSE);
}

void RCC_SystemClockConfig_PLL(PLL_Config *pll_config, uint32_t sysclk) {
    RCC_EnableHSI();
    while (!RCC_IsHSIReady());
    
    RCC_ConfigureFlashLatency(sysclk);
    
    RCC_ConfigurePLL1(pll_config);
    
    RCC_EnablePLL1();
    while (!RCC_IsPLL1Ready());
    
    RCC_SetSystemClock(CLOCK_SOURCE_PLL1);
}

void RCC_SystemClockConfig_MSI(void) {
    RCC_EnableCSI();
    while (!RCC_IsCSIReady());
    
    RCC_SetAHBPrescaler(AHB_DIV_1);
    RCC_SetAPB1Prescaler(APB_DIV_1);
    RCC_SetAPB2Prescaler(APB_DIV_1);
    
    RCC_ConfigureFlashLatency(CSI_FREQUENCY);
    RCC_SetSystemClock(CLOCK_SOURCE_CSI);
}

ClockStatus RCC_WaitForHSIReady(uint32_t timeout) {
    uint32_t start = 0;
    while (!RCC_IsHSIReady()) {
        start++;
        if (start >= timeout) {
            return CLOCK_TIMEOUT;
        }
    }
    return CLOCK_OK;
}

ClockStatus RCC_WaitForHSEReady(uint32_t timeout) {
    uint32_t start = 0;
    while (!RCC_IsHSEReady()) {
        start++;
        if (start >= timeout) {
            return CLOCK_TIMEOUT;
        }
    }
    return CLOCK_OK;
}

ClockStatus RCC_WaitForPLL1Ready(uint32_t timeout) {
    uint32_t start = 0;
    while (!RCC_IsPLL1Ready()) {
        start++;
        if (start >= timeout) {
            return CLOCK_TIMEOUT;
        }
    }
    return CLOCK_OK;
}

void RCC_EnablePeripheralClock(uint32_t periph_base) {
    if (periph_base >= 0x40020000 && periph_base < 0x40030000) {
        uint32_t bit = (periph_base - 0x40020000) >> 14;
        if (bit < 32) {
            RCC_AHB1ENR |= (1U << bit);
        } else if (bit < 64) {
            RCC_AHB2ENR |= (1U << (bit - 32));
        }
    } else if (periph_base >= 0x40000000 && periph_base < 0x40010000) {
        uint32_t bit = (periph_base - 0x40000000) >> 10;
        if (bit < 32) {
            RCC_APB1LENR |= (1U << bit);
        }
    } else if (periph_base >= 0x40010000 && periph_base < 0x40020000) {
        uint32_t bit = (periph_base - 0x40010000) >> 10;
        RCC_APB2ENR |= (1U << bit);
    }
}

void RCC_DisablePeripheralClock(uint32_t periph_base) {
    if (periph_base >= 0x40020000 && periph_base < 0x40030000) {
        uint32_t bit = (periph_base - 0x40020000) >> 14;
        if (bit < 32) {
            RCC_AHB1ENR &= ~(1U << bit);
        } else if (bit < 64) {
            RCC_AHB2ENR &= ~(1U << (bit - 32));
        }
    } else if (periph_base >= 0x40000000 && periph_base < 0x40010000) {
        uint32_t bit = (periph_base - 0x40000000) >> 10;
        if (bit < 32) {
            RCC_APB1LENR &= ~(1U << bit);
        }
    } else if (periph_base >= 0x40010000 && periph_base < 0x40020000) {
        uint32_t bit = (periph_base - 0x40010000) >> 10;
        RCC_APB2ENR &= ~(1U << bit);
    }
}
