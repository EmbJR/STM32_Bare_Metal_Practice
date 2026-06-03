/**
 * @file boot_jump.c
 * @brief Bare-metal Bootloader Jump Implementation for STM32F0 (ARM Cortex-M0).
 * Provides clean, pointer-based registers manipulation without CMSIS.
 */

#include "boot_jump.h"

/* ========================================================================== */
/* BARE-METAL REGISTER DEFINITIONS                       */
/* ========================================================================== */

/* NVIC Registers */
#define NVIC_ICER_REG       (*((volatile uint32_t*)0xE000E180U)) /* Interrupt Clear-Enable Register */
#define NVIC_ICPR_REG       (*((volatile uint32_t*)0xE000E280U)) /* Interrupt Clear-Pending Register */

/* System Control Block (SCB) Registers */
#define SCB_ICSR_REG        (*((volatile uint32_t*)0xE000ED04U)) /* Interrupt Control and State Register */

/* STM32F0 Specific System Configuration (SYSCFG) Registers */
#define RCC_APB2ENR_REG     (*((volatile uint32_t*)0x40021018U)) /* APB2 Peripheral Clock Enable Register */
#define SYSCFG_CFGR1_REG    (*((volatile uint32_t*)0x40010000U)) /* SYSCFG configuration register 1 */

/* SRAM and Vector Table Constants */
#define STM32F0_SRAM_START  0x20000000U
#define VECTOR_TABLE_SIZE   48U /* STM32F0 typically has 48 exception vectors (4 bytes each) */

/* ========================================================================== */
/* DRIVER API IMPLEMENTATIONS                           */
/* ========================================================================== */

/**
 * @brief Validates if the stack pointer and reset handler targets look sane.
 */
boot_jump_status_t boot_jump_validate_app(uint32_t app_address)
{
    /* 1. Retrieve Stack Pointer from Vector Table index 0 */
    uint32_t app_stack_pointer = *((volatile uint32_t*)app_address);
    
    /* 2. Retrieve Reset Vector address from Vector Table index 1 */
    uint32_t app_reset_vector = *((volatile uint32_t*)(app_address + 4));

    /* * Safety Check: 
     * Stack pointer on STM32F0 must point to SRAM (Starts at 0x20000000).
     * Exact RAM boundary changes across STM32F0 variants (e.g., 4KB, 8KB, 16KB, 32KB).
     * Checking if the Stack Pointer is within the 0x20000000 - 0x20008000 (32KB max) region.
     */
    if ((app_stack_pointer < STM32F0_SRAM_START) || (app_stack_pointer > 0x20008000U))
    {
        return BOOT_JUMP_ERR_SP;
    }

    /* * Reset vector address must lie inside Flash region (0x08000000 up to limits)
     * AND it must be an odd address (Thumb instruction set mandate: bit 0 of the
     * address must be 1).
     */
    if ((app_reset_vector < 0x08000000U) || (app_reset_vector > 0x08040000U) || ((app_reset_vector & 0x00000001U) == 0U))
    {
        return BOOT_JUMP_ERR_HANDLER;
    }

    return BOOT_JUMP_OK;
}

/**
 * @brief Cleans up NVIC interrupts, pending states, and system exceptions 
 * to ensure a pristine state before launching the user application.
 */
void boot_jump_system_reset_peripherals(void)
{
    /* Disable all global interrupts in the CPU Core */
    __asm volatile ("cpsid i" : : : "memory");

    /* Clear all NVIC interrupt enable registers */
    NVIC_ICER_REG = 0xFFFFFFFFU;

    /* Clear all NVIC pending interrupt flags */
    NVIC_ICPR_REG = 0xFFFFFFFFU;

    /* Clear active/pending states of system exceptions (SysTick, PendSV, SVC) */
    SCB_ICSR_REG |= (1U << 25); /* Clear PendSV exception pending bit if set */
}

/**
 * @brief Dynamic Remap Implementation. Copies Flash Vector Table of the application 
 * into SRAM and configures the SYSCFG registers to remap SRAM to 0x00000000.
 */
void boot_jump_remap_vector_table(uint32_t app_address)
{
    volatile uint32_t *sram_vectors = (volatile uint32_t*)STM32F0_SRAM_START;
    volatile uint32_t *flash_vectors = (volatile uint32_t*)app_address;

    /* 1. Copy the Vector Table from flash app_address to the start of SRAM */
    for (uint32_t i = 0; i < VECTOR_TABLE_SIZE; i++)
    {
        sram_vectors[i] = flash_vectors[i];
    }

    /* 2. Enable Clock for the SYSCFG (System Configuration) Peripheral */
    /* On STM32F0, SYSCFG clock enable is bit 0 of RCC_APB2ENR */
    RCC_APB2ENR_REG |= (1U << 0);

    /* 3. Configure SYSCFG to Remap SRAM to address 0x00000000 */
    /* Write 0x3 (SRAM mapped at 0x00000000) into bits [1:0] MEM_MODE of SYSCFG_CFGR1 */
    uint32_t temp = SYSCFG_CFGR1_REG;
    temp &= ~(0x00000003U); /* Clear MEM_MODE bits */
    temp |= (0x00000003U);  /* Set MEM_MODE to SRAM Remap */
    SYSCFG_CFGR1_REG = temp;
}

/**
 * @brief Performs the actual jump to the User Application.
 */
boot_jump_status_t boot_jump_to_app(uint32_t app_address)
{
    /* 1. Validate application integrity */
    boot_jump_status_t status = boot_jump_validate_app(app_address);
    if (status != BOOT_JUMP_OK)
    {
        return status;
    }

    /* 2. Prepare System Peripherals and Disable Interrupts */
    boot_jump_system_reset_peripherals();

    /* 3. Execute Vector Remap to SRAM (Mandatory F0 Step) */
    boot_jump_remap_vector_table(app_address);

    /* 4. Prepare Function pointer for Reset Handler Jump */
    typedef void (*app_reset_handler_t)(void);
    
    /* Reset Handler address resides at (offset + 4) */
    uint32_t jump_address = *((volatile uint32_t*)(app_address + 4));
    app_reset_handler_t jump_to_application = (app_reset_handler_t)jump_address;

    /* 5. Extract Application's Main Stack Pointer (MSP) */
    uint32_t msp_val = *((volatile uint32_t*)app_address);

    /* * 6. inline Assembler to:
     * - Set Main Stack Pointer (MSP) to application stack value
     * - Enable global interrupts (cpsie i)
     * - Execute branch to application reset handler
     */
    __asm volatile (
        "msr msp, %0\n\t"  /* Load MSP with app stack pointer value */
        "cpsie i\n\t"     /* Re-enable interrupts */
        "bx %1\n\t"        /* Branch to reset handler */
        :
        : "r" (msp_val), "r" (jump_to_application)
        : "memory"
    );

    /* Code should never reach this point if jump succeeded */
    return BOOT_JUMP_ERR_GENERIC;
}