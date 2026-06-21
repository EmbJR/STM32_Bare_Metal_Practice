#include "RCC_Examples.h"

void Example_1A_HSI_4MHz(void) {
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_HSI;
    pll_config.pll_m = 16;
    pll_config.pll_n = 16;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    RCC_SystemClockConfig_PLL(&pll_config, TARGET_SYSCLK_4MHZ);
}

void Example_1B_PLL_4MHz(void) {
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_HSI;
    pll_config.pll_m = 16;
    pll_config.pll_n = 16;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    RCC_SystemClockConfig_PLL(&pll_config, TARGET_SYSCLK_4MHZ);
}

void Example_1C_CSI_4MHz(void) {
    RCC_SystemClockConfig_MSI();
}

void Example_1D_HSI_4MHz_SysclkDiv(void) {
    RCC_SystemClockConfig_HSI(HSI_FREQUENCY);
    RCC_SetAHBPrescaler(AHB_DIV_16);
}
