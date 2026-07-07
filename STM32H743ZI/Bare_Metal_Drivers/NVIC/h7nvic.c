/**
 * @file h7nvic.c
 * @brief STM32H743 NVIC driver implementation.
 * @author Kilo
 * @date 2026-07-07
 *
 * Implements interrupt enable/disable, priority management,
 * user callback registration, and weak default ISR handlers
 * for all 150 maskable interrupts on STM32H743.
 *
 * No CMSIS / HAL dependency. Direct register access only.
 */

#include "h7nvic.h"

/* =========================================================================
 *  Callback array: one function pointer per IRQ (indices 0..149).
 *  Initialized to NULL (no callback installed).
 * ========================================================================= */
static NVIC_Callback_t nvic_callbacks[150];

/* =========================================================================
 *  ARM architecture helpers
 * ========================================================================= */
#ifndef __DSB
#define __DSB()  __asm__ volatile("dsb 0xF" ::: "memory")
#endif
#ifndef __ISB
#define __ISB()  __asm__ volatile("isb 0xF" ::: "memory")
#endif

/* =========================================================================
 *  Static helpers
 * ========================================================================= */
static uint32_t nvic_get_iser_index(NVIC_IRQn_t irq)
{
    return (uint32_t)irq / 32U;
}

static uint32_t nvic_get_bit_pos(NVIC_IRQn_t irq)
{
    return (uint32_t)irq % 32U;
}

/* =========================================================================
 *  Init / de-init
 * ========================================================================= */

/**
 * @brief Initialize the NVIC driver.
 *
 * Clears all user-registered callbacks and resets priority grouping
 * to default (no split, all bits are group priority).
 */
void NVIC_Init(void)
{
    for (uint32_t i = 0U; i < 150U; i++)
    {
        nvic_callbacks[i] = (NVIC_Callback_t)0;
    }

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

/**
 * @brief De-initialize the NVIC driver.
 *
 * Disables all maskable interrupts and clears all user callbacks.
 */
void NVIC_DeInit(void)
{
    for (uint32_t i = 0U; i < 8U; i++)
    {
        NVIC_ICER(i) = 0xFFFFFFFFU;
        NVIC_ICPR(i) = 0xFFFFFFFFU;
    }

    for (uint32_t i = 0U; i < 150U; i++)
    {
        nvic_callbacks[i] = (NVIC_Callback_t)0;
    }

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

/* =========================================================================
 *  Interrupt enable / disable
 *
 *  ISERn: write 1 to enable; write 0 has no effect.
 *  ICERn: write 1 to disable; write 0 has no effect.
 *  Reading either returns 1 if enabled.
 * ========================================================================= */

void NVIC_EnableIRQ(NVIC_IRQn_t irq)
{
    uint32_t idx  = nvic_get_iser_index(irq);
    uint32_t bit  = nvic_get_bit_pos(irq);
    NVIC_ISER(idx) = (1U << bit);
}

void NVIC_DisableIRQ(NVIC_IRQn_t irq)
{
    uint32_t idx  = nvic_get_iser_index(irq);
    uint32_t bit  = nvic_get_bit_pos(irq);
    NVIC_ICER(idx) = (1U << bit);
}

uint32_t NVIC_GetEnableState(NVIC_IRQn_t irq)
{
    uint32_t idx  = nvic_get_iser_index(irq);
    uint32_t bit  = nvic_get_bit_pos(irq);
    return (NVIC_ISER(idx) & (1U << bit)) ? 1U : 0U;
}

/* =========================================================================
 *  Pending interrupt control
 *
 *  ISPRn: write 1 to set pending; write 0 has no effect.
 *  ICPRn: write 1 to clear pending; write 0 has no effect.
 * ========================================================================= */

void NVIC_SetPendingIRQ(NVIC_IRQn_t irq)
{
    uint32_t idx  = nvic_get_iser_index(irq);
    uint32_t bit  = nvic_get_bit_pos(irq);
    NVIC_ISPR(idx) = (1U << bit);
}

void NVIC_ClearPendingIRQ(NVIC_IRQn_t irq)
{
    uint32_t idx  = nvic_get_iser_index(irq);
    uint32_t bit  = nvic_get_bit_pos(irq);
    NVIC_ICPR(idx) = (1U << bit);
}

uint32_t NVIC_GetPendingIRQ(NVIC_IRQn_t irq)
{
    uint32_t idx  = nvic_get_iser_index(irq);
    uint32_t bit  = nvic_get_bit_pos(irq);
    return (NVIC_ISPR(idx) & (1U << bit)) ? 1U : 0U;
}

/* =========================================================================
 *  Active interrupt (read-only)
 *
 *  IABRn returns 1 if the interrupt is currently active (handler executing
 *  or preempted). Useful for debugging or checking in ISRs.
 * ========================================================================= */

uint32_t NVIC_GetActiveIRQ(NVIC_IRQn_t irq)
{
    uint32_t idx  = nvic_get_iser_index(irq);
    uint32_t bit  = nvic_get_bit_pos(irq);
    return (NVIC_IABR(idx) & (1U << bit)) ? 1U : 0U;
}

/* =========================================================================
 *  Priority management
 *
 *  Cortex-M7 on STM32H743 implements 4 priority bits (bits [7:4] of each
 *  IPR byte). Valid values: 0x00, 0x10, 0x20, ... 0xF0.
 *  Lower value = higher priority.
 * ========================================================================= */

void NVIC_SetPriority(NVIC_IRQn_t irq, uint32_t priority)
{
    uint32_t shift = (uint32_t)irq;
    priority &= 0xF0U;
    NVIC_IPR(shift) = (uint8_t)priority;
}

uint32_t NVIC_GetPriority(NVIC_IRQn_t irq)
{
    uint32_t shift = (uint32_t)irq;
    return (uint32_t)(NVIC_IPR(shift) & 0xF0U);
}

/* =========================================================================
 *  Priority grouping
 *
 *  PRIGROUP in SCB_AIRCR controls how the 4 priority bits are split:
 *    PRIGROUP 0: 7.0 bits group, none sub
 *    PRIGROUP 1: 6.1 bits group, 3 bits sub
 *    PRIGROUP 2: 5.2 bits group, 2 bits sub
 *    PRIGROUP 3: 4.3 bits group, 1 bits sub
 *    PRIGROUP 4: 3.4 bits group, 0 bits sub
 *    PRIGROUP 5: 2.5 bits group, 1 bits sub
 *    PRIGROUP 6: 1.6 bits group, 2 bits sub
 *    PRIGROUP 7: 0.7 bits group, 4 bits sub
 *
 *  Use the NVIC_PRIORITYGROUP_x constants defined in h7nvic.h.
 *  Writing to AIRCR requires the VECTKEY in the upper halfword.
 * ========================================================================= */

void NVIC_SetPriorityGrouping(uint32_t priorityGroup)
{
    uint32_t reg  = SCB_AIRCR;
    uint32_t prio = (uint32_t)priorityGroup;
    reg &= ~(SCB_AIRCR_VECTKEY | SCB_AIRCR_PRIGROUP_Msk);
    reg |=  SCB_AIRCR_VECTKEY | ((prio & 0x7U) << SCB_AIRCR_PRIGROUP_Pos);
    SCB_AIRCR = reg;

    __DSB();
    __ISB();
}

/* =========================================================================
 *  Software trigger
 *
 *  Writing the IRQ number to STIR (Software Trigger Interrupt Register)
 *  sets the pending bit for that interrupt. The interrupt must be enabled
 *  in ISER for the handler to execute.
 * ========================================================================= */

void NVIC_SoftwareTrigger(NVIC_IRQn_t irq)
{
    NVIC_STIR = (uint32_t)irq;
}

/* =========================================================================
 *  User callback registration
 *
 *  Each IRQ can have one user-registered callback function. The callback
 *  is invoked from the weak default ISR handler below.
 * ========================================================================= */

void NVIC_RegisterCallback(NVIC_IRQn_t irq, NVIC_Callback_t callback)
{
    nvic_callbacks[(uint32_t)irq] = callback;
}

void NVIC_UnregisterCallback(NVIC_IRQn_t irq)
{
    nvic_callbacks[(uint32_t)irq] = (NVIC_Callback_t)0;
}

/* =========================================================================
 *  Sleep mode
 *
 *  WFI (Wait For Interrupt) enters sleep/standby depending on SLEEPDEEP
 *  bit in SCB->SCR.
 *  SLEEPONEXIT causes the CPU to re-enter sleep upon exiting an ISR,
 *  useful for interrupt-driven applications.
 * ========================================================================= */

void NVIC_EnterSleep(void)
{
    __DSB();
    __ISB();
    __asm__ volatile("wfi");
}

void NVIC_EnterSleepOnExit(uint8_t enable)
{
    volatile uint32_t *scr = (volatile uint32_t *)(SCB_BASE + 0x10U);
    if (enable)
    {
        SET_BIT(*scr, 0x02U);
    }
    else
    {
        CLEAR_BIT(*scr, 0x02U);
    }
}

/* =========================================================================
 *
 *  WEAK DEFAULT INTERRUPT HANDLERS
 *
 *  Each ISR is declared __attribute__((weak)) so the user can override
 *  them by simply defining a function with the same name in their
 *  application code.
 *
 *  The default handler calls the user-registered callback (if any).
 *  For EXTI lines, the handler also clears the EXTI pending flag.
 *  For other peripherals, flag clearing is the responsibility of the
 *  peripheral-specific ISR or the user callback.
 *
 *  All 150 handlers are provided below, grouped by IRQ number.
 *
 * ========================================================================= */

/* ========================= IRQ  0 ====================================== */
__attribute__((weak)) void WWDG_IRQHandler(void)
{
    if (nvic_callbacks[0]) { nvic_callbacks[0](); }
}

/* ========================= IRQ  1 ====================================== */
__attribute__((weak)) void PVD_AVD_IRQHandler(void)
{
    if (nvic_callbacks[1]) { nvic_callbacks[1](); }
}

/* ========================= IRQ  2 ====================================== */
__attribute__((weak)) void TAMP_STAMP_IRQHandler(void)
{
    if (nvic_callbacks[2]) { nvic_callbacks[2](); }
}

/* ========================= IRQ  3 ====================================== */
__attribute__((weak)) void RTC_WKUP_IRQHandler(void)
{
    if (nvic_callbacks[3]) { nvic_callbacks[3](); }
}

/* ========================= IRQ  4 ====================================== */
__attribute__((weak)) void FLASH_IRQHandler(void)
{
    if (nvic_callbacks[4]) { nvic_callbacks[4](); }
}

/* ========================= IRQ  5 ====================================== */
__attribute__((weak)) void RCC_IRQHandler(void)
{
    if (nvic_callbacks[5]) { nvic_callbacks[5](); }
}

/* ========================= IRQ  6 - EXTI0 ============================== */
__attribute__((weak)) void EXTI0_IRQHandler(void)
{
    /* Clear EXTI line 0 pending flag (write 1 to clear) */
    (*(volatile uint32_t *)(0x58000000UL + 0x88U)) = (1U << 0U);
    if (nvic_callbacks[6]) { nvic_callbacks[6](); }
}

/* ========================= IRQ  7 - EXTI1 ============================== */
__attribute__((weak)) void EXTI1_IRQHandler(void)
{
    (*(volatile uint32_t *)(0x58000000UL + 0x88U)) = (1U << 1U);
    if (nvic_callbacks[7]) { nvic_callbacks[7](); }
}

/* ========================= IRQ  8 - EXTI2 ============================== */
__attribute__((weak)) void EXTI2_IRQHandler(void)
{
    (*(volatile uint32_t *)(0x58000000UL + 0x88U)) = (1U << 2U);
    if (nvic_callbacks[8]) { nvic_callbacks[8](); }
}

/* ========================= IRQ  9 - EXTI3 ============================== */
__attribute__((weak)) void EXTI3_IRQHandler(void)
{
    (*(volatile uint32_t *)(0x58000000UL + 0x88U)) = (1U << 3U);
    if (nvic_callbacks[9]) { nvic_callbacks[9](); }
}

/* ========================= IRQ 10 - EXTI4 ============================== */
__attribute__((weak)) void EXTI4_IRQHandler(void)
{
    (*(volatile uint32_t *)(0x58000000UL + 0x88U)) = (1U << 4U);
    if (nvic_callbacks[10]) { nvic_callbacks[10](); }
}

/* ========================= IRQ 11 - DMA1_Stream0 ====================== */
__attribute__((weak)) void DMA1_Stream0_IRQHandler(void)
{
    if (nvic_callbacks[11]) { nvic_callbacks[11](); }
}

/* ========================= IRQ 12 - DMA1_Stream1 ====================== */
__attribute__((weak)) void DMA1_Stream1_IRQHandler(void)
{
    if (nvic_callbacks[12]) { nvic_callbacks[12](); }
}

/* ========================= IRQ 13 - DMA1_Stream2 ====================== */
__attribute__((weak)) void DMA1_Stream2_IRQHandler(void)
{
    if (nvic_callbacks[13]) { nvic_callbacks[13](); }
}

/* ========================= IRQ 14 - DMA1_Stream3 ====================== */
__attribute__((weak)) void DMA1_Stream3_IRQHandler(void)
{
    if (nvic_callbacks[14]) { nvic_callbacks[14](); }
}

/* ========================= IRQ 15 - DMA1_Stream4 ====================== */
__attribute__((weak)) void DMA1_Stream4_IRQHandler(void)
{
    if (nvic_callbacks[15]) { nvic_callbacks[15](); }
}

/* ========================= IRQ 16 - DMA1_Stream5 ====================== */
__attribute__((weak)) void DMA1_Stream5_IRQHandler(void)
{
    if (nvic_callbacks[16]) { nvic_callbacks[16](); }
}

/* ========================= IRQ 17 - DMA1_Stream6 ====================== */
__attribute__((weak)) void DMA1_Stream6_IRQHandler(void)
{
    if (nvic_callbacks[17]) { nvic_callbacks[17](); }
}

/* ========================= IRQ 18 - ADC =============================== */
__attribute__((weak)) void ADC_IRQHandler(void)
{
    if (nvic_callbacks[18]) { nvic_callbacks[18](); }
}

/* ========================= IRQ 19 - FDCAN1_IT0 ======================== */
__attribute__((weak)) void FDCAN1_IT0_IRQHandler(void)
{
    if (nvic_callbacks[19]) { nvic_callbacks[19](); }
}

/* ========================= IRQ 20 - FDCAN2_IT0 ======================== */
__attribute__((weak)) void FDCAN2_IT0_IRQHandler(void)
{
    if (nvic_callbacks[20]) { nvic_callbacks[20](); }
}

/* ========================= IRQ 21 - FDCAN1_IT1 ======================== */
__attribute__((weak)) void FDCAN1_IT1_IRQHandler(void)
{
    if (nvic_callbacks[21]) { nvic_callbacks[21](); }
}

/* ========================= IRQ 22 - FDCAN2_IT1 ======================== */
__attribute__((weak)) void FDCAN2_IT1_IRQHandler(void)
{
    if (nvic_callbacks[22]) { nvic_callbacks[22](); }
}

/* ========================= IRQ 23 - EXTI9_5 =========================== */
__attribute__((weak)) void EXTI9_5_IRQHandler(void)
{
    uint32_t pr = (*(volatile uint32_t *)(0x58000000UL + 0x88U));
    uint32_t clear_mask = 0U;
    for (uint32_t i = 5U; i <= 9U; i++)
    {
        if (pr & (1U << i)) { clear_mask |= (1U << i); }
    }
    (*(volatile uint32_t *)(0x58000000UL + 0x88U)) = clear_mask;
    if (nvic_callbacks[23]) { nvic_callbacks[23](); }
}

/* ========================= IRQ 24 - TIM1_BRK ========================== */
__attribute__((weak)) void TIM1_BRK_IRQHandler(void)
{
    if (nvic_callbacks[24]) { nvic_callbacks[24](); }
}

/* ========================= IRQ 25 - TIM1_UP =========================== */
__attribute__((weak)) void TIM1_UP_IRQHandler(void)
{
    if (nvic_callbacks[25]) { nvic_callbacks[25](); }
}

/* ========================= IRQ 26 - TIM1_TRG_COM ====================== */
__attribute__((weak)) void TIM1_TRG_COM_IRQHandler(void)
{
    if (nvic_callbacks[26]) { nvic_callbacks[26](); }
}

/* ========================= IRQ 27 - TIM1_CC =========================== */
__attribute__((weak)) void TIM1_CC_IRQHandler(void)
{
    if (nvic_callbacks[27]) { nvic_callbacks[27](); }
}

/* ========================= IRQ 28 - TIM2 ============================== */
__attribute__((weak)) void TIM2_IRQHandler(void)
{
    if (nvic_callbacks[28]) { nvic_callbacks[28](); }
}

/* ========================= IRQ 29 - TIM3 ============================== */
__attribute__((weak)) void TIM3_IRQHandler(void)
{
    if (nvic_callbacks[29]) { nvic_callbacks[29](); }
}

/* ========================= IRQ 30 - TIM4 ============================== */
__attribute__((weak)) void TIM4_IRQHandler(void)
{
    if (nvic_callbacks[30]) { nvic_callbacks[30](); }
}

/* ========================= IRQ 31 - I2C1_EV =========================== */
__attribute__((weak)) void I2C1_EV_IRQHandler(void)
{
    if (nvic_callbacks[31]) { nvic_callbacks[31](); }
}

/* ========================= IRQ 32 - I2C1_ER =========================== */
__attribute__((weak)) void I2C1_ER_IRQHandler(void)
{
    if (nvic_callbacks[32]) { nvic_callbacks[32](); }
}

/* ========================= IRQ 33 - I2C2_EV =========================== */
__attribute__((weak)) void I2C2_EV_IRQHandler(void)
{
    if (nvic_callbacks[33]) { nvic_callbacks[33](); }
}

/* ========================= IRQ 34 - I2C2_ER =========================== */
__attribute__((weak)) void I2C2_ER_IRQHandler(void)
{
    if (nvic_callbacks[34]) { nvic_callbacks[34](); }
}

/* ========================= IRQ 35 - SPI1 ============================== */
__attribute__((weak)) void SPI1_IRQHandler(void)
{
    if (nvic_callbacks[35]) { nvic_callbacks[35](); }
}

/* ========================= IRQ 36 - SPI2 ============================== */
__attribute__((weak)) void SPI2_IRQHandler(void)
{
    if (nvic_callbacks[36]) { nvic_callbacks[36](); }
}

/* ========================= IRQ 37 - USART1 ============================ */
__attribute__((weak)) void USART1_IRQHandler(void)
{
    if (nvic_callbacks[37]) { nvic_callbacks[37](); }
}

/* ========================= IRQ 38 - USART2 ============================ */
__attribute__((weak)) void USART2_IRQHandler(void)
{
    if (nvic_callbacks[38]) { nvic_callbacks[38](); }
}

/* ========================= IRQ 39 - USART3 ============================ */
__attribute__((weak)) void USART3_IRQHandler(void)
{
    if (nvic_callbacks[39]) { nvic_callbacks[39](); }
}

/* ========================= IRQ 40 - EXTI15_10 ========================= */
__attribute__((weak)) void EXTI15_10_IRQHandler(void)
{
    uint32_t pr = (*(volatile uint32_t *)(0x58000000UL + 0x88U));
    uint32_t clear_mask = 0U;
    for (uint32_t i = 10U; i <= 15U; i++)
    {
        if (pr & (1U << i)) { clear_mask |= (1U << i); }
    }
    (*(volatile uint32_t *)(0x58000000UL + 0x88U)) = clear_mask;
    if (nvic_callbacks[40]) { nvic_callbacks[40](); }
}

/* ========================= IRQ 41 - RTC_Alarm ======================== */
__attribute__((weak)) void RTC_Alarm_IRQHandler(void)
{
    if (nvic_callbacks[41]) { nvic_callbacks[41](); }
}

/* ========================= IRQ 42 - Reserved ========================= */
__attribute__((weak)) void RESERVED42_IRQHandler(void)
{
    if (nvic_callbacks[42]) { nvic_callbacks[42](); }
}

/* ========================= IRQ 43 - TIM8_BRK_TIM12 ==================== */
__attribute__((weak)) void TIM8_BRK_TIM12_IRQHandler(void)
{
    if (nvic_callbacks[43]) { nvic_callbacks[43](); }
}

/* ========================= IRQ 44 - TIM8_UP_TIM13 ===================== */
__attribute__((weak)) void TIM8_UP_TIM13_IRQHandler(void)
{
    if (nvic_callbacks[44]) { nvic_callbacks[44](); }
}

/* ========================= IRQ 45 - TIM8_TRG_COM_TIM14 ================ */
__attribute__((weak)) void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
    if (nvic_callbacks[45]) { nvic_callbacks[45](); }
}

/* ========================= IRQ 46 - TIM8_CC =========================== */
__attribute__((weak)) void TIM8_CC_IRQHandler(void)
{
    if (nvic_callbacks[46]) { nvic_callbacks[46](); }
}

/* ========================= IRQ 47 - DMA1_Stream7 ====================== */
__attribute__((weak)) void DMA1_Stream7_IRQHandler(void)
{
    if (nvic_callbacks[47]) { nvic_callbacks[47](); }
}

/* ========================= IRQ 48 - FMC =============================== */
__attribute__((weak)) void FMC_IRQHandler(void)
{
    if (nvic_callbacks[48]) { nvic_callbacks[48](); }
}

/* ========================= IRQ 49 - SDMMC1 ============================ */
__attribute__((weak)) void SDMMC1_IRQHandler(void)
{
    if (nvic_callbacks[49]) { nvic_callbacks[49](); }
}

/* ========================= IRQ 50 - TIM5 ============================== */
__attribute__((weak)) void TIM5_IRQHandler(void)
{
    if (nvic_callbacks[50]) { nvic_callbacks[50](); }
}

/* ========================= IRQ 51 - SPI3 ============================== */
__attribute__((weak)) void SPI3_IRQHandler(void)
{
    if (nvic_callbacks[51]) { nvic_callbacks[51](); }
}

/* ========================= IRQ 52 - UART4 ============================= */
__attribute__((weak)) void UART4_IRQHandler(void)
{
    if (nvic_callbacks[52]) { nvic_callbacks[52](); }
}

/* ========================= IRQ 53 - UART5 ============================= */
__attribute__((weak)) void UART5_IRQHandler(void)
{
    if (nvic_callbacks[53]) { nvic_callbacks[53](); }
}

/* ========================= IRQ 54 - TIM6_DAC ========================== */
__attribute__((weak)) void TIM6_DAC_IRQHandler(void)
{
    if (nvic_callbacks[54]) { nvic_callbacks[54](); }
}

/* ========================= IRQ 55 - TIM7 ============================== */
__attribute__((weak)) void TIM7_IRQHandler(void)
{
    if (nvic_callbacks[55]) { nvic_callbacks[55](); }
}

/* ========================= IRQ 56 - DMA2_Stream0 ====================== */
__attribute__((weak)) void DMA2_Stream0_IRQHandler(void)
{
    if (nvic_callbacks[56]) { nvic_callbacks[56](); }
}

/* ========================= IRQ 57 - DMA2_Stream1 ====================== */
__attribute__((weak)) void DMA2_Stream1_IRQHandler(void)
{
    if (nvic_callbacks[57]) { nvic_callbacks[57](); }
}

/* ========================= IRQ 58 - DMA2_Stream2 ====================== */
__attribute__((weak)) void DMA2_Stream2_IRQHandler(void)
{
    if (nvic_callbacks[58]) { nvic_callbacks[58](); }
}

/* ========================= IRQ 59 - DMA2_Stream3 ====================== */
__attribute__((weak)) void DMA2_Stream3_IRQHandler(void)
{
    if (nvic_callbacks[59]) { nvic_callbacks[59](); }
}

/* ========================= IRQ 60 - DMA2_Stream4 ====================== */
__attribute__((weak)) void DMA2_Stream4_IRQHandler(void)
{
    if (nvic_callbacks[60]) { nvic_callbacks[60](); }
}

/* ========================= IRQ 61 - ETH =============================== */
__attribute__((weak)) void ETH_IRQHandler(void)
{
    if (nvic_callbacks[61]) { nvic_callbacks[61](); }
}

/* ========================= IRQ 62 - ETH_WKUP ========================== */
__attribute__((weak)) void ETH_WKUP_IRQHandler(void)
{
    if (nvic_callbacks[62]) { nvic_callbacks[62](); }
}

/* ========================= IRQ 63 - FDCAN_CAL ========================= */
__attribute__((weak)) void FDCAN_CAL_IRQHandler(void)
{
    if (nvic_callbacks[63]) { nvic_callbacks[63](); }
}

/* ========================= IRQ 64..67 Reserved ======================= */
__attribute__((weak)) void RESERVED64_IRQHandler(void)
{
    if (nvic_callbacks[64]) { nvic_callbacks[64](); }
}
__attribute__((weak)) void RESERVED65_IRQHandler(void)
{
    if (nvic_callbacks[65]) { nvic_callbacks[65](); }
}
__attribute__((weak)) void RESERVED66_IRQHandler(void)
{
    if (nvic_callbacks[66]) { nvic_callbacks[66](); }
}
__attribute__((weak)) void RESERVED67_IRQHandler(void)
{
    if (nvic_callbacks[67]) { nvic_callbacks[67](); }
}

/* ========================= IRQ 68 - DMA2_Stream5 ====================== */
__attribute__((weak)) void DMA2_Stream5_IRQHandler(void)
{
    if (nvic_callbacks[68]) { nvic_callbacks[68](); }
}

/* ========================= IRQ 69 - DMA2_Stream6 ====================== */
__attribute__((weak)) void DMA2_Stream6_IRQHandler(void)
{
    if (nvic_callbacks[69]) { nvic_callbacks[69](); }
}

/* ========================= IRQ 70 - DMA2_Stream7 ====================== */
__attribute__((weak)) void DMA2_Stream7_IRQHandler(void)
{
    if (nvic_callbacks[70]) { nvic_callbacks[70](); }
}

/* ========================= IRQ 71 - USART6 ============================ */
__attribute__((weak)) void USART6_IRQHandler(void)
{
    if (nvic_callbacks[71]) { nvic_callbacks[71](); }
}

/* ========================= IRQ 72 - I2C3_EV =========================== */
__attribute__((weak)) void I2C3_EV_IRQHandler(void)
{
    if (nvic_callbacks[72]) { nvic_callbacks[72](); }
}

/* ========================= IRQ 73 - I2C3_ER =========================== */
__attribute__((weak)) void I2C3_ER_IRQHandler(void)
{
    if (nvic_callbacks[73]) { nvic_callbacks[73](); }
}

/* ========================= IRQ 74 - OTG_HS_EP1_OUT ==================== */
__attribute__((weak)) void OTG_HS_EP1_OUT_IRQHandler(void)
{
    if (nvic_callbacks[74]) { nvic_callbacks[74](); }
}

/* ========================= IRQ 75 - OTG_HS_EP1_IN ===================== */
__attribute__((weak)) void OTG_HS_EP1_IN_IRQHandler(void)
{
    if (nvic_callbacks[75]) { nvic_callbacks[75](); }
}

/* ========================= IRQ 76 - OTG_HS_WKUP ======================= */
__attribute__((weak)) void OTG_HS_WKUP_IRQHandler(void)
{
    if (nvic_callbacks[76]) { nvic_callbacks[76](); }
}

/* ========================= IRQ 77 - OTG_HS ============================ */
__attribute__((weak)) void OTG_HS_IRQHandler(void)
{
    if (nvic_callbacks[77]) { nvic_callbacks[77](); }
}

/* ========================= IRQ 78 - DCMI ============================== */
__attribute__((weak)) void DCMI_IRQHandler(void)
{
    if (nvic_callbacks[78]) { nvic_callbacks[78](); }
}

/* ========================= IRQ 79 - Reserved ========================= */
__attribute__((weak)) void RESERVED79_IRQHandler(void)
{
    if (nvic_callbacks[79]) { nvic_callbacks[79](); }
}

/* ========================= IRQ 80 - RNG =============================== */
__attribute__((weak)) void RNG_IRQHandler(void)
{
    if (nvic_callbacks[80]) { nvic_callbacks[80](); }
}

/* ========================= IRQ 81 - FPU =============================== */
__attribute__((weak)) void FPU_IRQHandler(void)
{
    if (nvic_callbacks[81]) { nvic_callbacks[81](); }
}

/* ========================= IRQ 82 - UART7 ============================= */
__attribute__((weak)) void UART7_IRQHandler(void)
{
    if (nvic_callbacks[82]) { nvic_callbacks[82](); }
}

/* ========================= IRQ 83 - UART8 ============================= */
__attribute__((weak)) void UART8_IRQHandler(void)
{
    if (nvic_callbacks[83]) { nvic_callbacks[83](); }
}

/* ========================= IRQ 84 - SPI4 ============================== */
__attribute__((weak)) void SPI4_IRQHandler(void)
{
    if (nvic_callbacks[84]) { nvic_callbacks[84](); }
}

/* ========================= IRQ 85 - SPI5 ============================== */
__attribute__((weak)) void SPI5_IRQHandler(void)
{
    if (nvic_callbacks[85]) { nvic_callbacks[85](); }
}

/* ========================= IRQ 86 - SPI6 ============================== */
__attribute__((weak)) void SPI6_IRQHandler(void)
{
    if (nvic_callbacks[86]) { nvic_callbacks[86](); }
}

/* ========================= IRQ 87 - SAI1 ============================== */
__attribute__((weak)) void SAI1_IRQHandler(void)
{
    if (nvic_callbacks[87]) { nvic_callbacks[87](); }
}

/* ========================= IRQ 88 - LTDC ============================== */
__attribute__((weak)) void LTDC_IRQHandler(void)
{
    if (nvic_callbacks[88]) { nvic_callbacks[88](); }
}

/* ========================= IRQ 89 - LTDC_ER =========================== */
__attribute__((weak)) void LTDC_ER_IRQHandler(void)
{
    if (nvic_callbacks[89]) { nvic_callbacks[89](); }
}

/* ========================= IRQ 90 - DMA2D ============================= */
__attribute__((weak)) void DMA2D_IRQHandler(void)
{
    if (nvic_callbacks[90]) { nvic_callbacks[90](); }
}

/* ========================= IRQ 91 - SAI2 ============================== */
__attribute__((weak)) void SAI2_IRQHandler(void)
{
    if (nvic_callbacks[91]) { nvic_callbacks[91](); }
}

/* ========================= IRQ 92 - QUADSPI =========================== */
__attribute__((weak)) void QUADSPI_IRQHandler(void)
{
    if (nvic_callbacks[92]) { nvic_callbacks[92](); }
}

/* ========================= IRQ 93 - LPTIM1 ============================ */
__attribute__((weak)) void LPTIM1_IRQHandler(void)
{
    if (nvic_callbacks[93]) { nvic_callbacks[93](); }
}

/* ========================= IRQ 94 - CEC =============================== */
__attribute__((weak)) void CEC_IRQHandler(void)
{
    if (nvic_callbacks[94]) { nvic_callbacks[94](); }
}

/* ========================= IRQ 95 - I2C4_EV =========================== */
__attribute__((weak)) void I2C4_EV_IRQHandler(void)
{
    if (nvic_callbacks[95]) { nvic_callbacks[95](); }
}

/* ========================= IRQ 96 - I2C4_ER =========================== */
__attribute__((weak)) void I2C4_ER_IRQHandler(void)
{
    if (nvic_callbacks[96]) { nvic_callbacks[96](); }
}

/* ========================= IRQ 97 - SPDIF_RX ========================== */
__attribute__((weak)) void SPDIF_RX_IRQHandler(void)
{
    if (nvic_callbacks[97]) { nvic_callbacks[97](); }
}

/* ========================= IRQ 98 - OTG_FS_EP1_OUT ==================== */
__attribute__((weak)) void OTG_FS_EP1_OUT_IRQHandler(void)
{
    if (nvic_callbacks[98]) { nvic_callbacks[98](); }
}

/* ========================= IRQ 99 - OTG_FS_EP1_IN ===================== */
__attribute__((weak)) void OTG_FS_EP1_IN_IRQHandler(void)
{
    if (nvic_callbacks[99]) { nvic_callbacks[99](); }
}

/* ========================= IRQ 100 - OTG_FS_WKUP ====================== */
__attribute__((weak)) void OTG_FS_WKUP_IRQHandler(void)
{
    if (nvic_callbacks[100]) { nvic_callbacks[100](); }
}

/* ========================= IRQ 101 - OTG_FS =========================== */
__attribute__((weak)) void OTG_FS_IRQHandler(void)
{
    if (nvic_callbacks[101]) { nvic_callbacks[101](); }
}

/* ========================= IRQ 102 - DMAMUX1_OVR ====================== */
__attribute__((weak)) void DMAMUX1_OVR_IRQHandler(void)
{
    if (nvic_callbacks[102]) { nvic_callbacks[102](); }
}

/* ========================= IRQ 103 - HRTIM1_Master ==================== */
__attribute__((weak)) void HRTIM1_Master_IRQHandler(void)
{
    if (nvic_callbacks[103]) { nvic_callbacks[103](); }
}

/* ========================= IRQ 104 - HRTIM1_TIMA ====================== */
__attribute__((weak)) void HRTIM1_TIMA_IRQHandler(void)
{
    if (nvic_callbacks[104]) { nvic_callbacks[104](); }
}

/* ========================= IRQ 105 - HRTIM1_TIMB ====================== */
__attribute__((weak)) void HRTIM1_TIMB_IRQHandler(void)
{
    if (nvic_callbacks[105]) { nvic_callbacks[105](); }
}

/* ========================= IRQ 106 - HRTIM1_TIMC ====================== */
__attribute__((weak)) void HRTIM1_TIMC_IRQHandler(void)
{
    if (nvic_callbacks[106]) { nvic_callbacks[106](); }
}

/* ========================= IRQ 107 - HRTIM1_TIMD ====================== */
__attribute__((weak)) void HRTIM1_TIMD_IRQHandler(void)
{
    if (nvic_callbacks[107]) { nvic_callbacks[107](); }
}

/* ========================= IRQ 108 - HRTIM1_TIME ====================== */
__attribute__((weak)) void HRTIM1_TIME_IRQHandler(void)
{
    if (nvic_callbacks[108]) { nvic_callbacks[108](); }
}

/* ========================= IRQ 109 - HRTIM1_FLT ======================= */
__attribute__((weak)) void HRTIM1_FLT_IRQHandler(void)
{
    if (nvic_callbacks[109]) { nvic_callbacks[109](); }
}

/* ========================= IRQ 110 - DFSDM1_FLT0 ====================== */
__attribute__((weak)) void DFSDM1_FLT0_IRQHandler(void)
{
    if (nvic_callbacks[110]) { nvic_callbacks[110](); }
}

/* ========================= IRQ 111 - DFSDM1_FLT1 ====================== */
__attribute__((weak)) void DFSDM1_FLT1_IRQHandler(void)
{
    if (nvic_callbacks[111]) { nvic_callbacks[111](); }
}

/* ========================= IRQ 112 - DFSDM1_FLT2 ====================== */
__attribute__((weak)) void DFSDM1_FLT2_IRQHandler(void)
{
    if (nvic_callbacks[112]) { nvic_callbacks[112](); }
}

/* ========================= IRQ 113 - DFSDM1_FLT3 ====================== */
__attribute__((weak)) void DFSDM1_FLT3_IRQHandler(void)
{
    if (nvic_callbacks[113]) { nvic_callbacks[113](); }
}

/* ========================= IRQ 114 - SAI3 ============================= */
__attribute__((weak)) void SAI3_IRQHandler(void)
{
    if (nvic_callbacks[114]) { nvic_callbacks[114](); }
}

/* ========================= IRQ 115 - SWPMI1 =========================== */
__attribute__((weak)) void SWPMI1_IRQHandler(void)
{
    if (nvic_callbacks[115]) { nvic_callbacks[115](); }
}

/* ========================= IRQ 116 - TIM15 ============================ */
__attribute__((weak)) void TIM15_IRQHandler(void)
{
    if (nvic_callbacks[116]) { nvic_callbacks[116](); }
}

/* ========================= IRQ 117 - TIM16 ============================ */
__attribute__((weak)) void TIM16_IRQHandler(void)
{
    if (nvic_callbacks[117]) { nvic_callbacks[117](); }
}

/* ========================= IRQ 118 - TIM17 ============================ */
__attribute__((weak)) void TIM17_IRQHandler(void)
{
    if (nvic_callbacks[118]) { nvic_callbacks[118](); }
}

/* ========================= IRQ 119 - MDIOS_WKUP ======================= */
__attribute__((weak)) void MDIOS_WKUP_IRQHandler(void)
{
    if (nvic_callbacks[119]) { nvic_callbacks[119](); }
}

/* ========================= IRQ 120 - MDIOS ============================ */
__attribute__((weak)) void MDIOS_IRQHandler(void)
{
    if (nvic_callbacks[120]) { nvic_callbacks[120](); }
}

/* ========================= IRQ 121 - JPEG ============================= */
__attribute__((weak)) void JPEG_IRQHandler(void)
{
    if (nvic_callbacks[121]) { nvic_callbacks[121](); }
}

/* ========================= IRQ 122 - MDMA ============================= */
__attribute__((weak)) void MDMA_IRQHandler(void)
{
    if (nvic_callbacks[122]) { nvic_callbacks[122](); }
}

/* ========================= IRQ 123 - Reserved ========================= */
__attribute__((weak)) void RESERVED123_IRQHandler(void)
{
    if (nvic_callbacks[123]) { nvic_callbacks[123](); }
}

/* ========================= IRQ 124 - SDMMC2 =========================== */
__attribute__((weak)) void SDMMC2_IRQHandler(void)
{
    if (nvic_callbacks[124]) { nvic_callbacks[124](); }
}

/* ========================= IRQ 125 - HSEM1 ============================ */
__attribute__((weak)) void HSEM1_IRQHandler(void)
{
    if (nvic_callbacks[125]) { nvic_callbacks[125](); }
}

/* ========================= IRQ 126 - Reserved ========================= */
__attribute__((weak)) void RESERVED126_IRQHandler(void)
{
    if (nvic_callbacks[126]) { nvic_callbacks[126](); }
}

/* ========================= IRQ 127 - ADC3 ============================= */
__attribute__((weak)) void ADC3_IRQHandler(void)
{
    if (nvic_callbacks[127]) { nvic_callbacks[127](); }
}

/* ========================= IRQ 128 - DMAMUX2_OVR ====================== */
__attribute__((weak)) void DMAMUX2_OVR_IRQHandler(void)
{
    if (nvic_callbacks[128]) { nvic_callbacks[128](); }
}

/* ========================= IRQ 129 - BDMA_Channel0 ==================== */
__attribute__((weak)) void BDMA_Channel0_IRQHandler(void)
{
    if (nvic_callbacks[129]) { nvic_callbacks[129](); }
}

/* ========================= IRQ 130 - BDMA_Channel1 ==================== */
__attribute__((weak)) void BDMA_Channel1_IRQHandler(void)
{
    if (nvic_callbacks[130]) { nvic_callbacks[130](); }
}

/* ========================= IRQ 131 - BDMA_Channel2 ==================== */
__attribute__((weak)) void BDMA_Channel2_IRQHandler(void)
{
    if (nvic_callbacks[131]) { nvic_callbacks[131](); }
}

/* ========================= IRQ 132 - BDMA_Channel3 ==================== */
__attribute__((weak)) void BDMA_Channel3_IRQHandler(void)
{
    if (nvic_callbacks[132]) { nvic_callbacks[132](); }
}

/* ========================= IRQ 133 - BDMA_Channel4 ==================== */
__attribute__((weak)) void BDMA_Channel4_IRQHandler(void)
{
    if (nvic_callbacks[133]) { nvic_callbacks[133](); }
}

/* ========================= IRQ 134 - BDMA_Channel5 ==================== */
__attribute__((weak)) void BDMA_Channel5_IRQHandler(void)
{
    if (nvic_callbacks[134]) { nvic_callbacks[134](); }
}

/* ========================= IRQ 135 - BDMA_Channel6 ==================== */
__attribute__((weak)) void BDMA_Channel6_IRQHandler(void)
{
    if (nvic_callbacks[135]) { nvic_callbacks[135](); }
}

/* ========================= IRQ 136 - BDMA_Channel7 ==================== */
__attribute__((weak)) void BDMA_Channel7_IRQHandler(void)
{
    if (nvic_callbacks[136]) { nvic_callbacks[136](); }
}

/* ========================= IRQ 137 - COMP ============================= */
__attribute__((weak)) void COMP_IRQHandler(void)
{
    if (nvic_callbacks[137]) { nvic_callbacks[137](); }
}

/* ========================= IRQ 138 - LPTIM2 =========================== */
__attribute__((weak)) void LPTIM2_IRQHandler(void)
{
    if (nvic_callbacks[138]) { nvic_callbacks[138](); }
}

/* ========================= IRQ 139 - LPTIM3 =========================== */
__attribute__((weak)) void LPTIM3_IRQHandler(void)
{
    if (nvic_callbacks[139]) { nvic_callbacks[139](); }
}

/* ========================= IRQ 140 - LPTIM4 =========================== */
__attribute__((weak)) void LPTIM4_IRQHandler(void)
{
    if (nvic_callbacks[140]) { nvic_callbacks[140](); }
}

/* ========================= IRQ 141 - LPTIM5 =========================== */
__attribute__((weak)) void LPTIM5_IRQHandler(void)
{
    if (nvic_callbacks[141]) { nvic_callbacks[141](); }
}

/* ========================= IRQ 142 - LPUART1 ========================== */
__attribute__((weak)) void LPUART1_IRQHandler(void)
{
    if (nvic_callbacks[142]) { nvic_callbacks[142](); }
}

/* ========================= IRQ 143 - Reserved ========================= */
__attribute__((weak)) void RESERVED143_IRQHandler(void)
{
    if (nvic_callbacks[143]) { nvic_callbacks[143](); }
}

/* ========================= IRQ 144 - CRS ============================== */
__attribute__((weak)) void CRS_IRQHandler(void)
{
    if (nvic_callbacks[144]) { nvic_callbacks[144](); }
}

/* ========================= IRQ 145 - ECC ============================== */
__attribute__((weak)) void ECC_IRQHandler(void)
{
    if (nvic_callbacks[145]) { nvic_callbacks[145](); }
}

/* ========================= IRQ 146 - SAI4 ============================= */
__attribute__((weak)) void SAI4_IRQHandler(void)
{
    if (nvic_callbacks[146]) { nvic_callbacks[146](); }
}

/* ========================= IRQ 147..148 Reserved ====================== */
__attribute__((weak)) void RESERVED147_IRQHandler(void)
{
    if (nvic_callbacks[147]) { nvic_callbacks[147](); }
}
__attribute__((weak)) void RESERVED148_IRQHandler(void)
{
    if (nvic_callbacks[148]) { nvic_callbacks[148](); }
}

/* ========================= IRQ 149 - WAKEUP_PIN ======================= */
__attribute__((weak)) void WAKEUP_PIN_IRQHandler(void)
{
    if (nvic_callbacks[149]) { nvic_callbacks[149](); }
}
