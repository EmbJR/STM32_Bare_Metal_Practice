/**
 * @file h7nvic.h
 * @brief STM32H743 NVIC (Nested Vectored Interrupt Controller) driver header.
 * @author Kilo
 * @date 2026-07-07
 *
 * Provides bare-metal NVIC register definitions, IRQ number enums,
 * callback type definitions, and public APIs for interrupt management
 * on the Cortex-M7 core of STM32H743ZIT6.
 *
 * Reference:
 *  - PM0253: STM32F7/H7 Cortex-M7 Programming Manual
 *  - RM0433: STM32H742/H743/753/750 Reference Manual (Vector table)
 *
 * No CMSIS / HAL dependency. Direct register access only.
 */

#ifndef H7_NVIC_H
#define H7_NVIC_H

#include <stdint.h>

/* =========================================================================
 *  NVIC base address (Cortex-M7 private peripheral bus)
 * ========================================================================= */
#define NVIC_BASE               0xE000E100UL

/* =========================================================================
 *  NVIC register offsets from NVIC_BASE
 *  Each register type has 8 instances for 240 interrupts (32 bits each).
 *  STM32H743 uses 150 maskable interrupts (IRQ 0..149).
 * ========================================================================= */
#define NVIC_ISER0_OFFSET       0x000U
#define NVIC_ISER1_OFFSET       0x004U
#define NVIC_ISER2_OFFSET       0x008U
#define NVIC_ISER3_OFFSET       0x00CU
#define NVIC_ISER4_OFFSET       0x010U
#define NVIC_ISER5_OFFSET       0x014U
#define NVIC_ISER6_OFFSET       0x018U
#define NVIC_ISER7_OFFSET       0x01CU

#define NVIC_ICER0_OFFSET       0x080U
#define NVIC_ICER1_OFFSET       0x084U
#define NVIC_ICER2_OFFSET       0x088U
#define NVIC_ICER3_OFFSET       0x08CU
#define NVIC_ICER4_OFFSET       0x090U
#define NVIC_ICER5_OFFSET       0x094U
#define NVIC_ICER6_OFFSET       0x098U
#define NVIC_ICER7_OFFSET       0x09CU

#define NVIC_ISPR0_OFFSET       0x100U
#define NVIC_ISPR1_OFFSET       0x104U
#define NVIC_ISPR2_OFFSET       0x108U
#define NVIC_ISPR3_OFFSET       0x10CU
#define NVIC_ISPR4_OFFSET       0x110U
#define NVIC_ISPR5_OFFSET       0x114U
#define NVIC_ISPR6_OFFSET       0x118U
#define NVIC_ISPR7_OFFSET       0x11CU

#define NVIC_ICPR0_OFFSET       0x180U
#define NVIC_ICPR1_OFFSET       0x184U
#define NVIC_ICPR2_OFFSET       0x188U
#define NVIC_ICPR3_OFFSET       0x18CU
#define NVIC_ICPR4_OFFSET       0x190U
#define NVIC_ICPR5_OFFSET       0x194U
#define NVIC_ICPR6_OFFSET       0x198U
#define NVIC_ICPR7_OFFSET       0x19CU

#define NVIC_IABR0_OFFSET       0x200U
#define NVIC_IABR1_OFFSET       0x204U
#define NVIC_IABR2_OFFSET       0x208U
#define NVIC_IABR3_OFFSET       0x20CU
#define NVIC_IABR4_OFFSET       0x210U
#define NVIC_IABR5_OFFSET       0x214U
#define NVIC_IABR6_OFFSET       0x218U
#define NVIC_IABR7_OFFSET       0x21CU

#define NVIC_IPR_BASE_OFFSET    0x300U

#define NVIC_STIR_OFFSET        0xE00U

/* =========================================================================
 *  Direct register access macros
 * ========================================================================= */
#define NVIC_ISER(i)            (*(volatile uint32_t *)(NVIC_BASE + NVIC_ISER0_OFFSET + ((i) * 4U)))
#define NVIC_ICER(i)            (*(volatile uint32_t *)(NVIC_BASE + NVIC_ICER0_OFFSET + ((i) * 4U)))
#define NVIC_ISPR(i)            (*(volatile uint32_t *)(NVIC_BASE + NVIC_ISPR0_OFFSET + ((i) * 4U)))
#define NVIC_ICPR(i)            (*(volatile uint32_t *)(NVIC_BASE + NVIC_ICPR0_OFFSET + ((i) * 4U)))
#define NVIC_IABR(i)            (*(volatile uint32_t *)(NVIC_BASE + NVIC_IABR0_OFFSET + ((i) * 4U)))
#define NVIC_IPR(i)             (*(volatile uint8_t  *)(NVIC_BASE + NVIC_IPR_BASE_OFFSET + (i)))

#define NVIC_STIR               (*(volatile uint32_t *)(NVIC_BASE + NVIC_STIR_OFFSET))

/* =========================================================================
 *  SCB register needed for priority grouping (AIRCR)
 * ========================================================================= */
#define SCB_BASE                0xE000ED00UL
#define SCB_AIRCR_OFFSET        0x0C0U
#define SCB_AIRCR               (*(volatile uint32_t *)(SCB_BASE + SCB_AIRCR_OFFSET))

#define SCB_AIRCR_VECTKEY_Pos   16U
#define SCB_AIRCR_VECTKEY       (0x05FAU << SCB_AIRCR_VECTKEY_Pos)
#define SCB_AIRCR_PRIGROUP_Pos  8U
#define SCB_AIRCR_PRIGROUP_Msk  (7U << SCB_AIRCR_PRIGROUP_Pos)

/* =========================================================================
 *  NVIC priority configuration
 *  Cortex-M7 on STM32H743 implements 4 priority bits (NVIC_PRIO_BITS = 4).
 *  Valid priority values: 0x00 (highest), 0x10, 0x20, ... 0xF0 (lowest).
 * ========================================================================= */
#define NVIC_PRIO_BITS          4U

/* Priority grouping constants for SCB_AIRCR.PRIGROUP */
#define NVIC_PRIORITYGROUP_0    (0x00000007U)   /* 0 bits group, 4 bits sub */
#define NVIC_PRIORITYGROUP_1    (0x00000006U)   /* 1 bits group, 3 bits sub */
#define NVIC_PRIORITYGROUP_2    (0x00000005U)   /* 2 bits group, 2 bits sub */
#define NVIC_PRIORITYGROUP_3    (0x00000004U)   /* 3 bits group, 1 bits sub */
#define NVIC_PRIORITYGROUP_4    (0x00000003U)   /* 4 bits group, 0 bits sub */

/* =========================================================================
 *  Bit manipulation macros
 * ========================================================================= */
#define SET_BIT(REG, BIT)       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)      ((REG) & (BIT))

/* =========================================================================
 *  IRQ number definitions (STM32H743 vector table, RM0433 §12.3)
 *
 *  Each enum value equals the IRQ number (0..149) used in NVIC_ISER[n],
 *  where n = IRQn / 32 and the bit within that register = IRQn % 32.
 *
 *  The ISER/ICER/ISPR/ICPR register index can be computed as:
 *      reg_index = IRQn / 32
 *      bit_pos   = IRQn % 32
 * ========================================================================= */
typedef enum
{
    /* ---- System / core IRQs (0..5) ---- */
    NVIC_IRQ_WWDG               = 0,
    NVIC_IRQ_PVD_AVD            = 1,
    NVIC_IRQ_TAMP_STAMP         = 2,
    NVIC_IRQ_RTC_WKUP           = 3,
    NVIC_IRQ_FLASH              = 4,
    NVIC_IRQ_RCC                = 5,

    /* ---- EXTI lines (6..10, 23, 40) ---- */
    NVIC_IRQ_EXTI0              = 6,
    NVIC_IRQ_EXTI1              = 7,
    NVIC_IRQ_EXTI2              = 8,
    NVIC_IRQ_EXTI3              = 9,
    NVIC_IRQ_EXTI4              = 10,

    /* ---- DMA1 streams (11..17, 47) ---- */
    NVIC_IRQ_DMA1_Stream0       = 11,
    NVIC_IRQ_DMA1_Stream1       = 12,
    NVIC_IRQ_DMA1_Stream2       = 13,
    NVIC_IRQ_DMA1_Stream3       = 14,
    NVIC_IRQ_DMA1_Stream4       = 15,
    NVIC_IRQ_DMA1_Stream5       = 16,
    NVIC_IRQ_DMA1_Stream6       = 17,

    /* ---- ADC (18) ---- */
    NVIC_IRQ_ADC                = 18,

    /* ---- FDCAN1/2 interrupts (19..22) ---- */
    NVIC_IRQ_FDCAN1_IT0         = 19,
    NVIC_IRQ_FDCAN2_IT0         = 20,
    NVIC_IRQ_FDCAN1_IT1         = 21,
    NVIC_IRQ_FDCAN2_IT1         = 22,

    /* ---- EXTI 9:5 (23) ---- */
    NVIC_IRQ_EXTI9_5            = 23,

    /* ---- TIM1 interrupts (24..27) ---- */
    NVIC_IRQ_TIM1_BRK           = 24,
    NVIC_IRQ_TIM1_UP            = 25,
    NVIC_IRQ_TIM1_TRG_COM       = 26,
    NVIC_IRQ_TIM1_CC            = 27,

    /* ---- TIM2..TIM4 (28..30) ---- */
    NVIC_IRQ_TIM2               = 28,
    NVIC_IRQ_TIM3               = 29,
    NVIC_IRQ_TIM4               = 30,

    /* ---- I2C1 Event/Error (31..32) ---- */
    NVIC_IRQ_I2C1_EV            = 31,
    NVIC_IRQ_I2C1_ER            = 32,

    /* ---- I2C2 Event/Error (33..34) ---- */
    NVIC_IRQ_I2C2_EV            = 33,
    NVIC_IRQ_I2C2_ER            = 34,

    /* ---- SPI1..SPI2 (35..36) ---- */
    NVIC_IRQ_SPI1               = 35,
    NVIC_IRQ_SPI2               = 36,

    /* ---- USART1..USART3 (37..39) ---- */
    NVIC_IRQ_USART1             = 37,
    NVIC_IRQ_USART2             = 38,
    NVIC_IRQ_USART3             = 39,

    /* ---- EXTI 15:10 (40) ---- */
    NVIC_IRQ_EXTI15_10          = 40,

    /* ---- RTC Alarm (41) ---- */
    NVIC_IRQ_RTC_Alarm          = 41,

    /* ---- Reserved 42 ---- */

    /* ---- TIM8 + TIM12..TIM14 (43..46) ---- */
    NVIC_IRQ_TIM8_BRK_TIM12     = 43,
    NVIC_IRQ_TIM8_UP_TIM13      = 44,
    NVIC_IRQ_TIM8_TRG_COM_TIM14 = 45,
    NVIC_IRQ_TIM8_CC            = 46,

    /* ---- DMA1 Stream7 (47) ---- */
    NVIC_IRQ_DMA1_Stream7       = 47,

    /* ---- FMC (48) ---- */
    NVIC_IRQ_FMC                = 48,

    /* ---- SDMMC1 (49) ---- */
    NVIC_IRQ_SDMMC1             = 49,

    /* ---- TIM5 (50) ---- */
    NVIC_IRQ_TIM5               = 50,

    /* ---- SPI3 (51) ---- */
    NVIC_IRQ_SPI3               = 51,

    /* ---- UART4..UART5 (52..53) ---- */
    NVIC_IRQ_UART4              = 52,
    NVIC_IRQ_UART5              = 53,

    /* ---- TIM6 + DAC (54) ---- */
    NVIC_IRQ_TIM6_DAC           = 54,

    /* ---- TIM7 (55) ---- */
    NVIC_IRQ_TIM7               = 55,

    /* ---- DMA2 streams (56..70) ---- */
    NVIC_IRQ_DMA2_Stream0       = 56,
    NVIC_IRQ_DMA2_Stream1       = 57,
    NVIC_IRQ_DMA2_Stream2       = 58,
    NVIC_IRQ_DMA2_Stream3       = 59,
    NVIC_IRQ_DMA2_Stream4       = 60,

    /* ---- ETH (61..62) ---- */
    NVIC_IRQ_ETH                = 61,
    NVIC_IRQ_ETH_WKUP           = 62,

    /* ---- FDCAN Calibration (63) ---- */
    NVIC_IRQ_FDCAN_CAL          = 63,

    /* ---- Reserved 64..67 ---- */

    NVIC_IRQ_DMA2_Stream5       = 68,
    NVIC_IRQ_DMA2_Stream6       = 69,
    NVIC_IRQ_DMA2_Stream7       = 70,

    /* ---- USART6 (71) ---- */
    NVIC_IRQ_USART6             = 71,

    /* ---- I2C3 Event/Error (72..73) ---- */
    NVIC_IRQ_I2C3_EV            = 72,
    NVIC_IRQ_I2C3_ER            = 73,

    /* ---- USB OTG HS (74..77) ---- */
    NVIC_IRQ_OTG_HS_EP1_OUT     = 74,
    NVIC_IRQ_OTG_HS_EP1_IN      = 75,
    NVIC_IRQ_OTG_HS_WKUP        = 76,
    NVIC_IRQ_OTG_HS             = 77,

    /* ---- DCMI (78) ---- */
    NVIC_IRQ_DCMI               = 78,

    /* ---- Reserved 79 ---- */

    /* ---- RNG (80) ---- */
    NVIC_IRQ_RNG                = 80,

    /* ---- FPU (81) ---- */
    NVIC_IRQ_FPU                = 81,

    /* ---- UART7..UART8 (82..83) ---- */
    NVIC_IRQ_UART7              = 82,
    NVIC_IRQ_UART8              = 83,

    /* ---- SPI4..SPI6 (84..86) ---- */
    NVIC_IRQ_SPI4               = 84,
    NVIC_IRQ_SPI5               = 85,
    NVIC_IRQ_SPI6               = 86,

    /* ---- SAI1 (87) ---- */
    NVIC_IRQ_SAI1               = 87,

    /* ---- LTDC (88..89) ---- */
    NVIC_IRQ_LTDC               = 88,
    NVIC_IRQ_LTDC_ER            = 89,

    /* ---- DMA2D (90) ---- */
    NVIC_IRQ_DMA2D              = 90,

    /* ---- SAI2 (91) ---- */
    NVIC_IRQ_SAI2               = 91,

    /* ---- QUADSPI (92) ---- */
    NVIC_IRQ_QUADSPI            = 92,

    /* ---- LPTIM1 (93) ---- */
    NVIC_IRQ_LPTIM1             = 93,

    /* ---- CEC (94) ---- */
    NVIC_IRQ_CEC                = 94,

    /* ---- I2C4 Event/Error (95..96) ---- */
    NVIC_IRQ_I2C4_EV            = 95,
    NVIC_IRQ_I2C4_ER            = 96,

    /* ---- SPDIF_RX (97) ---- */
    NVIC_IRQ_SPDIF_RX           = 97,

    /* ---- USB OTG FS (98..101) ---- */
    NVIC_IRQ_OTG_FS_EP1_OUT     = 98,
    NVIC_IRQ_OTG_FS_EP1_IN      = 99,
    NVIC_IRQ_OTG_FS_WKUP        = 100,
    NVIC_IRQ_OTG_FS             = 101,

    /* ---- DMAMUX1 Overrun (102) ---- */
    NVIC_IRQ_DMAMUX1_OVR        = 102,

    /* ---- HRTIM1 (103..109) ---- */
    NVIC_IRQ_HRTIM1_Master      = 103,
    NVIC_IRQ_HRTIM1_TIMA        = 104,
    NVIC_IRQ_HRTIM1_TIMB        = 105,
    NVIC_IRQ_HRTIM1_TIMC        = 106,
    NVIC_IRQ_HRTIM1_TIMD        = 107,
    NVIC_IRQ_HRTIM1_TIME        = 108,
    NVIC_IRQ_HRTIM1_FLT         = 109,

    /* ---- DFSDM1 Filters (110..113) ---- */
    NVIC_IRQ_DFSDM1_FLT0        = 110,
    NVIC_IRQ_DFSDM1_FLT1        = 111,
    NVIC_IRQ_DFSDM1_FLT2        = 112,
    NVIC_IRQ_DFSDM1_FLT3        = 113,

    /* ---- SAI3 (114) ---- */
    NVIC_IRQ_SAI3               = 114,

    /* ---- SWPMI1 (115) ---- */
    NVIC_IRQ_SWPMI1             = 115,

    /* ---- TIM15..TIM17 (116..118) ---- */
    NVIC_IRQ_TIM15              = 116,
    NVIC_IRQ_TIM16              = 117,
    NVIC_IRQ_TIM17              = 118,

    /* ---- MDIOS (119..120) ---- */
    NVIC_IRQ_MDIOS_WKUP         = 119,
    NVIC_IRQ_MDIOS              = 120,

    /* ---- JPEG (121) ---- */
    NVIC_IRQ_JPEG               = 121,

    /* ---- MDMA (122) ---- */
    NVIC_IRQ_MDMA               = 122,

    /* ---- Reserved 123 ---- */

    /* ---- SDMMC2 (124) ---- */
    NVIC_IRQ_SDMMC2             = 124,

    /* ---- HSEM1 (125) ---- */
    NVIC_IRQ_HSEM1              = 125,

    /* ---- Reserved 126 ---- */

    /* ---- ADC3 (127) ---- */
    NVIC_IRQ_ADC3               = 127,

    /* ---- DMAMUX2 Overrun (128) ---- */
    NVIC_IRQ_DMAMUX2_OVR        = 128,

    /* ---- BDMA Channels (129..136) ---- */
    NVIC_IRQ_BDMA_Channel0      = 129,
    NVIC_IRQ_BDMA_Channel1      = 130,
    NVIC_IRQ_BDMA_Channel2      = 131,
    NVIC_IRQ_BDMA_Channel3      = 132,
    NVIC_IRQ_BDMA_Channel4      = 133,
    NVIC_IRQ_BDMA_Channel5      = 134,
    NVIC_IRQ_BDMA_Channel6      = 135,
    NVIC_IRQ_BDMA_Channel7      = 136,

    /* ---- COMP (137) ---- */
    NVIC_IRQ_COMP               = 137,

    /* ---- LPTIM2..LPTIM5 (138..141) ---- */
    NVIC_IRQ_LPTIM2             = 138,
    NVIC_IRQ_LPTIM3             = 139,
    NVIC_IRQ_LPTIM4             = 140,
    NVIC_IRQ_LPTIM5             = 141,

    /* ---- LPUART1 (142) ---- */
    NVIC_IRQ_LPUART1            = 142,

    /* ---- Reserved 143 ---- */

    /* ---- CRS (144) ---- */
    NVIC_IRQ_CRS                = 144,

    /* ---- ECC (145) ---- */
    NVIC_IRQ_ECC                = 145,

    /* ---- SAI4 (146) ---- */
    NVIC_IRQ_SAI4               = 146,

    /* ---- Reserved 147..148 ---- */

    /* ---- WAKEUP_PIN (149) ---- */
    NVIC_IRQ_WAKEUP_PIN         = 149
} NVIC_IRQn_t;

/* =========================================================================
 *  Interrupt callback function type
 *
 *  Each IRQ can have one user-registered callback. The function takes
 *  no arguments and returns void. It is called from the weak default
 *  ISR handler after any necessary flag clearing.
 * ========================================================================= */
typedef void (*NVIC_Callback_t)(void);

/* =========================================================================
 *  Public API function prototypes
 * ========================================================================= */

/* ---- Init / de-init ------------------------------------------------ */
void     NVIC_Init           (void);
void     NVIC_DeInit         (void);

/* ---- Interrupt enable / disable ------------------------------------ */
void     NVIC_EnableIRQ      (NVIC_IRQn_t irq);
void     NVIC_DisableIRQ     (NVIC_IRQn_t irq);
uint32_t NVIC_GetEnableState (NVIC_IRQn_t irq);

/* ---- Pending interrupt --------------------------------------------- */
void     NVIC_SetPendingIRQ  (NVIC_IRQn_t irq);
void     NVIC_ClearPendingIRQ(NVIC_IRQn_t irq);
uint32_t NVIC_GetPendingIRQ  (NVIC_IRQn_t irq);

/* ---- Active interrupt (read-only) ---------------------------------- */
uint32_t NVIC_GetActiveIRQ   (NVIC_IRQn_t irq);

/* ---- Priority management ------------------------------------------- */
void     NVIC_SetPriority    (NVIC_IRQn_t irq, uint32_t priority);
uint32_t NVIC_GetPriority    (NVIC_IRQn_t irq);

/* ---- Priority grouping (SCB_AIRCR.PRIGROUP) ------------------------ */
void     NVIC_SetPriorityGrouping(uint32_t priorityGroup);

/* ---- Software trigger ---------------------------------------------- */
void     NVIC_SoftwareTrigger(NVIC_IRQn_t irq);

/* ---- User callback registration ------------------------------------ */
void     NVIC_RegisterCallback(NVIC_IRQn_t irq, NVIC_Callback_t callback);
void     NVIC_UnregisterCallback(NVIC_IRQn_t irq);

/* ---- Sleep mode ---------------------------------------------------- */
void     NVIC_EnterSleep     (void);
void     NVIC_EnterSleepOnExit(uint8_t enable);

#endif /* H7_NVIC_H */
