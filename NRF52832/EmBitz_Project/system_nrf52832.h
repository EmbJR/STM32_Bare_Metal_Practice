/* system_nrf52832.h - System Configuration for nRF52832 */
/* Bare-metal driver configuration */

#ifndef SYSTEM_NRF52832_H
#define SYSTEM_NRF52832_H

/* nRF52832 Device Definitions */
#ifndef NRF52832_XXAA
#define NRF52832_XXAA
#endif
#define __CM4_REV                 0x0001U
#define __MPU_PRESENT             1
#define __VTOR_PRESENT            1
#define __NVIC_PRIO_BITS          3
#define __Vendor_SysTickConfig    0
#define __FPU_PRESENT             1

/* ARM Cortex-M4F Core Configuration */
#ifndef __CORTEX_M
#define __CORTEX_M                4
#endif
#ifndef ARM_MATH_CM4
#define ARM_MATH_CM4
#endif

/* System Clock Configuration */
#define SYSTEM_CLOCK              64000000UL  /* 64 MHz */

/* Memory Sizes */
#define FLASH_SIZE                (512 * 1024)  /* 512 KB */
#define RAM_SIZE                  (64 * 1024)   /* 64 KB */

/* Interrupt Number Definitions */
typedef enum {
    /* Core Exceptions */
    NonMaskableInt_IRQn     = -14,
    HardFault_IRQn         = -13,
    MemoryManagement_IRQn  = -12,
    BusFault_IRQn          = -11,
    UsageFault_IRQn        = -10,
    SVCall_IRQn            = -5,
    DebugMonitor_IRQn      = -4,
    PendSV_IRQn            = -2,
    SysTick_IRQn           = -1,
    
    /* External Interrupts */
    POWER_CLOCK_IRQn       = 0,
    RADIO_IRQn             = 1,
    UARTE0_UART0_IRQn      = 2,
    SPIM0_SPIS0_TWIM0_TWIS0_IRQn = 3,
    SPIM1_SPIS1_TWIM1_TWIS1_IRQn = 4,
    NFCT_IRQn              = 5,
    GPIOTE_IRQn            = 6,
    SAADC_IRQn             = 7,
    TIMER0_IRQn            = 8,
    TIMER1_IRQn            = 9,
    TIMER2_IRQn            = 10,
    RTC0_IRQn              = 11,
    TEMP_IRQn              = 12,
    RNG_IRQn               = 13,
    ECB_IRQn               = 14,
    CCM_AAR_IRQn           = 15,
    WDT_IRQn               = 16,
    RTC1_IRQn              = 17,
    QDEC_IRQn              = 18,
    COMP_LPCOMP_IRQn       = 19,
    SWI0_EGU0_IRQn         = 20,
    SWI1_EGU1_IRQn         = 21,
    SWI2_EGU2_IRQn         = 22,
    SWI3_EGU3_IRQn         = 23,
    SWI4_EGU4_IRQn         = 24,
    SWI5_EGU5_IRQn         = 25
} IRQn_Type;

/* Core Includes */
#include <stdint.h>
#include <stdbool.h>

/* SystemCoreClock Variable */
extern uint32_t SystemCoreClock;

/* System Initialization Functions */
void SystemInit(void);
void SystemCoreClockUpdate(void);

/* Interrupt Enable/Disable Macros */
#define __enable_irq()    __asm volatile ("cpsie i" : : : "memory")
#define __disable_irq()   __asm volatile ("cpsid i" : : : "memory")

/* DMB, DSB, ISB Macros */
#define __DMB()           __asm volatile ("dmb" : : : "memory")
#define __DSB()           __asm volatile ("dsb" : : : "memory")
#define __ISB()           __asm volatile ("isb" : : : "memory")

/* NOP Macro */
#define __NOP()           __asm volatile ("nop" : : : "memory")

#endif /* SYSTEM_NRF52832_H */
