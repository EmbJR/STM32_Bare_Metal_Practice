#include "RCC_Config.h"
#include "Flash.h"

void SystemClock_Config_HSI(void) {
    // 1. Enable HSI and wait for ready
    RCC->CR |= RCC_CR_HSION_Pos;                 // Enable HSI
    while (!(RCC->CR & RCC_CR_HSIRDY_Pos));      // Wait for HSI ready
    
    // 2. Configure Flash latency for 32 MHz (1 wait state)
    FLASH->ACR &= ~FLASH_ACR_LATENCY_Pos;        // Clear latency bits
    FLASH->ACR |= FLASH_ACR_LATENCY_1WS;       // 1 wait state (24-48 MHz)
    FLASH->ACR |= FLASH_ACR_PRFTBE;          // Enable prefetch buffer
    
    // 3. Configure PLL: HSI as source, multiply by 4
    RCC->CFGR &= ~RCC_CFGR_PLLSRC_Pos;           // Clear PLL source bits
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_PREDIV; // HSI as PLL source
    RCC->CFGR &= ~RCC_CFGR_PLLMUL_Pos;           // Clear PLL multiplier bits
    RCC->CFGR |= RCC_CFGR_PLLMUL4;           // PLL multiplier = 4 (8MHz * 4 = 32MHz)
    
    // 4. Set AHB and APB prescalers to 1 (no division)
    RCC->CFGR &= ~RCC_CFGR_HPRE_Pos;             // AHB prescaler = /1
    RCC->CFGR &= ~RCC_CFGR_PPRE_Pos;             // APB prescaler = /1
    
    // 5. Enable PLL and wait for ready
    RCC->CR |= RCC_CR_PLLON_Pos;                 // Enable PLL
    while (!(RCC->CR & RCC_CR_PLLRDY_Pos));      // Wait for PLL ready
    
    // 6. Select PLL as system clock source
    RCC->CFGR &= ~RCC_CFGR_SW_Pos;               // Clear system clock switch
    RCC->CFGR |= RCC_CFGR_SW_PLL;            // Select PLL as system clock
    while ((RCC->CFGR & RCC_CFGR_SWS_Pos) != RCC_CFGR_SWS_PLL); // Wait for switch
    
    // Optional: Trim HSI for better accuracy if needed
    // RCC->CR &= ~RCC_CR_HSITRIM;           // Clear trim bits
    // RCC->CR |= (16 << 3);                 // Default trim value = 16
}