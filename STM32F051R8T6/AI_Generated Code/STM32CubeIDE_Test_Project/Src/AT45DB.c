/**
 * @file    AT45DB.c
 * @brief   AT45DB041D/AT45DB041E DataFlash Driver for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-02-28
 */

#include "AT45DB.h"
#include "rcc.h"

/*============================================================================
 * Helper Functions
 *============================================================================*/

/**
 * @brief   Select the AT45DB chip by pulling CS low
 */
void AT45DB_Select(AT45DB_HandleTypeDef *handle) {
    GPIO_ResetPin(handle->cs_port, handle->cs_pin);
}

/**
 * @brief   Deselect the AT45DB chip by pulling CS high
 */
void AT45DB_Deselect(AT45DB_HandleTypeDef *handle) {
    GPIO_SetPin(handle->cs_port, handle->cs_pin);
}

/**
 * @brief   Encode address according to current page size mode
 */
void AT45DB_EncodeAddress(uint32_t address, uint8_t *addr_bytes, uint16_t page_size) {
    if (page_size == AT45DB_PAGE_SIZE_BINARY) {
        // Binary page mode: A18..A8 = page, A7..A0 = byte
        addr_bytes[0] = (address >> 16) & 0xFF;
        addr_bytes[1] = (address >> 8) & 0xFF;
        addr_bytes[2] = address & 0xFF;
    } else {
        // Standard page mode: PA10..PA0, BA8..BA0
        uint16_t page = (address / AT45DB_PAGE_SIZE_STANDARD);
        uint16_t byte_in_page = address % AT45DB_PAGE_SIZE_STANDARD;
        
        addr_bytes[0] = (page >> 3) & 0xFF;
        addr_bytes[1] = ((page & 0x07) << 5) | ((byte_in_page >> 8) & 0x1F);
        addr_bytes[2] = byte_in_page & 0xFF;
    }
}

/*============================================================================
 * Initialization Functions
 *============================================================================*/

/**
 * @brief   Initialize AT45DB driver
 */
bool AT45DB_Init(AT45DB_HandleTypeDef *handle, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint32_t cs_pin) {
    if (!handle || !hspi || !cs_port) {
        return false;
    }
    
    // Initialize handle
    handle->hspi = hspi;
    handle->cs_port = cs_port;
    handle->cs_pin = cs_pin;
    handle->page_size = AT45DB_PAGE_SIZE_STANDARD;
    handle->is_45db041e = false;
    
    // Configure CS pin as output
    GPIO_InitTypeDef gpio_init;
    gpio_init.Pin = cs_pin;
    gpio_init.Mode = GPIO_MODE_OUTPUT;
    gpio_init.Ot = GPIO_OTYPE_PP;
    gpio_init.Speed = GPIO_SPEED_HIGH;
    gpio_init.Pull = GPIO_PULL_UP;
    gpio_init.AF = GPIO_AF0;
    GPIO_Init(cs_port, &gpio_init);
    
    // Deselect chip
    AT45DB_Deselect(handle);
    
    SPI_Enable(hspi);

    // Check device presence and read page size
    if (!AT45DB_CheckDevicePresence(handle)) {
        return false;
    }
    
    return true;
}

/**
 * @brief   De-initialize AT45DB driver
 */
void AT45DB_DeInit(AT45DB_HandleTypeDef *handle) {
    if (!handle) {
        return;
    }
    
    AT45DB_Deselect(handle);
}

/*============================================================================
 * Device Identification Functions
 *============================================================================*/

/**
 * @brief   Read JEDEC ID to check device presence
 */
bool AT45DB_ReadJEDECID(AT45DB_HandleTypeDef *handle, uint8_t *manufacturer_id, uint16_t *device_id) {
    uint8_t rx_data[3];
    
    AT45DB_Select(handle);
    SPI_SendData8(handle->hspi, AT45DB_CMD_JEDEC_ID);
    SPI_ReceiveBuffer8(handle->hspi, rx_data, 3);
    AT45DB_Deselect(handle);
    
    if (manufacturer_id) {
        *manufacturer_id = rx_data[0];
    }
    if (device_id) {
        *device_id = (rx_data[1] << 8) | rx_data[2];
    }
    
    return true;
}

/**
 * @brief   Check if device is present and read page size
 */
bool AT45DB_CheckDevicePresence(AT45DB_HandleTypeDef *handle) {
    uint8_t manufacturer_id;
    uint16_t device_id;
    
    if (!AT45DB_ReadJEDECID(handle, &manufacturer_id, &device_id)) {
        return false;
    }
    
    // Check manufacturer (should be 0x1F for Atmel)
    if (manufacturer_id != 0x1F) {
        return false;
    }
    
    // Check device ID
    switch (device_id) {
        case 0x2600:
            handle->is_45db041e = false;
            break;
        case 0x2601:
            handle->is_45db041e = true;
            break;
        default:
            return false;
    }
    
    // Read status register to get page size
    uint8_t status;
    if (AT45DB_ReadStatus(handle, &status)) {
        handle->page_size = (status & AT45DB_STATUS_PAGE_SIZE) ? 
                           AT45DB_PAGE_SIZE_BINARY : AT45DB_PAGE_SIZE_STANDARD;
    }
    
    return true;
}

/*============================================================================
 * Status Functions
 *============================================================================*/

/**
 * @brief   Read status register
 */
bool AT45DB_ReadStatus(AT45DB_HandleTypeDef *handle, uint8_t *status) {
    if (!handle || !status) {
        return false;
    }
    
    AT45DB_Select(handle);
    SPI_SendData8(handle->hspi, AT45DB_CMD_STATUS_READ);
    *status = SPI_ReceiveData8(handle->hspi);
    AT45DB_Deselect(handle);
    
    return true;
}

/**
 * @brief   Check if device is ready
 */
bool AT45DB_IsReady(AT45DB_HandleTypeDef *handle) {
    uint8_t status;
    
    if (!AT45DB_ReadStatus(handle, &status)) {
        return false;
    }
    
    return (status & AT45DB_STATUS_RDY_BUSY) != 0;
}

/**
 * @brief   Wait for device to become ready
 */
bool AT45DB_WaitReady(AT45DB_HandleTypeDef *handle, uint32_t timeout_ms) {
    uint32_t start_time = 0; // Implement your timer here
    
    while (!AT45DB_IsReady(handle)) {
        // Simple timeout implementation
        for (volatile int i = 0; i < 1000; i++);
        if (++start_time > timeout_ms) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief   Get current page size
 */
uint16_t AT45DB_GetPageSize(AT45DB_HandleTypeDef *handle) {
    return handle ? handle->page_size : 0;
}

/*============================================================================
 * Read Operations
 *============================================================================*/

/**
 * @brief   Continuous read from memory
 */
bool AT45DB_ReadContinuous(AT45DB_HandleTypeDef *handle, uint32_t address, uint8_t *buffer, uint32_t length) {
    uint8_t cmd_buffer[4];
    
    if (!handle || !buffer) {
        return false;
    }
    
    cmd_buffer[0] = AT45DB_CMD_CONTINUOUS_READ;
    AT45DB_EncodeAddress(address, &cmd_buffer[1], handle->page_size);
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    SPI_ReceiveBuffer8(handle->hspi, buffer, length);
    AT45DB_Deselect(handle);
    
    return true;
}

/**
 * @brief   Read from a specific page
 */
bool AT45DB_ReadPage(AT45DB_HandleTypeDef *handle, uint16_t page, uint16_t offset, uint8_t *buffer, uint32_t length) {
    uint32_t address = page * handle->page_size + offset;
    return AT45DB_ReadContinuous(handle, address, buffer, length);
}

/**
 * @brief   Read from Buffer 1
 */
bool AT45DB_ReadBuffer1(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *buffer, uint32_t length) {
    uint8_t cmd_buffer[4];
    
    if (!handle || !buffer) {
        return false;
    }
    
    cmd_buffer[0] = AT45DB_CMD_BUFFER1_READ;
    cmd_buffer[1] = (offset >> 8) & 0xFF;
    cmd_buffer[2] = offset & 0xFF;
    cmd_buffer[3] = 0x00; // Dummy byte
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    SPI_ReceiveBuffer8(handle->hspi, buffer, length);
    AT45DB_Deselect(handle);
    
    return true;
}

/**
 * @brief   Read from Buffer 2
 */
bool AT45DB_ReadBuffer2(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *buffer, uint32_t length) {
    uint8_t cmd_buffer[4];
    
    if (!handle || !buffer) {
        return false;
    }
    
    cmd_buffer[0] = AT45DB_CMD_BUFFER2_READ;
    cmd_buffer[1] = (offset >> 8) & 0xFF;
    cmd_buffer[2] = offset & 0xFF;
    cmd_buffer[3] = 0x00; // Dummy byte
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    SPI_ReceiveBuffer8(handle->hspi, buffer, length);
    AT45DB_Deselect(handle);
    
    return true;
}

/*============================================================================
 * Buffer Operations
 *============================================================================*/

/**
 * @brief   Write to Buffer 1
 */
bool AT45DB_WriteBuffer1(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *buffer, uint32_t length) {
    uint8_t cmd_buffer[4];
    
    if (!handle || !buffer) {
        return false;
    }
    
    cmd_buffer[0] = AT45DB_CMD_BUFFER1_WRITE;
    cmd_buffer[1] = (offset >> 8) & 0xFF;
    cmd_buffer[2] = offset & 0xFF;
    cmd_buffer[3] = 0x00; // Address bits 23-16 (unused)
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    SPI_SendBuffer8(handle->hspi, buffer, length);
    AT45DB_Deselect(handle);
    
    return true;
}

/**
 * @brief   Write to Buffer 2
 */
bool AT45DB_WriteBuffer2(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *buffer, uint32_t length) {
    uint8_t cmd_buffer[4];
    
    if (!handle || !buffer) {
        return false;
    }
    
    cmd_buffer[0] = AT45DB_CMD_BUFFER2_WRITE;
    cmd_buffer[1] = (offset >> 8) & 0xFF;
    cmd_buffer[2] = offset & 0xFF;
    cmd_buffer[3] = 0x00; // Address bits 23-16 (unused)
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    SPI_SendBuffer8(handle->hspi, buffer, length);
    AT45DB_Deselect(handle);
    
    return true;
}

/**
 * @brief   Program through Buffer 1 with built-in erase
 */
bool AT45DB_ProgramBuffer1(AT45DB_HandleTypeDef *handle, uint16_t page, uint16_t offset, uint8_t *buffer, uint32_t length) {
    uint8_t cmd_buffer[4];
    
    if (!handle || !buffer) {
        return false;
    }
    
    cmd_buffer[0] = AT45DB_CMD_BUFFER1_PROGRAM;
    uint32_t address = page * handle->page_size + offset;
    AT45DB_EncodeAddress(address, &cmd_buffer[1], handle->page_size);
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    SPI_SendBuffer8(handle->hspi, buffer, length);
    AT45DB_Deselect(handle);
    
    return AT45DB_WaitReady(handle, 1000);
}

/**
 * @brief   Program through Buffer 2 with built-in erase
 */
bool AT45DB_ProgramBuffer2(AT45DB_HandleTypeDef *handle, uint16_t page, uint16_t offset, uint8_t *buffer, uint32_t length) {
    uint8_t cmd_buffer[4];
    
    if (!handle || !buffer) {
        return false;
    }
    
    cmd_buffer[0] = AT45DB_CMD_BUFFER2_PROGRAM;
    uint32_t address = page * handle->page_size + offset;
    AT45DB_EncodeAddress(address, &cmd_buffer[1], handle->page_size);
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    SPI_SendBuffer8(handle->hspi, buffer, length);
    AT45DB_Deselect(handle);
    
    return AT45DB_WaitReady(handle, 1000);
}

/**
 * @brief   Transfer Buffer 1 to main memory page
 */
bool AT45DB_Buffer1ToPage(AT45DB_HandleTypeDef *handle, uint16_t page) {
    uint8_t cmd_buffer[4];
    
    if (!handle) {
        return false;
    }
    
    cmd_buffer[0] = AT45DB_CMD_BUFFER1_TO_PAGE;
    uint32_t address = page * handle->page_size;
    AT45DB_EncodeAddress(address, &cmd_buffer[1], handle->page_size);
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    AT45DB_Deselect(handle);
    
    return AT45DB_WaitReady(handle, 1000);
}

/**
 * @brief   Transfer Buffer 2 to main memory page
 */
bool AT45DB_Buffer2ToPage(AT45DB_HandleTypeDef *handle, uint16_t page) {
    uint8_t cmd_buffer[4];
    
    if (!handle) {
        return false;
    }
    
    cmd_buffer[0] = AT45DB_CMD_BUFFER2_TO_PAGE;
    uint32_t address = page * handle->page_size;
    AT45DB_EncodeAddress(address, &cmd_buffer[1], handle->page_size);
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    AT45DB_Deselect(handle);
    
    return AT45DB_WaitReady(handle, 1000);
}

/*============================================================================
 * Erase Operations
 *============================================================================*/

/**
 * @brief   Erase a single page
 */
bool AT45DB_ErasePage(AT45DB_HandleTypeDef *handle, uint16_t page) {
    uint8_t cmd_buffer[4];
    
    if (!handle) {
        return false;
    }
    
    cmd_buffer[0] = AT45DB_CMD_PAGE_ERASE;
    uint32_t address = page * handle->page_size;
    AT45DB_EncodeAddress(address, &cmd_buffer[1], handle->page_size);
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    AT45DB_Deselect(handle);
    
    return AT45DB_WaitReady(handle, 2000);
}

/**
 * @brief   Erase an 8-page block
 */
bool AT45DB_EraseBlock(AT45DB_HandleTypeDef *handle, uint16_t block) {
    uint8_t cmd_buffer[4];
    
    if (!handle) {
        return false;
    }
    
    cmd_buffer[0] = AT45DB_CMD_BLOCK_ERASE;
    uint32_t address = block * 8 * handle->page_size;
    AT45DB_EncodeAddress(address, &cmd_buffer[1], handle->page_size);
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    AT45DB_Deselect(handle);
    
    return AT45DB_WaitReady(handle, 4000);
}

/**
 * @brief   Erase a 64KB sector
 */
bool AT45DB_EraseSector(AT45DB_HandleTypeDef *handle, uint16_t sector) {
    uint8_t cmd_buffer[4];
    
    if (!handle) {
        return false;
    }
    
    cmd_buffer[0] = AT45DB_CMD_SECTOR_ERASE;
    uint32_t address = sector * 64 * 1024;
    AT45DB_EncodeAddress(address, &cmd_buffer[1], handle->page_size);
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    AT45DB_Deselect(handle);
    
    return AT45DB_WaitReady(handle, 30000);
}

/**
 * @brief   Erase entire chip (avoid on AT45DB041D)
 */
bool AT45DB_EraseChip(AT45DB_HandleTypeDef *handle) {
    uint8_t cmd_buffer[] = {AT45DB_CMD_CHIP_ERASE, AT45DB_CMD_CHIP_ERASE2, 
                           AT45DB_CMD_CHIP_ERASE3, AT45DB_CMD_CHIP_ERASE4};
    
    if (!handle) {
        return false;
    }
    
    // Avoid chip erase on AT45DB041D
    if (!handle->is_45db041e) {
        return false;
    }
    
    AT45DB_Select(handle);
    SPI_SendBuffer8(handle->hspi, cmd_buffer, 4);
    AT45DB_Deselect(handle);
    
    return AT45DB_WaitReady(handle, 60000);
}

/*============================================================================
 * Power Management
 *============================================================================*/

/**
 * @brief   Enter deep power-down mode
 */
bool AT45DB_EnterDeepPowerDown(AT45DB_HandleTypeDef *handle) {
    if (!handle) {
        return false;
    }
    
    AT45DB_Select(handle);
    SPI_SendData8(handle->hspi, AT45DB_CMD_DEEP_POWER_DOWN);
    AT45DB_Deselect(handle);
    
    return true;
}

/**
 * @brief   Resume from deep power-down mode
 */
bool AT45DB_ResumeFromDeepPowerDown(AT45DB_HandleTypeDef *handle) {
    if (!handle) {
        return false;
    }
    
    AT45DB_Select(handle);
    SPI_SendData8(handle->hspi, AT45DB_CMD_RESUME);
    AT45DB_Deselect(handle);
    
    // Wait for device to resume
    for (volatile int i = 0; i < 10000; i++);
    
    return true;
}

/**
 * @brief   Enter ultra-deep power-down mode
 */
bool AT45DB_EnterUltraDeepPowerDown(AT45DB_HandleTypeDef *handle) {
    if (!handle || !handle->is_45db041e) {
        return false;
    }
    
    AT45DB_Select(handle);
    SPI_SendData8(handle->hspi, AT45DB_CMD_ULTRA_DEEP_POWER_DOWN);
    AT45DB_Deselect(handle);
    
    return true;
}

/**
 * @brief   Exit ultra-deep power-down mode
 */
bool AT45DB_ExitUltraDeepPowerDown(AT45DB_HandleTypeDef *handle) {
    if (!handle || !handle->is_45db041e) {
        return false;
    }
    
    // Pulse CS low then high
    AT45DB_Select(handle);
    for (volatile int i = 0; i < 1000; i++);
    AT45DB_Deselect(handle);
    
    // Wait for device to resume
    for (volatile int i = 0; i < 10000; i++);
    
    return true;
}
