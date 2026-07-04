#ifndef RCC_H743ZI_H
#define RCC_H743ZI_H

#include <stdint.h>

/* =========================================================================
 *  Device base address
 * ========================================================================= */
#define RCC_BASE_ADDR         0x58024400UL
#define PWR_BASE_ADDR         0x58024800UL
#define FLASH_REG_BASE_ADDR   0x52002000UL

/* =========================================================================
 *  Generic macros
 * ========================================================================= */
#define RCC_REG(off)          (*(volatile uint32_t *)(RCC_BASE_ADDR + (off)))
#define PWR_REG(off)          (*(volatile uint32_t *)(PWR_BASE_ADDR + (off)))
#define FLASH_REG(off)        (*(volatile uint32_t *)(FLASH_REG_BASE_ADDR + (off)))

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))

/* =========================================================================
 *  RCC register offsets
 * ========================================================================= */
#define RCC_CR                0x00U
#define RCC_HSICFGR           0x04U
#define RCC_CRRCR             0x08U
#define RCC_CSICFGR           0x0CU
#define RCC_CFGR              0x10U
#define RCC_D1CFGR            0x18U
#define RCC_D2CFGR            0x1CU
#define RCC_D3CFGR            0x20U
#define RCC_PLLCKSELR         0x28U
#define RCC_PLLCFGR           0x2CU
#define RCC_PLL1DIVR          0x30U
#define RCC_PLL1FRACR         0x34U
#define RCC_PLL2DIVR          0x38U
#define RCC_PLL2FRACR         0x3CU
#define RCC_PLL3DIVR          0x40U
#define RCC_PLL3FRACR         0x44U
#define RCC_D1CCIPR           0x4CU
#define RCC_D2CCIP1R          0x50U
#define RCC_D2CCIP2R          0x54U
#define RCC_D3CCIPR           0x58U
#define RCC_CIER              0x60U
#define RCC_CIFR              0x64U
#define RCC_CICR              0x68U
#define RCC_BDCR              0x70U
#define RCC_CSR               0x74U
#define RCC_AHB3RSTR          0x7CU
#define RCC_AHB1RSTR          0x80U
#define RCC_AHB2RSTR          0x84U
#define RCC_AHB4RSTR          0x88U
#define RCC_APB3RSTR          0x8CU
#define RCC_APB1LRSTR         0x90U
#define RCC_APB1HRSTR         0x94U
#define RCC_APB2RSTR          0x98U
#define RCC_APB4RSTR          0x9CU
#define RCC_RSR               0xD0U
#define RCC_AHB3ENR           0xD4U
#define RCC_AHB1ENR           0xD8U
#define RCC_AHB2ENR           0xDCU
#define RCC_AHB4ENR           0xE0U
#define RCC_APB3ENR           0xE4U
#define RCC_APB1LENR          0xE8U
#define RCC_APB1HENR          0xECU
#define RCC_APB2ENR           0xF0U
#define RCC_APB4ENR           0xF4U

/* PWR and FLASH register offsets needed for clock config */
#define PWR_CR1               0x00U
#define PWR_CR3               0x08U
#define FLASH_ACR             0x00U

/* =========================================================================
 *  RCC_CR bit definitions
 * ========================================================================= */
#define RCC_CR_PLL3RDY        (1U << 29)
#define RCC_CR_PLL3ON         (1U << 28)
#define RCC_CR_PLL2RDY        (1U << 27)
#define RCC_CR_PLL2ON         (1U << 26)
#define RCC_CR_PLL1RDY        (1U << 25)
#define RCC_CR_PLL1ON         (1U << 24)
#define RCC_CR_HSECSSON       (1U << 19)
#define RCC_CR_HSEBYP         (1U << 18)
#define RCC_CR_HSERDY         (1U << 17)
#define RCC_CR_HSEON          (1U << 16)
#define RCC_CR_D2CKRDY        (1U << 15)
#define RCC_CR_D1CKRDY        (1U << 14)
#define RCC_CR_HSI48RDY       (1U << 13)
#define RCC_CR_HSI48ON        (1U << 12)
#define RCC_CR_CSIKERON       (1U << 9)
#define RCC_CR_CSIRDY         (1U << 8)
#define RCC_CR_CSION          (1U << 7)
#define RCC_CR_HSIDIVF        (1U << 5)
#define RCC_CR_HSIDIV_Pos     3U
#define RCC_CR_HSIDIV_Msk     (3U << RCC_CR_HSIDIV_Pos)
#define RCC_CR_HSIDIV_1       (0U << RCC_CR_HSIDIV_Pos)  /* 64 MHz */
#define RCC_CR_HSIDIV_2       (1U << RCC_CR_HSIDIV_Pos)  /* 32 MHz */
#define RCC_CR_HSIDIV_4       (2U << RCC_CR_HSIDIV_Pos)  /* 16 MHz */
#define RCC_CR_HSIDIV_8       (3U << RCC_CR_HSIDIV_Pos)  /*  8 MHz */
#define RCC_CR_HSIRDY         (1U << 2)
#define RCC_CR_HSIKERON       (1U << 1)
#define RCC_CR_HSION          (1U << 0)

/* =========================================================================
 *  RCC_CFGR bit definitions
 * ========================================================================= */
#define RCC_CFGR_MCO2_Pos     29U
#define RCC_CFGR_MCO2_Msk     (7U << RCC_CFGR_MCO2_Pos)
#define RCC_CFGR_MCO2PRE_Pos  25U
#define RCC_CFGR_MCO2PRE_Msk  (0xFU << RCC_CFGR_MCO2PRE_Pos)
#define RCC_CFGR_MCO1_Pos     22U
#define RCC_CFGR_MCO1_Msk     (7U << RCC_CFGR_MCO1_Pos)
#define RCC_CFGR_MCO1PRE_Pos  18U
#define RCC_CFGR_MCO1PRE_Msk  (0xFU << RCC_CFGR_MCO1PRE_Pos)
#define RCC_CFGR_TIMPRE       (1U << 15)
#define RCC_CFGR_HRTIMSEL     (1U << 14)
#define RCC_CFGR_RTCPRE_Pos   8U
#define RCC_CFGR_RTCPRE_Msk   (0x3FU << RCC_CFGR_RTCPRE_Pos)
#define RCC_CFGR_STOPKERWUCK  (1U << 7)
#define RCC_CFGR_STOPWUCK     (1U << 6)
#define RCC_CFGR_SWS_Pos      3U
#define RCC_CFGR_SWS_Msk      (7U << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_SW_Pos       0U
#define RCC_CFGR_SW_Msk       (7U << RCC_CFGR_SW_Pos)

#define RCC_CFGR_SW_HSI       0U
#define RCC_CFGR_SW_CSI       1U
#define RCC_CFGR_SW_HSE       2U
#define RCC_CFGR_SW_PLL1      3U

/* =========================================================================
 *  RCC_D1CFGR / RCC_D2CFGR / RCC_D3CFGR bit definitions
 * ========================================================================= */
#define RCC_D1CFGR_D1CPRE_Pos 8U
#define RCC_D1CFGR_D1CPRE_Msk (0xFU << RCC_D1CFGR_D1CPRE_Pos)
#define RCC_D1CFGR_D1PPRE_Pos 4U
#define RCC_D1CFGR_D1PPRE_Msk (7U << RCC_D1CFGR_D1PPRE_Pos)
#define RCC_D1CFGR_HPRE_Pos   0U
#define RCC_D1CFGR_HPRE_Msk   (0xFU << RCC_D1CFGR_HPRE_Pos)

#define RCC_D2CFGR_D2PPRE2_Pos 8U
#define RCC_D2CFGR_D2PPRE2_Msk (7U << RCC_D2CFGR_D2PPRE2_Pos)
#define RCC_D2CFGR_D2PPRE1_Pos 4U
#define RCC_D2CFGR_D2PPRE1_Msk (7U << RCC_D2CFGR_D2PPRE1_Pos)

#define RCC_D3CFGR_D3PPRE_Pos 4U
#define RCC_D3CFGR_D3PPRE_Msk (7U << RCC_D3CFGR_D3PPRE_Pos)

/* =========================================================================
 *  RCC_PLLCKSELR bit definitions
 * ========================================================================= */
#define RCC_PLLCKSELR_DIVM3_Pos 20U
#define RCC_PLLCKSELR_DIVM3_Msk (0x3FU << RCC_PLLCKSELR_DIVM3_Pos)
#define RCC_PLLCKSELR_DIVM2_Pos 12U
#define RCC_PLLCKSELR_DIVM2_Msk (0x3FU << RCC_PLLCKSELR_DIVM2_Pos)
#define RCC_PLLCKSELR_DIVM1_Pos 4U
#define RCC_PLLCKSELR_DIVM1_Msk (0x3FU << RCC_PLLCKSELR_DIVM1_Pos)
#define RCC_PLLCKSELR_PLLSRC_Pos 0U
#define RCC_PLLCKSELR_PLLSRC_Msk (3U << RCC_PLLCKSELR_PLLSRC_Pos)

#define RCC_PLLSRC_HSI        0U
#define RCC_PLLSRC_CSI        1U
#define RCC_PLLSRC_HSE        2U
#define RCC_PLLSRC_NONE       3U

/* =========================================================================
 *  RCC_PLLCFGR bit definitions
 * ========================================================================= */
#define RCC_PLLCFGR_DIVR3EN   (1U << 24)
#define RCC_PLLCFGR_DIVQ3EN   (1U << 23)
#define RCC_PLLCFGR_DIVP3EN   (1U << 22)
#define RCC_PLLCFGR_DIVR2EN   (1U << 21)
#define RCC_PLLCFGR_DIVQ2EN   (1U << 20)
#define RCC_PLLCFGR_DIVP2EN   (1U << 19)
#define RCC_PLLCFGR_DIVR1EN   (1U << 18)
#define RCC_PLLCFGR_DIVQ1EN   (1U << 17)
#define RCC_PLLCFGR_DIVP1EN   (1U << 16)
#define RCC_PLLCFGR_PLL3RGE_Pos 10U
#define RCC_PLLCFGR_PLL3RGE_Msk (3U << RCC_PLLCFGR_PLL3RGE_Pos)
#define RCC_PLLCFGR_PLL3VCOSEL  (1U << 9)
#define RCC_PLLCFGR_PLL3FRACEN (1U << 8)
#define RCC_PLLCFGR_PLL2RGE_Pos 6U
#define RCC_PLLCFGR_PLL2RGE_Msk (3U << RCC_PLLCFGR_PLL2RGE_Pos)
#define RCC_PLLCFGR_PLL2VCOSEL  (1U << 5)
#define RCC_PLLCFGR_PLL2FRACEN (1U << 4)
#define RCC_PLLCFGR_PLL1RGE_Pos 2U
#define RCC_PLLCFGR_PLL1RGE_Msk (3U << RCC_PLLCFGR_PLL1RGE_Pos)
#define RCC_PLLCFGR_PLL1VCOSEL  (1U << 1)
#define RCC_PLLCFGR_PLL1FRACEN (1U << 0)

/* =========================================================================
 *  RCC_PLLxDIVR bit definitions (PLL1DIVR, PLL2DIVR, PLL3DIVR same layout)
 * ========================================================================= */
#define RCC_PLLDIVR_DIVR_Pos  24U
#define RCC_PLLDIVR_DIVR_Msk  (0x7FU << RCC_PLLDIVR_DIVR_Pos)
#define RCC_PLLDIVR_DIVQ_Pos  16U
#define RCC_PLLDIVR_DIVQ_Msk  (0x7FU << RCC_PLLDIVR_DIVQ_Pos)
#define RCC_PLLDIVR_DIVP_Pos  9U
#define RCC_PLLDIVR_DIVP_Msk  (0x7FU << RCC_PLLDIVR_DIVP_Pos)
#define RCC_PLLDIVR_DIVN_Pos  0U
#define RCC_PLLDIVR_DIVN_Msk  (0x1FFU << RCC_PLLDIVR_DIVN_Pos)

/* =========================================================================
 *  RCC_BDCR bit definitions
 * ========================================================================= */
#define RCC_BDCR_BDRST        (1U << 16)
#define RCC_BDCR_RTCEN        (1U << 15)
#define RCC_BDCR_RTCSEL_Pos   8U
#define RCC_BDCR_RTCSEL_Msk   (3U << RCC_BDCR_RTCSEL_Pos)
#define RCC_BDCR_RTCSEL_NONE  (0U << RCC_BDCR_RTCSEL_Pos)
#define RCC_BDCR_RTCSEL_LSE   (1U << RCC_BDCR_RTCSEL_Pos)
#define RCC_BDCR_RTCSEL_LSI   (2U << RCC_BDCR_RTCSEL_Pos)
#define RCC_BDCR_RTCSEL_HSE   (3U << RCC_BDCR_RTCSEL_Pos)
#define RCC_BDCR_LSECSSD      (1U << 6)
#define RCC_BDCR_LSECSSON     (1U << 5)
#define RCC_BDCR_LSEDRV_Pos   3U
#define RCC_BDCR_LSEDRV_Msk   (3U << RCC_BDCR_LSEDRV_Pos)
#define RCC_BDCR_LSEBYP       (1U << 2)
#define RCC_BDCR_LSERDY       (1U << 1)
#define RCC_BDCR_LSEON        (1U << 0)

/* =========================================================================
 *  RCC_CSR bit definitions
 * ========================================================================= */
#define RCC_CSR_LSIRDY        (1U << 1)
#define RCC_CSR_LSION         (1U << 0)

/* =========================================================================
 *  PWR_CR1 bit definitions
 * ========================================================================= */
#define PWR_CR1_DBP           (1U << 8)

/* =========================================================================
 *  FLASH_ACR bit definitions
 * ========================================================================= */
#define FLASH_ACR_LATENCY_Pos 0U
#define FLASH_ACR_LATENCY_Msk (7U << FLASH_ACR_LATENCY_Pos)
#define FLASH_ACR_WRHIGHFREQ_Pos 4U
#define FLASH_ACR_WRHIGHFREQ_Msk (3U << FLASH_ACR_WRHIGHFREQ_Pos)

/* =========================================================================
 *  Prescaler division codes (for HPRE/D1CPRE/D1PPRE/D2PPRE1/D2PPRE2/D3PPRE)
 *  0xxx => divide by 1, 1000 => 2, 1001 => 4, 1010 => 8, 1011 => 16,
 *  1100 => 64, 1101 => 128, 1110 => 256, 1111 => 512
 * ========================================================================= */
#define AHB_PRESCALER_DIV1    0U
#define AHB_PRESCALER_DIV2    8U
#define AHB_PRESCALER_DIV4    9U
#define AHB_PRESCALER_DIV8    10U
#define AHB_PRESCALER_DIV16   11U
#define AHB_PRESCALER_DIV64   12U
#define AHB_PRESCALER_DIV128  13U
#define AHB_PRESCALER_DIV256  14U
#define AHB_PRESCALER_DIV512  15U

#define APB_PRESCALER_DIV1    0U
#define APB_PRESCALER_DIV2    4U
#define APB_PRESCALER_DIV4    5U
#define APB_PRESCALER_DIV8    6U
#define APB_PRESCALER_DIV16   7U

/* =========================================================================
 *  System clock source enum
 * ========================================================================= */
typedef enum
{
    RCC_SYSCLK_HSI = 0,
    RCC_SYSCLK_CSI,
    RCC_SYSCLK_HSE,
    RCC_SYSCLK_PLL1
} RCC_SysClkSrc_t;

/* =========================================================================
 *  Oscillator frequency constants (Hz)
 *  Adjust HSE_VALUE / HSI_VALUE if your board uses a different crystal.
 * ========================================================================= */
#define HSI_VALUE             64000000UL
#define CSI_VALUE             4000000UL
#define HSE_VALUE             8000000UL
#define LSI_VALUE             32000UL
#define HSI48_VALUE           48000000UL
#define LSE_VALUE             32768UL

/* =========================================================================
 *  Public API
 * ========================================================================= */

/* ---- LSE / RTC control (need PWR DBP bit first) ---------------------- */
void     RCC_LSEConfig(uint8_t enable, uint8_t bypass, uint8_t drive);
uint8_t  RCC_LSEReady(void);
void     RCC_RTCConfig(uint32_t rtcsel);
void     RCC_RTCEnable(uint8_t enable);

/* ---- LSI control ---------------------------------------------------- */
void     RCC_LSIEnable(uint8_t enable);
uint8_t  RCC_LSIRdy(void);

/* ---- HSI control --------------------------------------------------- */
void     RCC_HSIEnable(uint8_t enable);
uint8_t  RCC_HSIRdy(void);
void     RCC_HSIConfig(uint32_t div_code);   /* 0=64M, 1=32M, 2=16M, 3=8M */

/* ---- CSI control --------------------------------------------------- */
void     RCC_CSIEnable(uint8_t enable);
uint8_t  RCC_CSIRdy(void);

/* ---- HSE control --------------------------------------------------- */
void     RCC_HSEEnable(uint8_t enable);
uint8_t  RCC_HSERdy(void);
void     RCC_HSEBypass(uint8_t bypass);

/* ---- PLL1 control -------------------------------------------------- */
typedef struct
{
    uint32_t pll_src;       /* RCC_PLLSRC_HSI / CSI / HSE */
    uint32_t divm;          /* 1..63 (0 = disabled)        */
    uint32_t divn;          /* 4..512                     */
    uint32_t divp;          /* 1..128 (only even; 1 = bypass) */
    uint32_t divq;          /* 1..128 (0 = disabled)      */
    uint32_t divr;          /* 1..128 (0 = disabled)      */
    uint32_t pll_rge;       /* 0=1-2, 1=2-4, 2=4-8, 3=8-16 MHz */
    uint32_t pll_vcosel;    /* 0 = wide (192-836), 1 = medium (150-420) */
    uint8_t  enable_p;
    uint8_t  enable_q;
    uint8_t  enable_r;
} RCC_PLL1_Init_t;

void     RCC_PLL1Config(const RCC_PLL1_Init_t *cfg);
void     RCC_PLL1Enable(uint8_t enable);
uint8_t  RCC_PLL1Ready(void);

/* ---- Bus prescalers ------------------------------------------------ */
void     RCC_SetAHBPrescaler (uint32_t code);
void     RCC_SetD1CPRE       (uint32_t code);
void     RCC_SetD1PPRE       (uint32_t code);
void     RCC_SetD2PPRE1      (uint32_t code);
void     RCC_SetD2PPRE2      (uint32_t code);
void     RCC_SetD3PPRE       (uint32_t code);

/* ---- System clock switch ------------------------------------------- */
void     RCC_SetSysClockSrc  (RCC_SysClkSrc_t src);
RCC_SysClkSrc_t RCC_GetSysClockSrc(void);

/* ---- Flash latency (number of wait states) ------------------------- */
void     RCC_SetFlashLatency(uint32_t wait_states);

/* ---- MCO ----------------------------------------------------------- */
void     RCC_MCO1Config(uint32_t sel, uint32_t prescaler);
void     RCC_MCO2Config(uint32_t sel, uint32_t prescaler);

/* =========================================================================
 *  Peripheral clock enable / reset / disable
 *
 *  Each peripheral on the H743 sits on a specific bus:
 *      AHB1, AHB2, AHB3, AHB4, APB1L, APB1H, APB2, APB3, APB4
 *
 *  Encoding: 32-bit value = (bus_id << 24) | (1U << bit_position).
 *      bus_id      (bits 31:24)   selects the bus (see RCC_PeriphBus_t)
 *      bit_position(bits 23:0)    the bit position in the bus *_ENR register
 *
 *  Reference: RM0433 Rev 7, Table 83 (register map and reset values).
 * ========================================================================= */
typedef enum
{
    RCC_BUS_AHB1 = 1,
    RCC_BUS_AHB2 = 2,
    RCC_BUS_AHB3 = 3,
    RCC_BUS_AHB4 = 4,
    RCC_BUS_APB1L = 5,
    RCC_BUS_APB1H = 6,
    RCC_BUS_APB2 = 7,
    RCC_BUS_APB3 = 8,
    RCC_BUS_APB4 = 9
} RCC_PeriphBus_t;

#define RCC_PERIPH_ENCODE(bus, bit)  (((uint32_t)(bus) << 24) | (1U << (bit)))
#define RCC_PERIPH_BUS(p)            ((RCC_PeriphBus_t)(((uint32_t)(p) >> 24) & 0xFFU))
#define RCC_PERIPH_BIT(p)            ((uint32_t)((uint32_t)(p) & 0x00FFFFFFU))

typedef enum
{
    /* ---- AHB1 peripherals ---- */
    RCC_AHB1_DMA1          = RCC_PERIPH_ENCODE(RCC_BUS_AHB1,  0),
    RCC_AHB1_DMA2          = RCC_PERIPH_ENCODE(RCC_BUS_AHB1,  1),
    RCC_AHB1_ADC12         = RCC_PERIPH_ENCODE(RCC_BUS_AHB1,  5),
    RCC_AHB1_ETH1MAC       = RCC_PERIPH_ENCODE(RCC_BUS_AHB1, 15),
    RCC_AHB1_ETH1TX        = RCC_PERIPH_ENCODE(RCC_BUS_AHB1, 16),
    RCC_AHB1_ETH1RX        = RCC_PERIPH_ENCODE(RCC_BUS_AHB1, 17),
    RCC_AHB1_USB1OTGHS     = RCC_PERIPH_ENCODE(RCC_BUS_AHB1, 25),
    RCC_AHB1_USB1OTGHSULPI = RCC_PERIPH_ENCODE(RCC_BUS_AHB1, 26),
    RCC_AHB1_USB2OTGHS     = RCC_PERIPH_ENCODE(RCC_BUS_AHB1, 27),
    RCC_AHB1_USB2OTGHSULPI = RCC_PERIPH_ENCODE(RCC_BUS_AHB1, 28),

    /* ---- AHB2 peripherals ---- */
    RCC_AHB2_DCMI          = RCC_PERIPH_ENCODE(RCC_BUS_AHB2,  0),
    RCC_AHB2_CRYP          = RCC_PERIPH_ENCODE(RCC_BUS_AHB2,  4),
    RCC_AHB2_HASH          = RCC_PERIPH_ENCODE(RCC_BUS_AHB2,  5),
    RCC_AHB2_RNG           = RCC_PERIPH_ENCODE(RCC_BUS_AHB2,  6),
    RCC_AHB2_SDMMC2        = RCC_PERIPH_ENCODE(RCC_BUS_AHB2,  9),
    RCC_AHB2_SRAM1         = RCC_PERIPH_ENCODE(RCC_BUS_AHB2, 29),
    RCC_AHB2_SRAM2         = RCC_PERIPH_ENCODE(RCC_BUS_AHB2, 30),
    RCC_AHB2_SRAM3         = RCC_PERIPH_ENCODE(RCC_BUS_AHB2, 31),

    /* ---- AHB3 peripherals ---- */
    RCC_AHB3_MDMA          = RCC_PERIPH_ENCODE(RCC_BUS_AHB3,  0),
    RCC_AHB3_DMA2D         = RCC_PERIPH_ENCODE(RCC_BUS_AHB3,  4),
    RCC_AHB3_JPGDEC        = RCC_PERIPH_ENCODE(RCC_BUS_AHB3,  5),
    RCC_AHB3_FLASH         = RCC_PERIPH_ENCODE(RCC_BUS_AHB3,  8),
    RCC_AHB3_FMC           = RCC_PERIPH_ENCODE(RCC_BUS_AHB3, 12),
    RCC_AHB3_QSPI          = RCC_PERIPH_ENCODE(RCC_BUS_AHB3, 14),
    RCC_AHB3_SDMMC1        = RCC_PERIPH_ENCODE(RCC_BUS_AHB3, 16),

    /* ---- AHB4 peripherals ---- */
    RCC_AHB4_GPIOA         = RCC_PERIPH_ENCODE(RCC_BUS_AHB4,  0),
    RCC_AHB4_GPIOB         = RCC_PERIPH_ENCODE(RCC_BUS_AHB4,  1),
    RCC_AHB4_GPIOC         = RCC_PERIPH_ENCODE(RCC_BUS_AHB4,  2),
    RCC_AHB4_GPIOD         = RCC_PERIPH_ENCODE(RCC_BUS_AHB4,  3),
    RCC_AHB4_GPIOE         = RCC_PERIPH_ENCODE(RCC_BUS_AHB4,  4),
    RCC_AHB4_GPIOF         = RCC_PERIPH_ENCODE(RCC_BUS_AHB4,  5),
    RCC_AHB4_GPIOG         = RCC_PERIPH_ENCODE(RCC_BUS_AHB4,  6),
    RCC_AHB4_GPIOH         = RCC_PERIPH_ENCODE(RCC_BUS_AHB4,  7),
    RCC_AHB4_GPIOI         = RCC_PERIPH_ENCODE(RCC_BUS_AHB4,  8),
    RCC_AHB4_GPIOJ         = RCC_PERIPH_ENCODE(RCC_BUS_AHB4,  9),
    RCC_AHB4_GPIOK         = RCC_PERIPH_ENCODE(RCC_BUS_AHB4, 10),
    RCC_AHB4_CRC           = RCC_PERIPH_ENCODE(RCC_BUS_AHB4, 19),
    RCC_AHB4_BDMA          = RCC_PERIPH_ENCODE(RCC_BUS_AHB4, 21),
    RCC_AHB4_ADC3          = RCC_PERIPH_ENCODE(RCC_BUS_AHB4, 24),
    RCC_AHB4_HSEM          = RCC_PERIPH_ENCODE(RCC_BUS_AHB4, 25),
    RCC_AHB4_BKPRAM        = RCC_PERIPH_ENCODE(RCC_BUS_AHB4, 28),

    /* ---- APB1L peripherals ---- */
    RCC_APB1L_TIM2         = RCC_PERIPH_ENCODE(RCC_BUS_APB1L,  0),
    RCC_APB1L_TIM3         = RCC_PERIPH_ENCODE(RCC_BUS_APB1L,  1),
    RCC_APB1L_TIM4         = RCC_PERIPH_ENCODE(RCC_BUS_APB1L,  2),
    RCC_APB1L_TIM5         = RCC_PERIPH_ENCODE(RCC_BUS_APB1L,  3),
    RCC_APB1L_TIM6         = RCC_PERIPH_ENCODE(RCC_BUS_APB1L,  4),
    RCC_APB1L_TIM7         = RCC_PERIPH_ENCODE(RCC_BUS_APB1L,  5),
    RCC_APB1L_TIM12        = RCC_PERIPH_ENCODE(RCC_BUS_APB1L,  6),
    RCC_APB1L_TIM13        = RCC_PERIPH_ENCODE(RCC_BUS_APB1L,  7),
    RCC_APB1L_TIM14        = RCC_PERIPH_ENCODE(RCC_BUS_APB1L,  8),
    RCC_APB1L_LPTIM1       = RCC_PERIPH_ENCODE(RCC_BUS_APB1L,  9),
    RCC_APB1L_SPI2         = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 14),
    RCC_APB1L_SPI3         = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 15),
    RCC_APB1L_SPDIFRX      = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 16),
    RCC_APB1L_USART2       = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 17),
    RCC_APB1L_USART3       = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 18),
    RCC_APB1L_UART4        = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 19),
    RCC_APB1L_UART5        = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 20),
    RCC_APB1L_I2C1         = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 21),
    RCC_APB1L_I2C2         = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 22),
    RCC_APB1L_I2C3         = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 23),
    RCC_APB1L_CEC          = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 27),
    RCC_APB1L_DAC12        = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 29),
    RCC_APB1L_UART7        = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 30),
    RCC_APB1L_UART8        = RCC_PERIPH_ENCODE(RCC_BUS_APB1L, 31),

    /* ---- APB1H peripherals ---- */
    RCC_APB1H_CRS          = RCC_PERIPH_ENCODE(RCC_BUS_APB1H,  1),
    RCC_APB1H_SWP          = RCC_PERIPH_ENCODE(RCC_BUS_APB1H,  2),
    RCC_APB1H_OPAMP        = RCC_PERIPH_ENCODE(RCC_BUS_APB1H,  4),
    RCC_APB1H_MDIOS        = RCC_PERIPH_ENCODE(RCC_BUS_APB1H,  5),
    RCC_APB1H_FDCAN        = RCC_PERIPH_ENCODE(RCC_BUS_APB1H,  8),

    /* ---- APB2 peripherals ---- */
    RCC_APB2_TIM1          = RCC_PERIPH_ENCODE(RCC_BUS_APB2,   0),
    RCC_APB2_TIM8          = RCC_PERIPH_ENCODE(RCC_BUS_APB2,   1),
    RCC_APB2_USART1        = RCC_PERIPH_ENCODE(RCC_BUS_APB2,   4),
    RCC_APB2_USART6        = RCC_PERIPH_ENCODE(RCC_BUS_APB2,   5),
    RCC_APB2_SPI1          = RCC_PERIPH_ENCODE(RCC_BUS_APB2,  12),
    RCC_APB2_SPI4          = RCC_PERIPH_ENCODE(RCC_BUS_APB2,  13),
    RCC_APB2_TIM15         = RCC_PERIPH_ENCODE(RCC_BUS_APB2,  16),
    RCC_APB2_TIM16         = RCC_PERIPH_ENCODE(RCC_BUS_APB2,  17),
    RCC_APB2_TIM17         = RCC_PERIPH_ENCODE(RCC_BUS_APB2,  18),
    RCC_APB2_SPI5          = RCC_PERIPH_ENCODE(RCC_BUS_APB2,  20),
    RCC_APB2_SAI1          = RCC_PERIPH_ENCODE(RCC_BUS_APB2,  22),
    RCC_APB2_SAI2          = RCC_PERIPH_ENCODE(RCC_BUS_APB2,  23),
    RCC_APB2_SAI3          = RCC_PERIPH_ENCODE(RCC_BUS_APB2,  24),
    RCC_APB2_DFSDM1        = RCC_PERIPH_ENCODE(RCC_BUS_APB2,  28),
    RCC_APB2_HRTIM         = RCC_PERIPH_ENCODE(RCC_BUS_APB2,  29),

    /* ---- APB3 peripherals ---- */
    RCC_APB3_LTDC          = RCC_PERIPH_ENCODE(RCC_BUS_APB3,   3),
    RCC_APB3_WWDG1         = RCC_PERIPH_ENCODE(RCC_BUS_APB3,   6),

    /* ---- APB4 peripherals ---- */
    RCC_APB4_SYSCFG        = RCC_PERIPH_ENCODE(RCC_BUS_APB4,   1),
    RCC_APB4_LPUART1       = RCC_PERIPH_ENCODE(RCC_BUS_APB4,   3),
    RCC_APB4_SPI6          = RCC_PERIPH_ENCODE(RCC_BUS_APB4,   5),
    RCC_APB4_I2C4          = RCC_PERIPH_ENCODE(RCC_BUS_APB4,   7),
    RCC_APB4_LPTIM2        = RCC_PERIPH_ENCODE(RCC_BUS_APB4,   9),
    RCC_APB4_LPTIM3        = RCC_PERIPH_ENCODE(RCC_BUS_APB4,  10),
    RCC_APB4_LPTIM4        = RCC_PERIPH_ENCODE(RCC_BUS_APB4,  11),
    RCC_APB4_LPTIM5        = RCC_PERIPH_ENCODE(RCC_BUS_APB4,  12),
    RCC_APB4_COMP12        = RCC_PERIPH_ENCODE(RCC_BUS_APB4,  14),
    RCC_APB4_VREF          = RCC_PERIPH_ENCODE(RCC_BUS_APB4,  15),
    RCC_APB4_RTCAPB        = RCC_PERIPH_ENCODE(RCC_BUS_APB4,  16),
    RCC_APB4_SAI4          = RCC_PERIPH_ENCODE(RCC_BUS_APB4,  21)
} RCC_Periph_t;

/* High level AHB / APB helpers */
void RCC_AHB1_ClkEnable   (uint32_t mask, uint8_t enable);
void RCC_AHB1_ClkReset    (uint32_t mask);
void RCC_AHB2_ClkEnable   (uint32_t mask, uint8_t enable);
void RCC_AHB2_ClkReset    (uint32_t mask);
void RCC_AHB3_ClkEnable   (uint32_t mask, uint8_t enable);
void RCC_AHB3_ClkReset    (uint32_t mask);
void RCC_AHB4_ClkEnable   (uint32_t mask, uint8_t enable);
void RCC_AHB4_ClkReset    (uint32_t mask);
void RCC_APB1L_ClkEnable  (uint32_t mask, uint8_t enable);
void RCC_APB1L_ClkReset   (uint32_t mask);
void RCC_APB1H_ClkEnable  (uint32_t mask, uint8_t enable);
void RCC_APB1H_ClkReset   (uint32_t mask);
void RCC_APB2_ClkEnable   (uint32_t mask, uint8_t enable);
void RCC_APB2_ClkReset    (uint32_t mask);
void RCC_APB3_ClkEnable   (uint32_t mask, uint8_t enable);
void RCC_APB3_ClkReset    (uint32_t mask);
void RCC_APB4_ClkEnable   (uint32_t mask, uint8_t enable);
void RCC_APB4_ClkReset    (uint32_t mask);

/* Convenience: per-peripheral enable / disable / reset.
 * These are implemented in RCCH743ZI.c and dispatch to the correct bus.
 * (C does not allow static inline dispatch on enum ranges.)            */
void RCC_PeriphEnable (RCC_Periph_t p);
void RCC_PeriphDisable(RCC_Periph_t p);
void RCC_PeriphReset  (RCC_Periph_t p);

#endif /* RCC_H743ZI_H */
