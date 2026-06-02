/**
 * @file    FlashF051.h
 * @brief   Flash memory driver for STM32F051R8T6
 * @details Provides API for Flash memory programming, erasing, and configuration
 *          without external dependencies (HAL, CMSIS, etc.)
 * 
 * STM32F051R8T6 Flash Memory Layout:
 * - Total Flash: 64 KB (0x08000000 - 0x0800FFFF)
 * - Page Size: 1024 bytes (0x400)
 * - Number of Pages: 64
 * - Write/Erase Unit: 16-bit (2 bytes)
 */

#ifndef FLASHF051_H
#define FLASHF051_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
   Flash Memory Base Address and Register Offsets
   ========================================================================== */
#define FLASH_BASE_ADDR         0x40022000UL
#define FLASH_ACR_OFFSET        0x00U
#define FLASH_KEYR_OFFSET       0x04U
#define FLASH_OPTKEYR_OFFSET    0x08U
#define FLASH_SR_OFFSET         0x0CU
#define FLASH_CR_OFFSET         0x10U
#define FLASH_AR_OFFSET         0x14U
#define FLASH_OBR_OFFSET        0x1CU
#define FLASH_WRPR_OFFSET       0x20U

/* ============================================================================
   Flash Register Pointers
   ========================================================================== */
#define FLASH_ACR       (*((volatile uint32_t *)(FLASH_BASE_ADDR + FLASH_ACR_OFFSET)))
#define FLASH_KEYR      (*((volatile uint32_t *)(FLASH_BASE_ADDR + FLASH_KEYR_OFFSET)))
#define FLASH_OPTKEYR   (*((volatile uint32_t *)(FLASH_BASE_ADDR + FLASH_OPTKEYR_OFFSET)))
#define FLASH_SR        (*((volatile uint32_t *)(FLASH_BASE_ADDR + FLASH_SR_OFFSET)))
#define FLASH_CR        (*((volatile uint32_t *)(FLASH_BASE_ADDR + FLASH_CR_OFFSET)))
#define FLASH_AR        (*((volatile uint32_t *)(FLASH_BASE_ADDR + FLASH_AR_OFFSET)))
#define FLASH_OBR       (*((volatile uint32_t *)(FLASH_BASE_ADDR + FLASH_OBR_OFFSET)))
#define FLASH_WRPR      (*((volatile uint32_t *)(FLASH_BASE_ADDR + FLASH_WRPR_OFFSET)))

/* ============================================================================
   Flash Access Control Register (ACR) Bits
   ========================================================================== */
#define FLASH_ACR_LATENCY_Pos           0U
#define FLASH_ACR_LATENCY_Msk           (0x7U << FLASH_ACR_LATENCY_Pos)
#define FLASH_ACR_LATENCY_0WS           (0x0U << FLASH_ACR_LATENCY_Pos)  /* 0 wait states */
#define FLASH_ACR_LATENCY_1WS           (0x1U << FLASH_ACR_LATENCY_Pos)  /* 1 wait state */

#define FLASH_ACR_PRFTBE_Pos            4U
#define FLASH_ACR_PRFTBE_Msk            (0x1U << FLASH_ACR_PRFTBE_Pos)
#define FLASH_ACR_PRFTBE                FLASH_ACR_PRFTBE_Msk             /* Prefetch buffer enable */

#define FLASH_ACR_PRFTBS_Pos            5U
#define FLASH_ACR_PRFTBS_Msk            (0x1U << FLASH_ACR_PRFTBS_Pos)
#define FLASH_ACR_PRFTBS                FLASH_ACR_PRFTBS_Msk             /* Prefetch buffer status */

/* ============================================================================
   Flash Status Register (SR) Bits
   ========================================================================== */
#define FLASH_SR_BSY_Pos                0U
#define FLASH_SR_BSY                    (0x1U << FLASH_SR_BSY_Pos)       /* Busy flag */

#define FLASH_SR_PGERR_Pos              2U
#define FLASH_SR_PGERR                  (0x1U << FLASH_SR_PGERR_Pos)     /* Program error */

#define FLASH_SR_WRPRTERR_Pos           4U
#define FLASH_SR_WRPRTERR               (0x1U << FLASH_SR_WRPRTERR_Pos)  /* Write protection error */

#define FLASH_SR_EOP_Pos                5U
#define FLASH_SR_EOP                    (0x1U << FLASH_SR_EOP_Pos)       /* End of operation flag */

/* ============================================================================
   Flash Control Register (CR) Bits
   ========================================================================== */
#define FLASH_CR_PG_Pos                 0U
#define FLASH_CR_PG                     (0x1U << FLASH_CR_PG_Pos)        /* Programming */

#define FLASH_CR_PER_Pos                1U
#define FLASH_CR_PER                    (0x1U << FLASH_CR_PER_Pos)       /* Page erase */

#define FLASH_CR_MER_Pos                2U
#define FLASH_CR_MER                    (0x1U << FLASH_CR_MER_Pos)       /* Mass erase */

#define FLASH_CR_OPTPG_Pos              4U
#define FLASH_CR_OPTPG                  (0x1U << FLASH_CR_OPTPG_Pos)     /* Option byte programming */

#define FLASH_CR_OPTER_Pos              5U
#define FLASH_CR_OPTER                  (0x1U << FLASH_CR_OPTER_Pos)     /* Option byte erase */

#define FLASH_CR_STRT_Pos               6U
#define FLASH_CR_STRT                   (0x1U << FLASH_CR_STRT_Pos)      /* Start operation */

#define FLASH_CR_LOCK_Pos               7U
#define FLASH_CR_LOCK                   (0x1U << FLASH_CR_LOCK_Pos)      /* Lock control bit */

#define FLASH_CR_ERRIE_Pos              10U
#define FLASH_CR_ERRIE                  (0x1U << FLASH_CR_ERRIE_Pos)     /* Error interrupt enable */

#define FLASH_CR_EOPIE_Pos              12U
#define FLASH_CR_EOPIE                  (0x1U << FLASH_CR_EOPIE_Pos)     /* End of operation interrupt enable */

/* ============================================================================
   Flash Option Byte Register (OBR) Bits
   ========================================================================== */
#define FLASH_OBR_OPTERR_Pos            0U
#define FLASH_OBR_OPTERR                (0x1U << FLASH_OBR_OPTERR_Pos)   /* Option byte error */

#define FLASH_OBR_RDP_Pos               1U
#define FLASH_OBR_RDP                   (0x1U << FLASH_OBR_RDP_Pos)      /* Read protection bit */

/* ============================================================================
   Flash Unlock Keys
   ========================================================================== */
#define FLASH_UNLOCK_KEY1               0x45670123UL
#define FLASH_UNLOCK_KEY2               0xCDEF89ABUL
#define FLASH_OPTUNLOCK_KEY1            0x45670123UL
#define FLASH_OPTUNLOCK_KEY2            0xCDEF89ABUL

/* ============================================================================
   Flash Memory Characteristics
   ========================================================================== */
#define FLASH_PAGE_SIZE                 1024U          /* 1 KB */
#define FLASH_SIZE                      (64 * 1024U)   /* 64 KB */
#define FLASH_PAGES_COUNT               64U
#define FLASH_APP_START_ADDR            0x08000000UL
#define FLASH_APP_END_ADDR              0x0800FFFFUL

/* ============================================================================
   Enumerations
   ========================================================================== */

/**
 * @enum Flash_Status_t
 * @brief Flash operation status codes
 */
typedef enum {
    FLASH_STATUS_OK = 0,               /**< Operation successful */
    FLASH_STATUS_BUSY,                 /**< Flash is busy */
    FLASH_STATUS_PGERR,                /**< Programming error */
    FLASH_STATUS_WRPRTERR,             /**< Write protection error */
    FLASH_STATUS_OPTERR,               /**< Option byte error */
    FLASH_STATUS_TIMEOUT,              /**< Operation timeout */
    FLASH_STATUS_ADDRESS_INVALID,      /**< Invalid address */
    FLASH_STATUS_SIZE_INVALID          /**< Invalid size */
} Flash_Status_t;

/**
 * @enum Flash_Latency_t
 * @brief Flash latency (wait states) configuration
 */
typedef enum {
    FLASH_LATENCY_0 = 0,               /**< 0 wait states */
    FLASH_LATENCY_1 = 1                /**< 1 wait state */
} Flash_Latency_t;

/* ============================================================================
   Function Prototypes - Configuration
   ========================================================================== */

/**
 * @brief  Initialize Flash controller
 * @param  None
 * @retval None
 */
void FLASH_Init(void);

/**
 * @brief  Set Flash latency (wait states) based on CPU frequency
 * @param  latency: Latency value (FLASH_LATENCY_0 or FLASH_LATENCY_1)
 * @retval None
 * @note   For STM32F051:
 *         - 0 wait states: fclk < 24 MHz
 *         - 1 wait state: 24 MHz <= fclk <= 48 MHz
 */
void FLASH_SetLatency(Flash_Latency_t latency);

/**
 * @brief  Get current Flash latency setting
 * @param  None
 * @retval Flash_Latency_t: Current latency value
 */
Flash_Latency_t FLASH_GetLatency(void);

/**
 * @brief  Enable prefetch buffer for improved Flash read performance
 * @param  None
 * @retval None
 */
void FLASH_EnablePrefetch(void);

/**
 * @brief  Disable prefetch buffer
 * @param  None
 * @retval None
 */
void FLASH_DisablePrefetch(void);

/**
 * @brief  Get prefetch buffer status
 * @param  None
 * @retval true if prefetch buffer is ready, false otherwise
 */
bool FLASH_IsPrefetchReady(void);

/* ============================================================================
   Function Prototypes - Status and Control
   ========================================================================== */

/**
 * @brief  Get current Flash operation status
 * @param  None
 * @retval Flash_Status_t: Current status
 */
Flash_Status_t FLASH_GetStatus(void);

/**
 * @brief  Check if Flash is busy
 * @param  None
 * @retval true if busy, false if ready
 */
bool FLASH_IsBusy(void);

/**
 * @brief  Wait for Flash operation to complete
 * @param  timeout_ms: Maximum wait time in milliseconds
 * @retval Flash_Status_t: Operation status
 */
Flash_Status_t FLASH_WaitForComplete(uint32_t timeout_ms);

/**
 * @brief  Clear Flash error flags
 * @param  None
 * @retval None
 */
void FLASH_ClearErrors(void);

/**
 * @brief  Clear End-of-Operation (EOP) flag
 * @param  None
 * @retval None
 */
void FLASH_ClearEOP(void);

/**
 * @brief  Unlock Flash for write/erase operations
 * @param  None
 * @retval Flash_Status_t: Operation status
 */
Flash_Status_t FLASH_Unlock(void);

/**
 * @brief  Lock Flash to prevent accidental write/erase
 * @param  None
 * @retval None
 */
void FLASH_Lock(void);

/* ============================================================================
   Function Prototypes - Erase Operations
   ========================================================================== */

/**
 * @brief  Erase a single Flash page (1 KB)
 * @param  page_addr: Address within the page to erase (must be page-aligned)
 * @retval Flash_Status_t: Operation status
 * @note   - Address must be within valid Flash memory range
 *         - Function automatically unlocks and locks Flash
 *         - All data in the page is erased to 0xFFFF
 */
Flash_Status_t FLASH_ErasePage(uint32_t page_addr);

/**
 * @brief  Erase multiple consecutive Flash pages
 * @param  start_addr: Start address (must be page-aligned)
 * @param  page_count: Number of consecutive pages to erase
 * @retval Flash_Status_t: Operation status
 */
Flash_Status_t FLASH_ErasePages(uint32_t start_addr, uint16_t page_count);

/**
 * @brief  Erase entire Flash memory (mass erase)
 * @param  None
 * @retval Flash_Status_t: Operation status
 * @warning This erases ALL Flash content including application code
 */
Flash_Status_t FLASH_MassErase(void);

/* ============================================================================
   Function Prototypes - Program Operations
   ========================================================================== */

/**
 * @brief  Program 16-bit (half-word) value to Flash
 * @param  addr: Destination address (must be half-word aligned)
 * @param  data: 16-bit value to program
 * @retval Flash_Status_t: Operation status
 * @note   - Destination must be erased (0xFFFF) before programming
 *         - Function automatically unlocks and locks Flash
 */
Flash_Status_t FLASH_ProgramHalfWord(uint32_t addr, uint16_t data);

/**
 * @brief  Program 32-bit (word) value to Flash
 * @param  addr: Destination address (must be word aligned)
 * @param  data: 32-bit value to program
 * @retval Flash_Status_t: Operation status
 * @note   - This performs two consecutive 16-bit programs
 *         - Destination must be erased (0xFFFFFFFF) before programming
 */
Flash_Status_t FLASH_ProgramWord(uint32_t addr, uint32_t data);

/**
 * @brief  Program buffer of data to Flash
 * @param  dest_addr: Destination address (must be half-word aligned)
 * @param  src_data: Pointer to source data buffer
 * @param  length: Number of bytes to program (must be even)
 * @retval Flash_Status_t: Operation status
 * @note   - Destination memory must be erased before programming
 *         - Length must be even (programming in 16-bit units)
 *         - This is more efficient than individual half-word programs
 */
Flash_Status_t FLASH_ProgramBuffer(uint32_t dest_addr, const uint8_t *src_data, uint16_t length);

/* ============================================================================
   Function Prototypes - Utility
   ========================================================================== */

/**
 * @brief  Verify Flash memory programming
 * @param  addr: Address to verify
 * @param  expected_value: Expected 16-bit value
 * @retval true if value matches, false otherwise
 */
bool FLASH_VerifyHalfWord(uint32_t addr, uint16_t expected_value);

/**
 * @brief  Verify buffer programming in Flash
 * @param  dest_addr: Destination address
 * @param  src_data: Source data buffer for comparison
 * @param  length: Number of bytes to verify (must be even)
 * @retval true if all data matches, false otherwise
 */
bool FLASH_VerifyBuffer(uint32_t dest_addr, const uint8_t *src_data, uint16_t length);

/**
 * @brief  Validate if address is within Flash memory
 * @param  addr: Address to validate
 * @retval true if valid Flash address, false otherwise
 */
bool FLASH_IsValidAddress(uint32_t addr);

/**
 * @brief  Get page number from address
 * @param  addr: Flash address
 * @retval Page number (0-63), or -1 if invalid
 */
int16_t FLASH_GetPageNumber(uint32_t addr);

/**
 * @brief  Get page start address from page number
 * @param  page_num: Page number (0-63)
 * @retval Page start address, or 0 if invalid
 */
uint32_t FLASH_GetPageAddress(uint8_t page_num);

/**
 * @brief  Configure Flash timing based on system clock frequency
 * @param  sysclk_freq_hz: System clock frequency in Hz
 * @retval Flash_Status_t: Operation status
 * @note   Automatically sets appropriate latency and enables prefetch buffer
 */
Flash_Status_t FLASH_ConfigureForFrequency(uint32_t sysclk_freq_hz);

#endif /* FLASHF051_H */
