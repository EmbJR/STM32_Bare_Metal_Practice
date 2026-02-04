#ifndef RCC_H
#define RCC_H

#include <stdint.h>

/* CMSIS-style memory access macros */
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read/write' permissions */

/* RCC Base Address */
#define RCC_BASE        0x40021000UL

/* RCC Register Structure Definition */
typedef struct {
    __IO uint32_t CR;          /*!< RCC clock control register,                 Address offset: 0x00 */
    __IO uint32_t CFGR;        /*!< RCC clock configuration register,           Address offset: 0x04 */
    __IO uint32_t CIR;         /*!< RCC clock interrupt register,               Address offset: 0x08 */
    __IO uint32_t APB2RSTR;    /*!< RCC APB2 peripheral reset register,         Address offset: 0x0C */
    __IO uint32_t APB1RSTR;    /*!< RCC APB1 peripheral reset register,         Address offset: 0x10 */
    __IO uint32_t AHBENR;      /*!< RCC AHB peripheral clock enable register,   Address offset: 0x14 */
    __IO uint32_t APB2ENR;     /*!< RCC APB2 peripheral clock enable register,  Address offset: 0x18 */
    __IO uint32_t APB1ENR;     /*!< RCC APB1 peripheral clock enable register,  Address offset: 0x1C */
    __IO uint32_t BDCR;        /*!< RCC RTC domain control register,            Address offset: 0x20 */
    __IO uint32_t CSR;         /*!< RCC control/status register,                Address offset: 0x24 */
    __IO uint32_t AHBRSTR;     /*!< RCC AHB peripheral reset register,          Address offset: 0x28 */
    __IO uint32_t CFGR2;       /*!< RCC clock configuration register 2,         Address offset: 0x2C */
    __IO uint32_t CFGR3;       /*!< RCC clock configuration register 3,         Address offset: 0x30 */
    __IO uint32_t CR2;         /*!< RCC clock control register 2,               Address offset: 0x34 */
} RCC_TypeDef;

/* RCC Peripheral Declaration */
#define RCC             ((RCC_TypeDef *) RCC_BASE)

/* ============================ RCC_CR Register Bits ============================ */
/* Clock Control Register */
#define RCC_CR_HSION_Pos            (0U)                                 
#define RCC_CR_HSION_Msk            (0x1UL << RCC_CR_HSION_Pos)          /*!< HSI clock enable */
#define RCC_CR_HSIRDY_Pos           (1U)                                 
#define RCC_CR_HSIRDY_Msk           (0x1UL << RCC_CR_HSIRDY_Pos)         /*!< HSI clock ready flag */

#define RCC_CR_HSITRIM_Pos          (3U)                                 
#define RCC_CR_HSITRIM_Msk          (0x1FUL << RCC_CR_HSITRIM_Pos)       /*!< HSI clock trimming */

#define RCC_CR_HSICAL_Pos           (8U)                                 
#define RCC_CR_HSICAL_Msk           (0xFFUL << RCC_CR_HSICAL_Pos)        /*!< HSI clock calibration */

#define RCC_CR_HSEON_Pos            (16U)                                
#define RCC_CR_HSEON_Msk            (0x1UL << RCC_CR_HSEON_Pos)          /*!< HSE clock enable */
#define RCC_CR_HSERDY_Pos           (17U)                                
#define RCC_CR_HSERDY_Msk           (0x1UL << RCC_CR_HSERDY_Pos)         /*!< HSE clock ready flag */
#define RCC_CR_HSEBYP_Pos           (18U)                                
#define RCC_CR_HSEBYP_Msk           (0x1UL << RCC_CR_HSEBYP_Pos)         /*!< HSE clock bypass */

#define RCC_CR_CSSON_Pos            (19U)                                
#define RCC_CR_CSSON_Msk            (0x1UL << RCC_CR_CSSON_Pos)          /*!< Clock security system enable */

#define RCC_CR_PLLON_Pos            (24U)                                
#define RCC_CR_PLLON_Msk            (0x1UL << RCC_CR_PLLON_Pos)          /*!< PLL enable */
#define RCC_CR_PLLRDY_Pos           (25U)                                
#define RCC_CR_PLLRDY_Msk           (0x1UL << RCC_CR_PLLRDY_Pos)         /*!< PLL clock ready flag */

/* ============================ RCC_CFGR Register Bits ============================ */
/* Clock Configuration Register */
/* System clock switch */
#define RCC_CFGR_SW_Pos             (0U)                                 
#define RCC_CFGR_SW_Msk             (0x3UL << RCC_CFGR_SW_Pos)           
#define RCC_CFGR_SW_HSI             (0x0UL << RCC_CFGR_SW_Pos)           /*!< HSI selected as system clock */
#define RCC_CFGR_SW_HSE             (0x1UL << RCC_CFGR_SW_Pos)           /*!< HSE selected as system clock */
#define RCC_CFGR_SW_PLL             (0x2UL << RCC_CFGR_SW_Pos)           /*!< PLL selected as system clock */
#define RCC_CFGR_SW_HSI48           (0x3UL << RCC_CFGR_SW_Pos)           /*!< HSI48 selected as system clock */

/* System clock switch status */
#define RCC_CFGR_SWS_Pos            (2U)                                 
#define RCC_CFGR_SWS_Msk            (0x3UL << RCC_CFGR_SWS_Pos)          
#define RCC_CFGR_SWS_HSI            (0x0UL << RCC_CFGR_SWS_Pos)          /*!< HSI used as system clock */
#define RCC_CFGR_SWS_HSE            (0x1UL << RCC_CFGR_SWS_Pos)          /*!< HSE used as system clock */
#define RCC_CFGR_SWS_PLL            (0x2UL << RCC_CFGR_SWS_Pos)          /*!< PLL used as system clock */
#define RCC_CFGR_SWS_HSI48          (0x3UL << RCC_CFGR_SWS_Pos)          /*!< HSI48 used as system clock */

/* AHB prescaler */
#define RCC_CFGR_HPRE_Pos           (4U)                                 
#define RCC_CFGR_HPRE_Msk           (0xFUL << RCC_CFGR_HPRE_Pos)         
#define RCC_CFGR_HPRE_DIV1          (0x0UL << RCC_CFGR_HPRE_Pos)         /*!< SYSCLK not divided */
#define RCC_CFGR_HPRE_DIV2          (0x8UL << RCC_CFGR_HPRE_Pos)         /*!< SYSCLK divided by 2 */
#define RCC_CFGR_HPRE_DIV4          (0x9UL << RCC_CFGR_HPRE_Pos)         /*!< SYSCLK divided by 4 */
#define RCC_CFGR_HPRE_DIV8          (0xAUL << RCC_CFGR_HPRE_Pos)         /*!< SYSCLK divided by 8 */
#define RCC_CFGR_HPRE_DIV16         (0xBUL << RCC_CFGR_HPRE_Pos)         /*!< SYSCLK divided by 16 */
#define RCC_CFGR_HPRE_DIV64         (0xCUL << RCC_CFGR_HPRE_Pos)         /*!< SYSCLK divided by 64 */
#define RCC_CFGR_HPRE_DIV128        (0xDUL << RCC_CFGR_HPRE_Pos)         /*!< SYSCLK divided by 128 */
#define RCC_CFGR_HPRE_DIV256        (0xEUL << RCC_CFGR_HPRE_Pos)         /*!< SYSCLK divided by 256 */
#define RCC_CFGR_HPRE_DIV512        (0xFUL << RCC_CFGR_HPRE_Pos)         /*!< SYSCLK divided by 512 */

/* APB prescaler */
#define RCC_CFGR_PPRE_Pos           (8U)                                 
#define RCC_CFGR_PPRE_Msk           (0x7UL << RCC_CFGR_PPRE_Pos)         
#define RCC_CFGR_PPRE_DIV1          (0x0UL << RCC_CFGR_PPRE_Pos)         /*!< HCLK not divided */
#define RCC_CFGR_PPRE_DIV2          (0x4UL << RCC_CFGR_PPRE_Pos)         /*!< HCLK divided by 2 */
#define RCC_CFGR_PPRE_DIV4          (0x5UL << RCC_CFGR_PPRE_Pos)         /*!< HCLK divided by 4 */
#define RCC_CFGR_PPRE_DIV8          (0x6UL << RCC_CFGR_PPRE_Pos)         /*!< HCLK divided by 8 */
#define RCC_CFGR_PPRE_DIV16         (0x7UL << RCC_CFGR_PPRE_Pos)         /*!< HCLK divided by 16 */

/* ADC prescaler */
#define RCC_CFGR_ADCPRE_Pos         (14U)                                
#define RCC_CFGR_ADCPRE_Msk         (0x1UL << RCC_CFGR_ADCPRE_Pos)       
#define RCC_CFGR_ADCPRE_DIV2        (0x0UL << RCC_CFGR_ADCPRE_Pos)       /*!< PCLK divided by 2 for ADC */
#define RCC_CFGR_ADCPRE_DIV4        (0x1UL << RCC_CFGR_ADCPRE_Pos)       /*!< PCLK divided by 4 for ADC */

/* PLL entry clock source */
#define RCC_CFGR_PLLSRC_Pos         (16U)                                
#define RCC_CFGR_PLLSRC_Msk         (0x3UL << RCC_CFGR_PLLSRC_Pos)       
#define RCC_CFGR_PLLSRC_HSI_DIV2    (0x0UL << RCC_CFGR_PLLSRC_Pos)       /*!< HSI oscillator clock divided by 2 selected as PLL input clock */
#define RCC_CFGR_PLLSRC_HSI_PREDIV  (0x1UL << RCC_CFGR_PLLSRC_Pos)       /*!< HSI oscillator clock selected as PLL input clock */
#define RCC_CFGR_PLLSRC_HSE_PREDIV  (0x2UL << RCC_CFGR_PLLSRC_Pos)       /*!< HSE oscillator clock selected as PLL input clock */
#define RCC_CFGR_PLLSRC_HSE_DIV2    (0x3UL << RCC_CFGR_PLLSRC_Pos)       /*!< HSE oscillator clock divided by 2 selected as PLL input clock */

/* HSE divider for PLL entry */
#define RCC_CFGR_PLLXTPRE_Pos       (19U)                                
#define RCC_CFGR_PLLXTPRE_Msk       (0x1UL << RCC_CFGR_PLLXTPRE_Pos)     
#define RCC_CFGR_PLLXTPRE_HSE_DIV2  (0x1UL << RCC_CFGR_PLLXTPRE_Pos)     /*!< HSE clock divided by 2 for PLL entry */

/* PLL multiplication factor */
#define RCC_CFGR_PLLMUL_Pos         (20U)                                
#define RCC_CFGR_PLLMUL_Msk         (0xFUL << RCC_CFGR_PLLMUL_Pos)       
#define RCC_CFGR_PLLMUL2            (0x0UL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 2 */
#define RCC_CFGR_PLLMUL3            (0x1UL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 3 */
#define RCC_CFGR_PLLMUL4            (0x2UL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 4 */
#define RCC_CFGR_PLLMUL5            (0x3UL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 5 */
#define RCC_CFGR_PLLMUL6            (0x4UL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 6 */
#define RCC_CFGR_PLLMUL7            (0x5UL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 7 */
#define RCC_CFGR_PLLMUL8            (0x6UL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 8 */
#define RCC_CFGR_PLLMUL9            (0x7UL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 9 */
#define RCC_CFGR_PLLMUL10           (0x8UL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 10 */
#define RCC_CFGR_PLLMUL11           (0x9UL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 11 */
#define RCC_CFGR_PLLMUL12           (0xAUL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 12 */
#define RCC_CFGR_PLLMUL13           (0xBUL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 13 */
#define RCC_CFGR_PLLMUL14           (0xCUL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 14 */
#define RCC_CFGR_PLLMUL15           (0xDUL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 15 */
#define RCC_CFGR_PLLMUL16           (0xEUL << RCC_CFGR_PLLMUL_Pos)       /*!< PLL input clock x 16 */

/* Microcontroller clock output */
#define RCC_CFGR_MCO_Pos            (24U)                                
#define RCC_CFGR_MCO_Msk            (0xFUL << RCC_CFGR_MCO_Pos)          
#define RCC_CFGR_MCO_NOCLOCK        (0x0UL << RCC_CFGR_MCO_Pos)          /*!< No clock */
#define RCC_CFGR_MCO_HSI14          (0x1UL << RCC_CFGR_MCO_Pos)          /*!< HSI14 clock selected */
#define RCC_CFGR_MCO_LSI            (0x2UL << RCC_CFGR_MCO_Pos)          /*!< LSI clock selected */
#define RCC_CFGR_MCO_LSE            (0x3UL << RCC_CFGR_MCO_Pos)          /*!< LSE clock selected */
#define RCC_CFGR_MCO_SYSCLK         (0x4UL << RCC_CFGR_MCO_Pos)          /*!< System clock selected */
#define RCC_CFGR_MCO_HSI            (0x5UL << RCC_CFGR_MCO_Pos)          /*!< HSI clock selected */
#define RCC_CFGR_MCO_HSE            (0x6UL << RCC_CFGR_MCO_Pos)          /*!< HSE clock selected */
#define RCC_CFGR_MCO_PLL            (0x7UL << RCC_CFGR_MCO_Pos)          /*!< PLL clock selected */
#define RCC_CFGR_MCO_HSI48          (0x8UL << RCC_CFGR_MCO_Pos)          /*!< HSI48 clock selected */

/* MCO prescaler */
#define RCC_CFGR_MCOPRE_Pos         (28U)                                
#define RCC_CFGR_MCOPRE_Msk         (0x7UL << RCC_CFGR_MCOPRE_Pos)       
#define RCC_CFGR_MCOPRE_DIV1        (0x0UL << RCC_CFGR_MCOPRE_Pos)       /*!< MCO divided by 1 */
#define RCC_CFGR_MCOPRE_DIV2        (0x1UL << RCC_CFGR_MCOPRE_Pos)       /*!< MCO divided by 2 */
#define RCC_CFGR_MCOPRE_DIV4        (0x2UL << RCC_CFGR_MCOPRE_Pos)       /*!< MCO divided by 4 */
#define RCC_CFGR_MCOPRE_DIV8        (0x3UL << RCC_CFGR_MCOPRE_Pos)       /*!< MCO divided by 8 */
#define RCC_CFGR_MCOPRE_DIV16       (0x4UL << RCC_CFGR_MCOPRE_Pos)       /*!< MCO divided by 16 */
#define RCC_CFGR_MCOPRE_DIV32       (0x5UL << RCC_CFGR_MCOPRE_Pos)       /*!< MCO divided by 32 */
#define RCC_CFGR_MCOPRE_DIV64       (0x6UL << RCC_CFGR_MCOPRE_Pos)       /*!< MCO divided by 64 */
#define RCC_CFGR_MCOPRE_DIV128      (0x7UL << RCC_CFGR_MCOPRE_Pos)       /*!< MCO divided by 128 */

/* PLL clock not divided for MCO */
#define RCC_CFGR_PLLNODIV_Pos       (31U)                                
#define RCC_CFGR_PLLNODIV_Msk       (0x1UL << RCC_CFGR_PLLNODIV_Pos)     
#define RCC_CFGR_PLLNODIV           RCC_CFGR_PLLNODIV_Msk               /*!< PLL clock not divided for MCO */

/* ============================ Helper Macros ============================ */
/* Bit manipulation macros */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  \
    ((REG) = (((REG) & (~(CLEARMASK))) | (SETMASK)))

/* Clock frequency constants */
#define HSI_CLOCK_FREQUENCY     8000000UL    /* 8 MHz */
#define HSI48_CLOCK_FREQUENCY   48000000UL   /* 48 MHz */
#define HSI14_CLOCK_FREQUENCY   14000000UL   /* 14 MHz */
#define LSI_CLOCK_FREQUENCY     40000UL      /* 40 kHz */
#define LSE_CLOCK_FREQUENCY     32768UL      /* 32.768 kHz */

/* Maximum clock frequencies */
#define SYSCLK_MAX_FREQUENCY    48000000UL   /* 48 MHz */
#define HCLK_MAX_FREQUENCY      48000000UL   /* 48 MHz */
#define PCLK_MAX_FREQUENCY      48000000UL   /* 48 MHz */

#endif /* RCC_H */