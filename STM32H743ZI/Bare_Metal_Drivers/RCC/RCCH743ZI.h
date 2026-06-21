#ifndef RCC_H743ZI_H
#define RCC_H743ZI_H

#include <stdint.h>

#define RCC_BASE                0x58024400UL

#define RCC_CR                  (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR                (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR2               (*(volatile uint32_t *)(RCC_BASE + 0x2C))
#define RCC_CFGR3               (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_PLLCFGR             (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_PLLCSELR            (*(volatile uint32_t *)(RCC_BASE + 0x28))

#define RCC_D1CFGR              (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_D2CFGR              (*(volatile uint32_t *)(RCC_BASE + 0x1C))
#define RCC_D3CFGR              (*(volatile uint32_t *)(RCC_BASE + 0x20))

#define RCC_AHB1ENR             (*(volatile uint32_t *)(RCC_BASE + 0x48))
#define RCC_AHB2ENR             (*(volatile uint32_t *)(RCC_BASE + 0x4C))
#define RCC_AHB3ENR             (*(volatile uint32_t *)(RCC_BASE + 0x50))
#define RCC_AHB4ENR             (*(volatile uint32_t *)(RCC_BASE + 0x54))

#define RCC_APB1LENR             (*(volatile uint32_t *)(RCC_BASE + 0x58))
#define RCC_APB1HENR             (*(volatile uint32_t *)(RCC_BASE + 0x5C))
#define RCC_APB2ENR              (*(volatile uint32_t *)(RCC_BASE + 0x60))
#define RCC_APB3ENR              (*(volatile uint32_t *)(RCC_BASE + 0x64))
#define RCC_APB4ENR              (*(volatile uint32_t *)(RCC_BASE + 0x68))

#define RCC_BDCR                (*(volatile uint32_t *)(RCC_BASE + 0x70))
#define RCC_CSR                  (*(volatile uint32_t *)(RCC_BASE + 0x74))
#define RCC_RSR                  (*(volatile uint32_t *)(RCC_BASE + 0x90))

#define PWR_BASE                0x40007000UL
#define PWR_CR1                 (*(volatile uint32_t *)(PWR_BASE + 0x00))
#define PWR_CR2                 (*(volatile uint32_t *)(PWR_BASE + 0x04))
#define PWR_CR3                 (*(volatile uint32_t *)(PWR_BASE + 0x08))
#define PWR_CR4                 (*(volatile uint32_t *)(PWR_BASE + 0x0C))
#define PWR_SR1                 (*(volatile uint32_t *)(PWR_BASE + 0x10))
#define PWR_SR2                 (*(volatile uint32_t *)(PWR_BASE + 0x14))

#define FLASH_BASE              0x40022000UL
#define FLASH_ACR               (*(volatile uint32_t *)(FLASH_BASE + 0x00))

#define SYSTICK_BASE            0xE000ED10UL
#define SYSTICK_LOAD            (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_VAL             (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_CTRL            (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))
#define SYSTICK_CALIB           (*(volatile uint32_t *)(SYSTICK_BASE + 0x0C))

#define CPUID_BASE              0xE000ED00UL
#define CPUID                   (*(volatile uint32_t *)(CPUID_BASE + 0x00))

typedef enum {
    CLOCK_SOURCE_HSI = 0,
    CLOCK_SOURCE_CSI = 1,
    CLOCK_SOURCE_HSE = 2,
    CLOCK_SOURCE_PLL1 = 3
} SystemClockSource;

typedef enum {
    PLL_SOURCE_HSI = 0,
    PLL_SOURCE_CSI = 1,
    PLL_SOURCE_HSE = 2,
    PLL_SOURCE_DISABLED = 3
} PLLSource;

typedef enum {
    OSCILLATOR_HSI,
    OSCILLATOR_HSE,
    OSCILLATOR_CSI,
    OSCILLATOR_LSI,
    OSCILLATOR_LSE,
    OSCILLATOR_HSI48
} OscillatorType;

typedef enum {
    VOLTAGE_SCALE1,   // 480 MHz @ 1.25V
    VOLTAGE_SCALE2,   // 400 MHz @ 1.15V
    VOLTAGE_SCALE3,   // 300 MHz @ 1.05V
    VOLTAGE_SCALE4    // 200 MHz @ 0.95V
} VoltageScale;

typedef enum {
    AHB_DIV_1   = 0,
    AHB_DIV_2   = 8,
    AHB_DIV_4   = 9,
    AHB_DIV_8   = 10,
    AHB_DIV_16  = 11,
    AHB_DIV_64  = 12,
    AHB_DIV_128 = 13,
    AHB_DIV_256 = 14,
    AHB_DIV_512 = 15
} AHBPrescaler;

typedef enum {
    APB_DIV_1  = 0,
    APB_DIV_2  = 4,
    APB_DIV_4  = 5,
    APB_DIV_8  = 6,
    APB_DIV_16 = 7
} APBPrescaler;

typedef enum {
    CLOCK_OK = 0,
    CLOCK_TIMEOUT,
    CLOCK_INVALID_PARAM
} ClockStatus;

typedef struct {
    SystemClockSource source;
    uint32_t sysclk_freq;
    AHBPrescaler ahb_prescaler;
    APBPrescaler apb1_prescaler;
    APBPrescaler apb2_prescaler;
    APBPrescaler apb3_prescaler;
    APBPrescaler apb4_prescaler;
} RCC_ClockConfig;

typedef struct {
    PLLSource pll_source;
    uint32_t pll_m;
    uint32_t pll_n;
    uint32_t pll_p;
    uint32_t pll_q;
    uint32_t pll_r;
} PLL_Config;

#define HSI_FREQUENCY           64000000UL
#define CSI_FREQUENCY           4000000UL
#define LSI_FREQUENCY           32000UL
#define LSE_FREQUENCY           32768UL
#define HSI48_FREQUENCY         48000000UL

#define HSE_FREQUENCY_MIN       4000000UL
#define HSE_FREQUENCY_MAX       48000000UL

#define MAX_SYSCLK_FREQUENCY    480000000UL
#define MIN_SYSCLK_FREQUENCY     4000000UL

#define CR_HSION_Pos            0
#define CR_HSIRDY_Pos           1
#define CR_HSI48ON_Pos          8
#define CR_HSI48RDY_Pos         9
#define CR_CSION_Pos            2
#define CR_CSIRDY_Pos           3
#define CR_HSEON_Pos            8
#define CR_HSERDY_Pos           9
#define CR_HSEBYP_Pos           18
#define CR_HSECSSON_Pos         19
#define CR_PLL1ON_Pos           24
#define CR_PLL1RDY_Pos          25
#define CR_PLL2ON_Pos           26
#define CR_PLL2RDY_Pos          27
#define CR_PLL3ON_Pos           28
#define CR_PLL3RDY_Pos          29

#define CR_HSION                (1U << CR_HSION_Pos)
#define CR_HSIRDY               (1U << CR_HSIRDY_Pos)
#define CR_HSI48ON              (1U << CR_HSI48ON_Pos)
#define CR_HSI48RDY             (1U << CR_HSI48RDY_Pos)
#define CR_CSION                (1U << CR_CSION_Pos)
#define CR_CSIRDY               (1U << CR_CSIRDY_Pos)
#define CR_HSEON                (1U << CR_HSEON_Pos)
#define CR_HSERDY               (1U << CR_HSERDY_Pos)
#define CR_HSEBYP               (1U << CR_HSEBYP_Pos)
#define CR_HSECSSON             (1U << CR_HSECSSON_Pos)
#define CR_PLL1ON               (1U << CR_PLL1ON_Pos)
#define CR_PLL1RDY              (1U << CR_PLL1RDY_Pos)
#define CR_PLL2ON               (1U << CR_PLL2ON_Pos)
#define CR_PLL2RDY              (1U << CR_PLL2RDY_Pos)
#define CR_PLL3ON               (1U << CR_PLL3ON_Pos)
#define CR_PLL3RDY              (1U << CR_PLL3RDY_Pos)

#define CFGR_SW_Pos             0
#define CFGR_SWS_Pos            2
#define CFGR_SW_MASK            0x3U
#define CFGR_SWS_MASK           0x3U
#define CFGR_SW_HSI             0
#define CFGR_SW_CSI             1
#define CFGR_SW_HSE             2
#define CFGR_SW_PLL1            3

#define PLLCFGR_PLL1M_Pos       4
#define PLLCFGR_PLL1N_Pos       8
#define PLLCFGR_PLL1P_Pos       16
#define PLLCFGR_PLL1Q_Pos       20
#define PLLCFGR_PLL1R_Pos       24
#define PLLCFGR_PLL1M_MASK      0x3FU
#define PLLCFGR_PLL1N_MASK      0x1FFU
#define PLLCFGR_PLL1P_MASK      0x7FU
#define PLLCFGR_PLLSRC_Pos      0
#define PLLCFGR_PLLSRC_MASK     0x3U

#define D1CFGR_HPRE_Pos         0
#define D1CFGR_D1PPRE_Pos      4
#define D1CFGR_D1PPRE_MASK      0x7U
#define D2CFGR_D2PPRE1_Pos      4
#define D2CFGR_D2PPRE2_Pos      8
#define D2CFGR_D2PPRE1_MASK     0x7U
#define D2CFGR_D2PPRE2_MASK     0x7U
#define D3CFGR_D3PPRE_Pos       4
#define D3CFGR_D3PPRE_MASK      0x7U

#define BDCR_LSEON_Pos          0
#define BDCR_LSERDY_Pos         1
#define BDCR_LSEBYP_Pos         2
#define BDCR_LSEON              (1U << BDCR_LSEON_Pos)
#define BDCR_LSERDY             (1U << BDCR_LSERDY_Pos)
#define BDCR_LSEBYP             (1U << BDCR_LSEBYP_Pos)

#define CSR_LSION_Pos           0
#define CSR_LSIRDY_Pos          1
#define CSR_LSION               (1U << CSR_LSION_Pos)
#define CSR_LSIRDY              (1U << CSR_LSIRDY_Pos)

#define FLASH_ACR_LATENCY_Pos          0
#define FLASH_ACR_LATENCY_MASK         0xFU
#define FLASH_ACR_WRHIGHFREQ_Pos        4
#define FLASH_ACR_WRHIGHFREQ_MASK      0x3U
#define FLASH_ACR_LATENCY_0WS          0x0U
#define FLASH_ACR_LATENCY_1WS          0x1U
#define FLASH_ACR_LATENCY_2WS          0x2U
#define FLASH_ACR_LATENCY_3WS          0x3U
#define FLASH_ACR_LATENCY_4WS          0x4U
#define FLASH_ACR_LATENCY_5WS          0x5U
#define FLASH_ACR_LATENCY_6WS          0x6U
#define FLASH_ACR_LATENCY_7WS          0x7U

#define PWR_CR1_VOS_Pos         9
#define PWR_CR1_VOS_MASK        0x3U
#define PWR_CR1_VOS_SCALE1      0x1U
#define PWR_CR1_VOS_SCALE2      0x2U
#define PWR_CR1_VOS_SCALE3      0x3U

#define PWR_CR5_R1MODE_Pos      8
#define PWR_CR5_R1MODE_MASK     0x1U

void RCC_EnableHSI(void);
void RCC_DisableHSI(void);
uint32_t RCC_IsHSIReady(void);

void RCC_EnableCSI(void);
void RCC_DisableCSI(void);
uint32_t RCC_IsCSIReady(void);

void RCC_EnableHSE(void);
void RCC_DisableHSE(void);
uint32_t RCC_IsHSEReady(void);

void RCC_EnableLSI(void);
void RCC_DisableLSI(void);
uint32_t RCC_IsLSIReady(void);

void RCC_EnableLSE(void);
void RCC_DisableLSE(void);
uint32_t RCC_IsLSEReady(void);

void RCC_EnableHSI48(void);
void RCC_DisableHSI48(void);
uint32_t RCC_IsHSI48Ready(void);

void RCC_EnablePLL1(void);
void RCC_DisablePLL1(void);
uint32_t RCC_IsPLL1Ready(void);

void RCC_EnablePLL2(void);
void RCC_DisablePLL2(void);
uint32_t RCC_IsPLL2Ready(void);

void RCC_EnablePLL3(void);
void RCC_DisablePLL3(void);
uint32_t RCC_IsPLL3Ready(void);

void RCC_ConfigurePLL1(const PLL_Config *config);
void RCC_ConfigurePLL2(const PLL_Config *config);
void RCC_ConfigurePLL3(const PLL_Config *config);

void RCC_SetSystemClock(SystemClockSource source);
SystemClockSource RCC_GetSystemClockSource(void);

void RCC_SetAHBPrescaler(AHBPrescaler prescaler);
void RCC_SetAPB1Prescaler(APBPrescaler prescaler);
void RCC_SetAPB2Prescaler(APBPrescaler prescaler);
void RCC_SetAPB3Prescaler(APBPrescaler prescaler);
void RCC_SetAPB4Prescaler(APBPrescaler prescaler);

uint32_t RCC_GetSYSCLKFrequency(void);
uint32_t RCC_GetHCLKFrequency(void);
uint32_t RCC_GetPCLK1Frequency(void);
uint32_t RCC_GetPCLK2Frequency(void);

void RCC_SetVoltageScale(VoltageScale scale);
VoltageScale RCC_GetVoltageScale(void);

void RCC_ConfigureFlashLatency(uint32_t sysclk_freq);
void RCC_SetPowerMode(VoltageScale voltage_scale);

void RCC_SystemClockConfig_HSI(uint32_t sysclk);
void RCC_SystemClockConfig_HSE(uint32_t sysclk);
void RCC_SystemClockConfig_PLL(PLL_Config *pll_config, uint32_t sysclk);
void RCC_SystemClockConfig_MSI(void);

ClockStatus RCC_WaitForHSIReady(uint32_t timeout);
ClockStatus RCC_WaitForHSEReady(uint32_t timeout);
ClockStatus RCC_WaitForPLL1Ready(uint32_t timeout);

void RCC_EnablePeripheralClock(uint32_t periph_base);
void RCC_DisablePeripheralClock(uint32_t periph_base);

#endif
