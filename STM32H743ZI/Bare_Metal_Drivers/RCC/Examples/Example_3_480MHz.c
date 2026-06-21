#include "RCC_Examples.h"

void Example_3A_HSE_480MHz(void) {
    RCC_SetPowerMode(VOLTAGE_SCALE1);
    
    RCC_EnableHSE();
    while (!RCC_IsHSEReady());
    
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_HSE;
    pll_config.pll_m = 2;
    pll_config.pll_n = 120;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    
    RCC_ConfigureFlashLatency(TARGET_SYSCLK_480MHZ);
    RCC_ConfigurePLL1(&pll_config);
    RCC_EnablePLL1();
    while (!RCC_IsPLL1Ready());
    
    RCC_SetAHBPrescaler(AHB_DIV_1);
    RCC_SetAPB1Prescaler(APB_DIV_2);
    RCC_SetAPB2Prescaler(APB_DIV_2);
    
    RCC_SetSystemClock(CLOCK_SOURCE_PLL1);
}

void Example_3B_PLL_480MHz(void) {
    RCC_SetPowerMode(VOLTAGE_SCALE1);
    
    RCC_EnableHSI();
    while (!RCC_IsHSIReady());
    
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_HSI;
    pll_config.pll_m = 8;
    pll_config.pll_n = 120;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    
    RCC_ConfigureFlashLatency(TARGET_SYSCLK_480MHZ);
    RCC_ConfigurePLL1(&pll_config);
    RCC_EnablePLL1();
    while (!RCC_IsPLL1Ready());
    
    RCC_SetAHBPrescaler(AHB_DIV_1);
    RCC_SetAPB1Prescaler(APB_DIV_2);
    RCC_SetAPB2Prescaler(APB_DIV_2);
    
    RCC_SetSystemClock(CLOCK_SOURCE_PLL1);
}

void Example_3C_CSI_480MHz(void) {
    RCC_SetPowerMode(VOLTAGE_SCALE1);
    
    RCC_EnableCSI();
    while (!RCC_IsCSIReady());
    
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_CSI;
    pll_config.pll_m = 1;
    pll_config.pll_n = 120;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    
    RCC_ConfigureFlashLatency(TARGET_SYSCLK_480MHZ);
    RCC_ConfigurePLL1(&pll_config);
    RCC_EnablePLL1();
    while (!RCC_IsPLL1Ready());
    
    RCC_SetAHBPrescaler(AHB_DIV_1);
    RCC_SetAPB1Prescaler(APB_DIV_2);
    RCC_SetAPB2Prescaler(APB_DIV_2);
    
    RCC_SetSystemClock(CLOCK_SOURCE_PLL1);
}

void Example_3D_HSI_480MHz(void) {
    RCC_SetPowerMode(VOLTAGE_SCALE1);
    
    RCC_EnableHSI();
    while (!RCC_IsHSIReady());
    
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_HSI;
    pll_config.pll_m = 8;
    pll_config.pll_n = 120;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    
    RCC_ConfigureFlashLatency(TARGET_SYSCLK_480MHZ);
    RCC_ConfigurePLL1(&pll_config);
    RCC_EnablePLL1();
    while (!RCC_IsPLL1Ready());
    
    RCC_SetAHBPrescaler(AHB_DIV_1);
    RCC_SetAPB1Prescaler(APB_DIV_2);
    RCC_SetAPB2Prescaler(APB_DIV_2);
    
    RCC_SetSystemClock(CLOCK_SOURCE_PLL1);
}

void Example_3B_PLL_480MHz(void) {
    RCC_SetPowerMode(VOLTAGE_SCALE1);
    
    RCC_EnableHSI();
    while (!RCC_IsHSIReady());
    
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_HSI;
    pll_config.pll_m = 8;
    pll_config.pll_n = 120;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    
    RCC_ConfigureFlashLatency(TARGET_SYSCLK_480MHZ);
    RCC_ConfigurePLL1(&pll_config);
    RCC_EnablePLL1();
    while (!RCC_IsPLL1Ready());
    
    RCC_SetAHBPrescaler(AHB_DIV_1);
    RCC_SetAPB1Prescaler(APB_DIV_2);
    RCC_SetAPB2Prescaler(APB_DIV_2);
    
    RCC_SetSystemClock(CLOCK_SOURCE_PLL1);
}

void Example_3C_CSI_480MHz(void) {
    RCC_SetPowerMode(VOLTAGE_SCALE1);
    
    RCC_EnableCSI();
    while (!RCC_IsCSIReady());
    
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_CSI;
    pll_config.pll_m = 1;
    pll_config.pll_n = 120;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    
    RCC_ConfigureFlashLatency(TARGET_SYSCLK_480MHZ);
    RCC_ConfigurePLL1(&pll_config);
    RCC_EnablePLL1();
    while (!RCC_IsPLL1Ready());
    
    RCC_SetAHBPrescaler(AHB_DIV_1);
    RCC_SetAPB1Prescaler(APB_DIV_2);
    RCC_SetAPB2Prescaler(APB_DIV_2);
    
    RCC_SetSystemClock(CLOCK_SOURCE_PLL1);
}

void Example_3D_HSI_480MHz(void) {
    RCC_SetPowerMode(VOLTAGE_SCALE1);
    
    RCC_EnableHSI();
    while (!RCC_IsHSIReady());
    
    PLL_Config pll_config;
    pll_config.pll_source = PLL_SOURCE_HSI;
    pll_config.pll_m = 8;
    pll_config.pll_n = 120;
    pll_config.pll_p = 2;
    pll_config.pll_q = 2;
    pll_config.pll_r = 2;
    
    RCC_ConfigureFlashLatency(TARGET_SYSCLK_480MHZ);
    RCC_ConfigurePLL1(&pll_config);
    RCC_EnablePLL1();
    while (!RCC_IsPLL1Ready());
    
    RCC_SetAHBPrescaler(AHB_DIV_1);
    RCC_SetAPB1Prescaler(APB_DIV_2);
    RCC_SetAPB2Prescaler(APB_DIV_2);
    
    RCC_SetSystemClock(CLOCK_SOURCE_PLL1);
}
