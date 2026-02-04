#include "rcc.h"

// Example configuration for 48MHz system clock using PLL from HSE
void SystemClock_Config_48MHz(void) {
    RCC_Config config = {
        .system_clock_source = CLOCK_SOURCE_PLL,
        .target_frequency = SYSTEM_CLOCK_48MHZ,
        .hse_enabled = true,
        .pll_enabled = true,
        .pll_source = PLL_SOURCE_HSE,
        .pll_multiplier = 6,  // HSE 8MHz * 6 = 48MHz
        .ahb_prescaler = AHB_PRESCALER_1,
        .apb_prescaler = APB_PRESCALER_1,
        .hsi48_enabled = false,
        .css_enabled = true
    };

    RCC_Init(&config);
}

// Example configuration for 8MHz using HSI (no crystal needed)
void SystemClock_Config_8MHz(void) {
    RCC_Config config = {
        .system_clock_source = CLOCK_SOURCE_HSI,
        .target_frequency = SYSTEM_CLOCK_8MHZ,
        .hse_enabled = false,
        .pll_enabled = false,
        .pll_source = PLL_SOURCE_HSI_DIV2,
        .pll_multiplier = 2,
        .ahb_prescaler = AHB_PRESCALER_1,
        .apb_prescaler = APB_PRESCALER_1,
        .hsi48_enabled = false,
        .css_enabled = false
    };

    RCC_Init(&config);
}

int main(void) {
    // Configure system clock
	SystemClock_Config_8MHz();

    // Enable peripheral clocks as needed
    RCC_EnablePeripheralClock(PERIPH_GPIOA, 0);
    RCC_EnablePeripheralClock(PERIPH_GPIOB, 0);
    RCC_EnablePeripheralClock(PERIPH_USART1, 0);
    RCC_EnablePeripheralClock(PERIPH_ADC, 0);

    // Get current system clock frequency
    uint32_t sysclk = RCC_GetSystemClockFrequency();

    while (1) {
        // Application code here
    }

    return 0;
}
