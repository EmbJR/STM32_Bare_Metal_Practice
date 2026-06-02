/**
 * @file    FlashF051_Example.c
 * @brief   Example demonstrating Flash driver usage for STM32F051R8T6
 * @details Shows:
 *          - Flash initialization
 *          - Page erasing
 *          - Data programming (half-word, word, buffer)
 *          - Data verification
 *          - Error handling
 * 
 * @note    This example demonstrates various Flash operations.
 *          Choose appropriate operations for your application.
 */

#include "FlashF051.h"

/* ============================================================================
   Example Configuration
   ========================================================================== */

/* Define a user data area in Flash (last 4 KB for configuration storage) */
#define USER_DATA_PAGE_START    60U                                /* Page 60 */
#define USER_DATA_PAGE_COUNT    4U                                 /* 4 KB total */
#define USER_DATA_ADDR          FLASH_GetPageAddress(USER_DATA_PAGE_START)

/* Example data structures */
#define DATA_SIZE               256U

/* ============================================================================
   Example 1: Basic Flash Initialization and Configuration
   ========================================================================== */

/**
 * @brief  Initialize Flash for 48 MHz system clock
 * @param  None
 * @retval Flash_Status_t: Initialization status
 */
Flash_Status_t Example_FlashInit(void)
{
    /* Initialize Flash controller */
    FLASH_Init();
    
    /* Configure for 48 MHz operation */
    /* STM32F051 requires:
     * - 1 wait state for frequencies > 24 MHz
     * - Prefetch buffer enabled for better performance */
    return FLASH_ConfigureForFrequency(48000000);
}

/* ============================================================================
   Example 2: Program Single Half-Word
   ========================================================================== */

/**
 * @brief  Program a single 16-bit value to Flash
 * @param  address: Target Flash address
 * @param  value: 16-bit value to program
 * @retval Flash_Status_t: Operation status
 * @note   Address must be half-word aligned and page must be erased first
 */
Flash_Status_t Example_ProgramHalfWord(uint32_t address, uint16_t value)
{
    Flash_Status_t status;
    
    /* Verify address is valid */
    if (!FLASH_IsValidAddress(address)) {
        return FLASH_STATUS_ADDRESS_INVALID;
    }
    
    /* Program the half-word */
    status = FLASH_ProgramHalfWord(address, value);
    
    if (status == FLASH_STATUS_OK) {
        /* Verify the programming was successful */
        if (!FLASH_VerifyHalfWord(address, value)) {
            return FLASH_STATUS_PGERR;
        }
    }
    
    return status;
}

/* ============================================================================
   Example 3: Program Single Word (32-bit)
   ========================================================================== */

/**
 * @brief  Program a single 32-bit value to Flash
 * @param  address: Target Flash address
 * @param  value: 32-bit value to program
 * @retval Flash_Status_t: Operation status
 */
Flash_Status_t Example_ProgramWord(uint32_t address, uint32_t value)
{
    Flash_Status_t status;
    
    /* Verify address is valid and word-aligned */
    if (!FLASH_IsValidAddress(address) || (address & 0x03)) {
        return FLASH_STATUS_ADDRESS_INVALID;
    }
    
    /* Program the word */
    status = FLASH_ProgramWord(address, value);
    
    if (status == FLASH_STATUS_OK) {
        /* Verify: read back and compare */
        volatile uint32_t *flash_ptr = (volatile uint32_t *)address;
        if (*flash_ptr != value) {
            return FLASH_STATUS_PGERR;
        }
    }
    
    return status;
}

/* ============================================================================
   Example 4: Erase a Single Page
   ========================================================================== */

/**
 * @brief  Erase a single Flash page (1 KB)
 * @param  page_address: Address within the page to erase
 * @retval Flash_Status_t: Operation status
 * @note   All data in the page is erased to 0xFFFF
 */
Flash_Status_t Example_ErasePage(uint32_t page_address)
{
    Flash_Status_t status;
    volatile uint16_t *verify_ptr;
    uint16_t i;
    
    /* Perform page erase */
    status = FLASH_ErasePage(page_address);
    
    if (status == FLASH_STATUS_OK) {
        /* Verify page is erased (all bytes should be 0xFF) */
        verify_ptr = (volatile uint16_t *)page_address;
        for (i = 0; i < (FLASH_PAGE_SIZE / 2); i++) {
            if (verify_ptr[i] != 0xFFFF) {
                return FLASH_STATUS_PGERR;
            }
        }
    }
    
    return status;
}

/* ============================================================================
   Example 5: Program Data Buffer
   ========================================================================== */

/**
 * @brief  Program buffer to Flash memory
 * @param  flash_addr: Target Flash address (must be half-word aligned)
 * @param  data: Pointer to data buffer
 * @param  length: Number of bytes to program (must be even)
 * @retval Flash_Status_t: Operation status
 * @note   Destination must be erased before programming
 */
Flash_Status_t Example_ProgramBuffer(uint32_t flash_addr, const uint8_t *data, uint16_t length)
{
    Flash_Status_t status;
    
    /* Validate parameters */
    if (data == NULL || length == 0) {
        return FLASH_STATUS_SIZE_INVALID;
    }
    
    if (length & 0x01) {
        return FLASH_STATUS_SIZE_INVALID;
    }
    
    /* Check if buffer fits in valid Flash address space */
    if ((flash_addr + length - 1) > FLASH_APP_END_ADDR) {
        return FLASH_STATUS_ADDRESS_INVALID;
    }
    
    /* Program the buffer */
    status = FLASH_ProgramBuffer(flash_addr, data, length);
    
    if (status == FLASH_STATUS_OK) {
        /* Verify the entire buffer */
        if (!FLASH_VerifyBuffer(flash_addr, data, length)) {
            return FLASH_STATUS_PGERR;
        }
    }
    
    return status;
}

/* ============================================================================
   Example 6: Configuration Storage Pattern
   ========================================================================== */

/**
 * @struct  ConfigData_t
 * @brief   Example configuration data structure
 */
typedef struct {
    uint32_t magic;                    /* Magic number for validation */
    uint16_t version;                  /* Configuration version */
    uint16_t device_id;                /* Device identifier */
    uint32_t parameter1;               /* Example parameter 1 */
    uint32_t parameter2;               /* Example parameter 2 */
    uint32_t parameter3;               /* Example parameter 3 */
    uint16_t checksum;                 /* Simple checksum */
} ConfigData_t;

#define CONFIG_MAGIC            0xDEADBEEFU
#define CONFIG_VERSION          1U

/**
 * @brief  Calculate simple checksum for data
 * @param  data: Pointer to data
 * @param  length: Number of bytes
 * @retval uint16_t: Checksum value
 */
static uint16_t Example_CalculateChecksum(const uint8_t *data, uint16_t length)
{
    uint16_t checksum = 0;
    uint16_t i;
    
    for (i = 0; i < length; i++) {
        checksum += data[i];
    }
    
    return checksum;
}

/**
 * @brief  Write configuration to Flash
 * @param  config: Pointer to configuration data
 * @retval Flash_Status_t: Operation status
 */
Flash_Status_t Example_WriteConfiguration(ConfigData_t *config)
{
    Flash_Status_t status;
    uint8_t *config_bytes = (uint8_t *)config;
    uint16_t config_size = sizeof(ConfigData_t);
    uint32_t config_addr = USER_DATA_ADDR;
    
    if (config == NULL) {
        return FLASH_STATUS_SIZE_INVALID;
    }
    
    /* Erase configuration area (first page of user data) */
    status = FLASH_ErasePage(config_addr);
    if (status != FLASH_STATUS_OK) {
        return status;
    }
    
    /* Calculate and set checksum */
    config->checksum = 0;
    config->checksum = Example_CalculateChecksum(config_bytes, config_size - 2);
    
    /* Program configuration to Flash */
    status = FLASH_ProgramBuffer(config_addr, config_bytes, config_size);
    
    return status;
}

/**
 * @brief  Read configuration from Flash
 * @param  config: Pointer to buffer for configuration data
 * @retval Flash_Status_t: Operation status
 */
Flash_Status_t Example_ReadConfiguration(ConfigData_t *config)
{
    uint8_t *config_bytes = (uint8_t *)config;
    uint16_t config_size = sizeof(ConfigData_t);
    uint32_t config_addr = USER_DATA_ADDR;
    uint16_t i;
    uint16_t calculated_checksum;
    
    if (config == NULL) {
        return FLASH_STATUS_SIZE_INVALID;
    }
    
    /* Read configuration from Flash */
    volatile uint8_t *flash_ptr = (volatile uint8_t *)config_addr;
    for (i = 0; i < config_size; i++) {
        config_bytes[i] = flash_ptr[i];
    }
    
    /* Verify magic number */
    if (config->magic != CONFIG_MAGIC) {
        return FLASH_STATUS_PGERR;
    }
    
    /* Verify checksum */
    calculated_checksum = Example_CalculateChecksum(config_bytes, config_size - 2);
    if (calculated_checksum != config->checksum) {
        return FLASH_STATUS_PGERR;
    }
    
    return FLASH_STATUS_OK;
}

/* ============================================================================
   Example 7: Mass Erase (Use with Caution!)
   ========================================================================== */

/**
 * @brief  Perform mass erase of all Flash memory
 * @param  None
 * @retval Flash_Status_t: Operation status
 * @warning This erases ALL Flash content including application code!
 *          Use only in special circumstances (e.g., firmware update with bootloader)
 */
Flash_Status_t Example_MassErase(void)
{
    return FLASH_MassErase();
}

/* ============================================================================
   Example 8: Complete Application Workflow
   ========================================================================== */

/**
 * @brief  Complete workflow example
 * @param  None
 * @retval 0 if successful, non-zero error code otherwise
 */
int Example_CompleteWorkflow(void)
{
    Flash_Status_t status;
    ConfigData_t config;
    uint8_t test_data[64];
    uint16_t i;
    
    /* Step 1: Initialize Flash */
    status = Example_FlashInit();
    if (status != FLASH_STATUS_OK) {
        return -1;
    }
    
    /* Step 2: Create test data */
    for (i = 0; i < sizeof(test_data); i++) {
        test_data[i] = (uint8_t)(i & 0xFF);
    }
    
    /* Step 3: Erase user data page */
    status = Example_ErasePage(USER_DATA_ADDR);
    if (status != FLASH_STATUS_OK) {
        return -2;
    }
    
    /* Step 4: Program test data */
    status = Example_ProgramBuffer(USER_DATA_ADDR, test_data, sizeof(test_data));
    if (status != FLASH_STATUS_OK) {
        return -3;
    }
    
    /* Step 5: Create and write configuration */
    config.magic = CONFIG_MAGIC;
    config.version = CONFIG_VERSION;
    config.device_id = 0x0051;  /* STM32F051 device ID */
    config.parameter1 = 0x12345678;
    config.parameter2 = 0x9ABCDEF0;
    config.parameter3 = 0xAAAABBBB;
    
    status = Example_WriteConfiguration(&config);
    if (status != FLASH_STATUS_OK) {
        return -4;
    }
    
    /* Step 6: Read back configuration to verify */
    status = Example_ReadConfiguration(&config);
    if (status != FLASH_STATUS_OK) {
        return -5;
    }
    
    /* Step 7: Verify configuration values */
    if (config.device_id != 0x0051 || config.parameter1 != 0x12345678) {
        return -6;
    }
    
    return 0;  /* Success */
}

/* ============================================================================
   Example 9: Page-by-Page Erase
   ========================================================================== */

/**
 * @brief  Erase multiple consecutive pages
 * @param  start_page: Starting page number
 * @param  page_count: Number of pages to erase
 * @retval Flash_Status_t: Operation status
 */
Flash_Status_t Example_ErasePages(uint8_t start_page, uint16_t page_count)
{
    uint32_t start_addr;
    
    if (start_page >= FLASH_PAGES_COUNT) {
        return FLASH_STATUS_ADDRESS_INVALID;
    }
    
    start_addr = FLASH_GetPageAddress(start_page);
    return FLASH_ErasePages(start_addr, page_count);
}

/* ============================================================================
   Example 10: Get Flash Information
   ========================================================================== */

/**
 * @brief  Get Flash memory information
 * @param  None
 * @retval None
 * @note   This is useful for diagnostic and debugging purposes
 */
void Example_GetFlashInfo(void)
{
    /* Flash memory characteristics for STM32F051R8T6:
     * - Total Flash: 64 KB (0x08000000 - 0x0800FFFF)
     * - Page Size: 1024 bytes (0x400)
     * - Number of Pages: 64
     * - Write Unit: 16-bit (half-word)
     * - System clock capability: up to 48 MHz
     * - For frequencies <= 24 MHz: 0 wait states
     * - For frequencies 24-48 MHz: 1 wait state
     */
    
    /* Get current Flash configuration */
    Flash_Latency_t latency = FLASH_GetLatency();
    bool prefetch_ready = FLASH_IsPrefetchReady();
    
    /* Example usage (pseudo-code for reference):
     * printf("Flash Latency: %d wait states\n", latency);
     * printf("Prefetch Buffer: %s\n", prefetch_ready ? "Ready" : "Not Ready");
     * printf("Flash Size: %d KB\n", FLASH_SIZE / 1024);
     * printf("Page Size: %d bytes\n", FLASH_PAGE_SIZE);
     * printf("Total Pages: %d\n", FLASH_PAGES_COUNT);
     */
}

/* ============================================================================
   Example 11: Safe Flash Write with Retry
   ========================================================================== */

/**
 * @brief  Program data with verification and retry on failure
 * @param  flash_addr: Target Flash address
 * @param  data: Source data pointer
 * @param  length: Number of bytes to program
 * @param  max_retries: Maximum number of retry attempts
 * @retval Flash_Status_t: Final operation status
 */
Flash_Status_t Example_SafeProgramWithRetry(uint32_t flash_addr, 
                                           const uint8_t *data, 
                                           uint16_t length, 
                                           uint8_t max_retries)
{
    Flash_Status_t status;
    uint8_t retry_count = 0;
    
    while (retry_count < max_retries) {
        status = FLASH_ProgramBuffer(flash_addr, data, length);
        
        if (status == FLASH_STATUS_OK) {
            /* Verify the data */
            if (FLASH_VerifyBuffer(flash_addr, data, length)) {
                return FLASH_STATUS_OK;
            }
            /* Verification failed, retry */
            retry_count++;
        } else {
            /* Clear errors and retry */
            FLASH_ClearErrors();
            retry_count++;
        }
    }
    
    return FLASH_STATUS_PGERR;
}

/* ============================================================================
   Example 12: Read Flash Data
   ========================================================================== */

/**
 * @brief  Read data from Flash memory
 * @param  flash_addr: Source Flash address
 * @param  buffer: Destination buffer
 * @param  length: Number of bytes to read
 * @retval Flash_Status_t: Operation status
 */
Flash_Status_t Example_ReadFlash(uint32_t flash_addr, uint8_t *buffer, uint16_t length)
{
    uint16_t i;
    volatile uint8_t *flash_ptr;
    
    if (buffer == NULL || length == 0) {
        return FLASH_STATUS_SIZE_INVALID;
    }
    
    if (!FLASH_IsValidAddress(flash_addr)) {
        return FLASH_STATUS_ADDRESS_INVALID;
    }
    
    flash_ptr = (volatile uint8_t *)flash_addr;
    for (i = 0; i < length; i++) {
        buffer[i] = flash_ptr[i];
    }
    
    return FLASH_STATUS_OK;
}
