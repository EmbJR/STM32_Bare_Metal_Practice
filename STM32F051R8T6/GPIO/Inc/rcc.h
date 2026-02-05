#ifndef RCC_H
#define RCC_H

#include <stdint.h>
#include <stdbool.h>

// RCC Base Address
#define RCC_BASE        0x40021000UL

// Register offsets
#define RCC_CR          (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR        (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CIR         (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_APB2RSTR    (*(volatile uint32_t *)(RCC_BASE + 0x0C))
#define RCC_APB1RSTR    (*(volatile uint32_t *)(RCC_BASE + 0x10))
#define RCC_AHBENR      (*(volatile uint32_t *)(RCC_BASE + 0x14))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x1C))
#define RCC_BDCR        (*(volatile uint32_t *)(RCC_BASE + 0x20))
#define RCC_CSR         (*(volatile uint32_t *)(RCC_BASE + 0x24))
#define RCC_CFGR2       (*(volatile uint32_t *)(RCC_BASE + 0x2C))
#define RCC_CFGR3       (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_CR2         (*(volatile uint32_t *)(RCC_BASE + 0x34))

// Clock sources
typedef enum {
    CLOCK_SOURCE_HSI = 0,
    CLOCK_SOURCE_HSE = 1,
    CLOCK_SOURCE_PLL = 2,
    CLOCK_SOURCE_HSI48 = 3  // For F04x/F07x/F09x only
} ClockSource;

// PLL sources
typedef enum {
    PLL_SOURCE_HSI_DIV2 = 0,
    PLL_SOURCE_HSE = 1,
    PLL_SOURCE_HSE_DIV2 = 2,
    PLL_SOURCE_HSI48_DIV2 = 3  // For F04x/F07x/F09x only
} PLLSource;

// System clock frequencies
typedef enum {
    SYSTEM_CLOCK_8MHZ = 8000000UL,
    SYSTEM_CLOCK_16MHZ = 16000000UL,
    SYSTEM_CLOCK_24MHZ = 24000000UL,
    SYSTEM_CLOCK_32MHZ = 32000000UL,
    SYSTEM_CLOCK_48MHZ = 48000000UL
} SystemClockFreq;

// AHB prescalers
typedef enum {
    AHB_PRESCALER_1 = 0,
    AHB_PRESCALER_2 = 8,
    AHB_PRESCALER_4 = 9,
    AHB_PRESCALER_8 = 10,
    AHB_PRESCALER_16 = 11,
    AHB_PRESCALER_64 = 12,
    AHB_PRESCALER_128 = 13,
    AHB_PRESCALER_256 = 14,
    AHB_PRESCALER_512 = 15
} AHBPrescaler;

// APB prescalers
typedef enum {
    APB_PRESCALER_1 = 0,
    APB_PRESCALER_2 = 4,
    APB_PRESCALER_4 = 5,
    APB_PRESCALER_8 = 6,
    APB_PRESCALER_16 = 7
} APBPrescaler;

// RCC Configuration structure
typedef struct {
    ClockSource system_clock_source;
    SystemClockFreq target_frequency;
    bool hse_enabled;
    bool pll_enabled;
    PLLSource pll_source;
    uint8_t pll_multiplier;  // 2-16 (refer to PLLMUL bits)
    AHBPrescaler ahb_prescaler;
    APBPrescaler apb_prescaler;
    bool hsi48_enabled;      // For F04x/F07x/F09x only
    bool css_enabled;        // Clock Security System
} RCC_Config;

// Function prototypes
void RCC_Init(const RCC_Config *config);
void RCC_EnableHSI(void);
void RCC_EnableHSE(void);
void RCC_EnablePLL(void);
void RCC_EnableHSI48(void);  // For F04x/F07x/F09x only
void RCC_DisableHSI(void);
void RCC_DisableHSE(void);
void RCC_DisablePLL(void);
void RCC_DisableHSI48(void); // For F04x/F07x/F09x only
void RCC_SetSystemClockSource(ClockSource source);
void RCC_SetAHBPrescaler(AHBPrescaler prescaler);
void RCC_SetAPBPrescaler(APBPrescaler prescaler);
void RCC_SetPLLConfig(PLLSource source, uint8_t multiplier);
uint32_t RCC_GetSystemClockFrequency(void);
void RCC_EnablePeripheralClock(uint8_t peripheral_type, uint8_t peripheral_num);
void RCC_DisablePeripheralClock(uint8_t peripheral_type, uint8_t peripheral_num);

// Peripheral types for clock control
#define PERIPH_GPIOA    0x01
#define PERIPH_GPIOB    0x02
#define PERIPH_GPIOC    0x04
#define PERIPH_GPIOD    0x08
#define PERIPH_GPIOE    0x10
#define PERIPH_GPIOF    0x20
#define PERIPH_DMA1     0x80
#define PERIPH_DMA2     0x81  // For F09x only
#define PERIPH_TIM1     0x90
#define PERIPH_TIM2     0x91
#define PERIPH_TIM3     0x92
#define PERIPH_TIM14    0x93
#define PERIPH_TIM15    0x94
#define PERIPH_TIM16    0x95
#define PERIPH_TIM17    0x96
#define PERIPH_USART1   0xA0
#define PERIPH_USART2   0xA1
#define PERIPH_USART3   0xA2
#define PERIPH_SPI1     0xB0
#define PERIPH_SPI2     0xB1
#define PERIPH_I2C1     0xC0
#define PERIPH_I2C2     0xC1
#define PERIPH_ADC      0xD0
#define PERIPH_DAC      0xD1
#define PERIPH_USB      0xE0  // For F04x/F07x only

#endif // RCC_H
