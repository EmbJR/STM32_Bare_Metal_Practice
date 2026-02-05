#include "rcc.h"
#include "gpio.h"

// Flash Access Control Register (FLASH_ACR) address
#define FLASH_ACR      (*(volatile uint32_t *)0x40022000UL)


// Example configurations
void Configure_LED_Pin(void) {
    GPIO_Config led_config = {
        .mode = GPIO_MODE_OUTPUT,
        .output_type = GPIO_OUTPUT_PUSH_PULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_PULL_UP,
        .alternate = GPIO_AF0
    };

    // Configure PB0 as LED output
    GPIO_Init(GPIOC, GPIO_PIN_9, &led_config);
    GPIO_Init(GPIOC, GPIO_PIN_8, &led_config);
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

void SystemClock_Config_32MHz_HSI(void) {

	// Step 1: Configure Flash wait states BEFORE increasing clock speed
	FLASH_ACR |= (1 << 0);  // Set LATENCY[0] = 1 (1 wait state for >24MHz)

    RCC_Config config = {
        .system_clock_source = CLOCK_SOURCE_PLL,
        .target_frequency = 32000000UL,
        .hse_enabled = false,            // No external crystal
        .pll_enabled = true,
        .pll_source = PLL_SOURCE_HSI_DIV2, // HSI/2 = 4MHz
        .pll_multiplier = 8,             // 4MHz * 8 = 32MHz
        .ahb_prescaler = AHB_PRESCALER_1,
        .apb_prescaler = APB_PRESCALER_1,
        .hsi48_enabled = false,
        .css_enabled = false
    };

    RCC_Init(&config);
}

int main(void) {
    // Configure system clock
	SystemClock_Config_32MHz_HSI();
    // Enable peripheral clocks as needed
    //RCC_EnablePeripheralClock(PERIPH_GPIOA, 0);
    //RCC_EnablePeripheralClock(PERIPH_GPIOC, 0);

    // Get current system clock frequency
    uint32_t sysclk = RCC_GetSystemClockFrequency();
    Configure_LED_Pin();

    while (1) {
    	GPIO_TogglePin(GPIOC, GPIO_PIN_9);
    	GPIO_TogglePin(GPIOC, GPIO_PIN_8);
    	for(int i = 0; i < 500; i++)
    		for(int j = 0; j < 200; j++);
        // Application code here
    }

    return 0;
}
