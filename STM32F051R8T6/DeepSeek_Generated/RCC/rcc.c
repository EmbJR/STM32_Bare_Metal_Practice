#include "rcc.h"
#include <stddef.h>  // Added for NULL definition

// Private function prototypes
static void RCC_ConfigurePLL(const RCC_Config *config);
static void RCC_ConfigureClockTree(const RCC_Config *config);
static uint32_t RCC_CalculatePLLFrequency(const RCC_Config *config);
static bool RCC_IsDeviceF04xF07xF09x(void);

// Initialize RCC with given configuration
void RCC_Init(const RCC_Config *config) {
    // 1. Enable HSI as fallback clock
    RCC_EnableHSI();

    // 2. Configure HSE if needed
    if (config->hse_enabled) {
        RCC_EnableHSE();
    }

    // 3. Configure HSI48 for specific devices
    if (config->hsi48_enabled && RCC_IsDeviceF04xF07xF09x()) {
        RCC_EnableHSI48();
    }

    // 4. Configure PLL if enabled
    if (config->pll_enabled) {
        RCC_ConfigurePLL(config);
        RCC_EnablePLL();
    }

    // 5. Configure clock tree
    RCC_ConfigureClockTree(config);

    // 6. Enable CSS if requested
    if (config->css_enabled) {
        RCC_CR |= (1 << 19); // Set CSSON bit
    }
}

// Enable HSI oscillator
void RCC_EnableHSI(void) {
    RCC_CR |= (1 << 0); // Set HSION bit

    // Wait for HSI ready
    while (!(RCC_CR & (1 << 1))) {
        // Wait for HSIRDY
    }
}

// Enable HSE oscillator
void RCC_EnableHSE(void) {
    RCC_CR |= (1 << 16); // Set HSEON bit

    // Wait for HSE ready
    while (!(RCC_CR & (1 << 17))) {
        // Wait for HSERDY
    }
}

// Enable HSI48 oscillator (F04x/F07x/F09x only)
void RCC_EnableHSI48(void) {
    if (!RCC_IsDeviceF04xF07xF09x()) return;

    RCC_CR2 |= (1 << 16); // Set HSI48ON bit

    // Wait for HSI48 ready
    while (!(RCC_CR2 & (1 << 17))) {
        // Wait for HSI48RDY
    }
}

// Enable PLL
void RCC_EnablePLL(void) {
    RCC_CR |= (1 << 24); // Set PLLON bit

    // Wait for PLL ready
    while (!(RCC_CR & (1 << 25))) {
        // Wait for PLLRDY
    }
}

// Disable HSI oscillator
void RCC_DisableHSI(void) {
    RCC_CR &= ~(1 << 0); // Clear HSION bit
}

// Disable HSE oscillator
void RCC_DisableHSE(void) {
    RCC_CR &= ~(1 << 16); // Clear HSEON bit
}

// Disable PLL
void RCC_DisablePLL(void) {
    RCC_CR &= ~(1 << 24); // Clear PLLON bit

    // Wait for PLL to stop
    while (RCC_CR & (1 << 25)) {
        // Wait for PLLRDY to clear
    }
}

// Disable HSI48 oscillator (F04x/F07x/F09x only)
void RCC_DisableHSI48(void) {
    if (!RCC_IsDeviceF04xF07xF09x()) return;

    RCC_CR2 &= ~(1 << 16); // Clear HSI48ON bit
}

// Set system clock source
void RCC_SetSystemClockSource(ClockSource source) {
    // Clear SW bits
    RCC_CFGR &= ~(3 << 0);

    // Set new source
    RCC_CFGR |= (source << 0);

    // Wait for switch
    while (((RCC_CFGR >> 2) & 3) != source) {
        // Wait for SWS to match
    }
}

// Set AHB prescaler
void RCC_SetAHBPrescaler(AHBPrescaler prescaler) {
    RCC_CFGR &= ~(0xF << 4);    // Clear HPRE bits
    RCC_CFGR |= (prescaler << 4); // Set new prescaler
}

// Set APB prescaler (APB1 and APB2 share same prescaler in F0)
void RCC_SetAPBPrescaler(APBPrescaler prescaler) {
    RCC_CFGR &= ~(7 << 8);     // Clear PPRE bits
    RCC_CFGR |= (prescaler << 8); // Set new prescaler
}

// Configure PLL source and multiplier
void RCC_SetPLLConfig(PLLSource source, uint8_t multiplier) {
    // Disable PLL before configuration
    bool pll_was_enabled = (RCC_CR & (1 << 24)) != 0;
    if (pll_was_enabled) {
        RCC_DisablePLL();
    }

    // Configure PLL source
    if (RCC_IsDeviceF04xF07xF09x()) {
        RCC_CFGR &= ~(3 << 15); // Clear PLLSRC bits
        RCC_CFGR |= (source << 15);
    } else {
        // F03x/F05x devices
        RCC_CFGR &= ~(1 << 16); // Clear PLLSRC bit
        RCC_CFGR |= ((source & 0x01) << 16);
    }

    // Configure PLL multiplier (2-16)
    if (multiplier >= 2 && multiplier <= 16) {
        RCC_CFGR &= ~(0xF << 18); // Clear PLLMUL bits

        // Convert multiplier to PLLMUL encoding
        uint8_t pllmul_encoding;
        if (multiplier == 16) pllmul_encoding = 0x7; // 0111 for x16
        else if (multiplier >= 6 && multiplier <= 15) pllmul_encoding = multiplier - 2;
        else if (multiplier == 5) pllmul_encoding = 0x3; // 0011 for x5
        else if (multiplier == 4) pllmul_encoding = 0x2; // 0010 for x4
        else if (multiplier == 3) pllmul_encoding = 0x1; // 0001 for x3
        else pllmul_encoding = 0x0; // 0000 for x2

        RCC_CFGR |= (pllmul_encoding << 18);
    }

    // Re-enable PLL if it was enabled
    if (pll_was_enabled) {
        RCC_EnablePLL();
    }
}

// Get current system clock frequency
uint32_t RCC_GetSystemClockFrequency(void) {
    ClockSource source = (RCC_CFGR >> 2) & 3; // Read SWS bits

    switch (source) {
        case CLOCK_SOURCE_HSI:
            return 8000000UL; // 8 MHz HSI

        case CLOCK_SOURCE_HSE:
            return 8000000UL; // Default HSE frequency (adjust based on crystal)

        case CLOCK_SOURCE_PLL:
            // Pass NULL since we don't have a config structure here
            return RCC_CalculatePLLFrequency(NULL);

        case CLOCK_SOURCE_HSI48:
            return 48000000UL; // 48 MHz HSI48

        default:
            return 8000000UL; // Default to HSI
    }
}

// Enable peripheral clock
void RCC_EnablePeripheralClock(uint8_t peripheral_type, uint8_t peripheral_num) {
    uint8_t peripheral_group = peripheral_type >> 4;
    uint8_t peripheral_index = peripheral_type & 0x0F;

    switch (peripheral_group) {
        case 0x0: // GPIOs
            RCC_AHBENR |= (1 << (17 + peripheral_index));
            break;

        case 0x8: // DMA
            if (peripheral_num == 1) RCC_AHBENR |= (1 << 0);
            else if (peripheral_num == 2 && RCC_IsDeviceF04xF07xF09x())
                RCC_AHBENR |= (1 << 1);
            break;

        case 0x9: // Timers
            switch (peripheral_num) {
                case 1: RCC_APB2ENR |= (1 << 11); break;
                case 2: RCC_APB1ENR |= (1 << 0); break;
                case 3: RCC_APB1ENR |= (1 << 1); break;
                case 14: RCC_APB1ENR |= (1 << 8); break;
                case 15: RCC_APB2ENR |= (1 << 16); break;
                case 16: RCC_APB2ENR |= (1 << 17); break;
                case 17: RCC_APB2ENR |= (1 << 18); break;
            }
            break;

        case 0xA: // USARTs
            switch (peripheral_num) {
                case 1: RCC_APB2ENR |= (1 << 14); break;
                case 2: RCC_APB1ENR |= (1 << 17); break;
                case 3: RCC_APB1ENR |= (1 << 18); break;
            }
            break;

        case 0xB: // SPI
            if (peripheral_num == 1) RCC_APB2ENR |= (1 << 12);
            else if (peripheral_num == 2) RCC_APB1ENR |= (1 << 14);
            break;

        case 0xC: // I2C
            if (peripheral_num == 1) RCC_APB1ENR |= (1 << 21);
            else if (peripheral_num == 2) RCC_APB1ENR |= (1 << 22);
            break;

        case 0xD: // ADC/DAC
            if (peripheral_num == 0) RCC_APB2ENR |= (1 << 9); // ADC
            else if (peripheral_num == 1) RCC_APB1ENR |= (1 << 29); // DAC
            break;

        case 0xE: // USB (F04x/F07x only)
            if (RCC_IsDeviceF04xF07xF09x()) {
                RCC_APB1ENR |= (1 << 23);
            }
            break;
    }
}

// Disable peripheral clock
void RCC_DisablePeripheralClock(uint8_t peripheral_type, uint8_t peripheral_num) {
    uint8_t peripheral_group = peripheral_type >> 4;
    uint8_t peripheral_index = peripheral_type & 0x0F;

    switch (peripheral_group) {
        case 0x0: // GPIOs
            RCC_AHBENR &= ~(1 << (17 + peripheral_index));
            break;

        case 0x8: // DMA
            if (peripheral_num == 1) RCC_AHBENR &= ~(1 << 0);
            else if (peripheral_num == 2 && RCC_IsDeviceF04xF07xF09x())
                RCC_AHBENR &= ~(1 << 1);
            break;

        case 0x9: // Timers
            switch (peripheral_num) {
                case 1: RCC_APB2ENR &= ~(1 << 11); break;
                case 2: RCC_APB1ENR &= ~(1 << 0); break;
                case 3: RCC_APB1ENR &= ~(1 << 1); break;
                case 14: RCC_APB1ENR &= ~(1 << 8); break;
                case 15: RCC_APB2ENR &= ~(1 << 16); break;
                case 16: RCC_APB2ENR &= ~(1 << 17); break;
                case 17: RCC_APB2ENR &= ~(1 << 18); break;
            }
            break;

        case 0xA: // USARTs
            switch (peripheral_num) {
                case 1: RCC_APB2ENR &= ~(1 << 14); break;
                case 2: RCC_APB1ENR &= ~(1 << 17); break;
                case 3: RCC_APB1ENR &= ~(1 << 18); break;
            }
            break;

        case 0xB: // SPI
            if (peripheral_num == 1) RCC_APB2ENR &= ~(1 << 12);
            else if (peripheral_num == 2) RCC_APB1ENR &= ~(1 << 14);
            break;

        case 0xC: // I2C
            if (peripheral_num == 1) RCC_APB1ENR &= ~(1 << 21);
            else if (peripheral_num == 2) RCC_APB1ENR &= ~(1 << 22);
            break;

        case 0xD: // ADC/DAC
            if (peripheral_num == 0) RCC_APB2ENR &= ~(1 << 9); // ADC
            else if (peripheral_num == 1) RCC_APB1ENR &= ~(1 << 29); // DAC
            break;

        case 0xE: // USB (F04x/F07x only)
            if (RCC_IsDeviceF04xF07xF09x()) {
                RCC_APB1ENR &= ~(1 << 23);
            }
            break;
    }
}

// Private: Configure PLL based on config
static void RCC_ConfigurePLL(const RCC_Config *config) {
    RCC_SetPLLConfig(config->pll_source, config->pll_multiplier);
}

// Private: Configure complete clock tree
static void RCC_ConfigureClockTree(const RCC_Config *config) {
    // Set AHB prescaler
    RCC_SetAHBPrescaler(config->ahb_prescaler);

    // Set APB prescaler
    RCC_SetAPBPrescaler(config->apb_prescaler);

    // Set final system clock source
    RCC_SetSystemClockSource(config->system_clock_source);
}

// Private: Calculate PLL frequency
static uint32_t RCC_CalculatePLLFrequency(const RCC_Config *config) {
    uint32_t pll_input_freq = 0;
    uint8_t pll_source = (RCC_CFGR >> 15) & 0x03;
    uint8_t pll_multiplier_encoding = (RCC_CFGR >> 18) & 0x0F;
    uint8_t pll_multiplier = 2; // Default

    // Decode PLL multiplier
    switch (pll_multiplier_encoding) {
        case 0x0: pll_multiplier = 2; break;
        case 0x1: pll_multiplier = 3; break;
        case 0x2: pll_multiplier = 4; break;
        case 0x3: pll_multiplier = 5; break;
        case 0x4: pll_multiplier = 6; break;
        case 0x5: pll_multiplier = 7; break;
        case 0x6: pll_multiplier = 8; break;
        case 0x7: pll_multiplier = 9; break;
        case 0x8: pll_multiplier = 10; break;
        case 0x9: pll_multiplier = 11; break;
        case 0xA: pll_multiplier = 12; break;
        case 0xB: pll_multiplier = 13; break;
        case 0xC: pll_multiplier = 14; break;
        case 0xD: pll_multiplier = 15; break;
        case 0xE: pll_multiplier = 16; break;
        case 0xF: pll_multiplier = 16; break;
    }

    // Get PLL input frequency
    if (RCC_IsDeviceF04xF07xF09x()) {
        switch (pll_source) {
            case PLL_SOURCE_HSI_DIV2:
                pll_input_freq = 4000000UL; // HSI / 2
                break;
            case PLL_SOURCE_HSE:
                pll_input_freq = 8000000UL; // HSE directly
                break;
            case PLL_SOURCE_HSE_DIV2:
                pll_input_freq = 4000000UL; // HSE / 2
                break;
            case PLL_SOURCE_HSI48_DIV2:
                pll_input_freq = 24000000UL; // HSI48 / 2
                break;
        }
    } else {
        // F03x/F05x devices
        bool pllsrc = (RCC_CFGR >> 16) & 0x01;
        if (pllsrc) {
            pll_input_freq = 8000000UL; // HSE
        } else {
            pll_input_freq = 4000000UL; // HSI/2
        }
    }

    return pll_input_freq * pll_multiplier;
}

// Private: Check if device is F04x/F07x/F09x
static bool RCC_IsDeviceF04xF07xF09x(void) {
    // This is a simplified check. In real implementation,
    // you would read device ID from DBGMCU or use compile-time flags
    // For now, we assume F04x/F07x/F09x if HSI48 register exists

    // Check if RCC_CR2 register exists by reading a known bit
    // (HSI48ON bit 16 would be 0 after reset on devices that support it)
    // Return true for now - you should implement proper device detection
    return true; // Placeholder - implement based on your device

    // Alternative: Check via compile-time flags
    // #if defined(STM32F04x) || defined(STM32F07x) || defined(STM32F09x)
    //     return true;
    // #else
    //     return false;
    // #endif
}
