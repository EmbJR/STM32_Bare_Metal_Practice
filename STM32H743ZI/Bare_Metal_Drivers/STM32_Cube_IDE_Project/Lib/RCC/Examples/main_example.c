#include "RCC_Examples.h"

volatile uint32_t g_tick = 0;

void SysTick_Init(uint32_t reload) {
    SYSTICK_LOAD = reload;
    SYSTICK_VAL = 0;
    SYSTICK_CTRL = (1 << 2) | (1 << 1) | (1 << 0);
}

void SysTick_Handler(void) {
    g_tick++;
}

void Delay_Init(void) {
    SysTick_Init((RCC_GetSYSCLKFrequency() / 1000) - 1);
}

void Delay_ms(uint32_t ms) {
    uint32_t start = g_tick;
    while ((g_tick - start) < ms);
}

void RCC_PrintClockInfo(void) {
    volatile uint32_t sysclk = RCC_GetSYSCLKFrequency();
    volatile uint32_t hclk = RCC_GetHCLKFrequency();
    volatile uint32_t pclk1 = RCC_GetPCLK1Frequency();
    volatile uint32_t pclk2 = RCC_GetPCLK2Frequency();
    volatile SystemClockSource src = RCC_GetSystemClockSource();
    
    (void)sysclk;
    (void)hclk;
    (void)pclk1;
    (void)pclk2;
    (void)src;
}

int main(void) {
    Delay_Init();
    
    Example_1A_HSI_4MHz();
    Delay_ms(100);
    RCC_PrintClockInfo();
    
    Example_2A_HSE_16MHz();
    Delay_ms(100);
    RCC_PrintClockInfo();
    
    Example_3B_PLL_480MHz();
    Delay_ms(100);
    RCC_PrintClockInfo();
    
    while (1) {
        Delay_ms(1000);
    }
    
    return 0;
}
