#include "RCC_Examples.h"

void Example_2A_HSE_16MHz(void) {
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_HSE;
    pll_config.pll_m = 2;
    pll_config.pll_n = 8;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    RCC_SystemClockConfig_PLL(&pll_config, TARGET_SYSCLK_16MHZ);
}

void Example_2B_PLL_16MHz(void) {
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_HSI;
    pll_config.pll_m = 8;
    pll_config.pll_n = 16;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    RCC_SystemClockConfig_PLL(&pll_config, TARGET_SYSCLK_16MHZ);
}

void Example_2C_CSI_16MHz(void) {
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_CSI;
    pll_config.pll_m = 1;
    pll_config.pll_n = 16;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    RCC_SystemClockConfig_PLL(&pll_config, TARGET_SYSCLK_16MHZ);
}

void Example_2D_HSI_16MHz(void) {
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_HSI;
    pll_config.pll_m = 8;
    pll_config.pll_n = 16;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    RCC_SystemClockConfig_PLL(&pll_config, TARGET_SYSCLK_16MHZ);
}
