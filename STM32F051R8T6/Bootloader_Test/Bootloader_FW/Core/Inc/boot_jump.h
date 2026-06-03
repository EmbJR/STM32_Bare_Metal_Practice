/**
 * @file boot_jump.h
 * @brief Bare-metal Bootloader Jump Interface for STM32F0 (ARM Cortex-M0).
 * No CMSIS, HAL, or LL dependencies.
 */

#ifndef BOOT_JUMP_H
#define BOOT_JUMP_H

#include <stdint.h>

/**
 * @brief Return codes for bootloader jump APIs
 */
typedef enum {
    BOOT_JUMP_OK          = 0,
    BOOT_JUMP_ERR_SP      = 1, /* Invalid Stack Pointer value */
    BOOT_JUMP_ERR_HANDLER = 2, /* Invalid Reset Handler address */
    BOOT_JUMP_ERR_GENERIC = 3  /* Generic error */
} boot_jump_status_t;

/**
 * @brief Validates application image headers at a given address
 * @param app_address Start address of the application binary in Flash
 * @return BOOT_JUMP_OK if headers look valid, error code otherwise
 */
boot_jump_status_t boot_jump_validate_app(uint32_t app_address);

/**
 * @brief De-initializes basic core blocks and peripherals to reset state
 */
void boot_jump_system_reset_peripherals(void);

/**
 * @brief Remaps SRAM to address 0x00000000 and copies Vector Table.
 * This is MANDATORY for STM32F0 because it lacks an SCB->VTOR register.
 * @param app_address Start address of the application binary in Flash
 */
void boot_jump_remap_vector_table(uint32_t app_address);

/**
 * @brief Sets Stack Pointer and executes jump to application.
 * This function does not return if successful.
 * @param app_address Start address of the application binary in Flash
 * @return Will only return if an error occurs during pre-checks.
 */
boot_jump_status_t boot_jump_to_app(uint32_t app_address);

#endif /* BOOT_JUMP_H */