/**
 * @file    AT45DB.h
 * @brief   AT45DB041D/AT45DB041E DataFlash Driver for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-02-28
 */

#ifndef AT45DB_H
#define AT45DB_H

#include <stdint.h>
#include <stdbool.h>
#include "../../SPI/Code/spiF051.h"
#include "../../GPIO_AI/gpio.h"

/*============================================================================
 * Device Configuration
 *============================================================================*/
#define AT45DB_PAGE_SIZE_STANDARD  264  // Default page size
#define AT45DB_PAGE_SIZE_BINARY    256  // 256-byte page size
#define AT45DB_TOTAL_PAGES         2048 // Total number of pages
#define AT45DB_BUFFER_SIZE_STANDARD  264
#define AT45DB_BUFFER_SIZE_BINARY    256

/*============================================================================
 * Command Opcodes
 *============================================================================*/
#define AT45DB_CMD_JEDEC_ID        0x9F
#define AT45DB_CMD_STATUS_READ     0xD7
#define AT45DB_CMD_CONTINUOUS_READ 0x0B
#define AT45DB_CMD_PAGE_READ       0xD2
#define AT45DB_CMD_BUFFER1_WRITE   0x84
#define AT45DB_CMD_BUFFER2_WRITE   0x87
#define AT45DB_CMD_BUFFER1_READ    0xD4
#define AT45DB_CMD_BUFFER2_READ    0xD6
#define AT45DB_CMD_BUFFER1_PROGRAM 0x82
#define AT45DB_CMD_BUFFER2_PROGRAM 0x85
#define AT45DB_CMD_BUFFER1_TO_PAGE 0x83
#define AT45DB_CMD_BUFFER2_TO_PAGE 0x86
#define AT45DB_CMD_PAGE_ERASE      0x81
#define AT45DB_CMD_BLOCK_ERASE     0x50
#define AT45DB_CMD_SECTOR_ERASE    0x7C
#define AT45DB_CMD_CHIP_ERASE      0xC7
#define AT45DB_CMD_CHIP_ERASE2     0x94
#define AT45DB_CMD_CHIP_ERASE3     0x80
#define AT45DB_CMD_CHIP_ERASE4     0x9A
#define AT45DB_CMD_DEEP_POWER_DOWN 0xB9
#define AT45DB_CMD_RESUME          0xAB
#define AT45DB_CMD_ULTRA_DEEP_POWER_DOWN 0x79

/*============================================================================
 * Status Register Bits
 *============================================================================*/
#define AT45DB_STATUS_RDY_BUSY     (1 << 7)
#define AT45DB_STATUS_COMPARE      (1 << 6)
#define AT45DB_STATUS_PROTECT      (1 << 1)
#define AT45DB_STATUS_PAGE_SIZE    (1 << 0)
#define AT45DB_STATUS_EPE          (1 << 5)
#define AT45DB_STATUS_SLE          (1 << 4)

/*============================================================================
 * Device Handle Structure
 *============================================================================*/
typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *cs_port;
    uint32_t cs_pin;
    uint16_t page_size;
    bool is_45db041e;
} AT45DB_HandleTypeDef;

/*============================================================================
 * Driver API Prototypes
 *============================================================================*/

/* Initialization and Configuration */
bool AT45DB_Init(AT45DB_HandleTypeDef *handle, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint32_t cs_pin);
void AT45DB_DeInit(AT45DB_HandleTypeDef *handle);

/* Device Identification */
bool AT45DB_ReadJEDECID(AT45DB_HandleTypeDef *handle, uint8_t *manufacturer_id, uint16_t *device_id);
bool AT45DB_CheckDevicePresence(AT45DB_HandleTypeDef *handle);

/* Status Register */
bool AT45DB_ReadStatus(AT45DB_HandleTypeDef *handle, uint8_t *status);
bool AT45DB_IsReady(AT45DB_HandleTypeDef *handle);
bool AT45DB_WaitReady(AT45DB_HandleTypeDef *handle, uint32_t timeout_ms);
uint16_t AT45DB_GetPageSize(AT45DB_HandleTypeDef *handle);

/* Read Operations */
bool AT45DB_ReadContinuous(AT45DB_HandleTypeDef *handle, uint32_t address, uint8_t *buffer, uint32_t length);
bool AT45DB_ReadPage(AT45DB_HandleTypeDef *handle, uint16_t page, uint16_t offset, uint8_t *buffer, uint32_t length);
bool AT45DB_ReadBuffer1(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *buffer, uint32_t length);
bool AT45DB_ReadBuffer2(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *buffer, uint32_t length);

/* Buffer Operations */
bool AT45DB_WriteBuffer1(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *buffer, uint32_t length);
bool AT45DB_WriteBuffer2(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *buffer, uint32_t length);
bool AT45DB_ProgramBuffer1(AT45DB_HandleTypeDef *handle, uint16_t page, uint16_t offset, uint8_t *buffer, uint32_t length);
bool AT45DB_ProgramBuffer2(AT45DB_HandleTypeDef *handle, uint16_t page, uint16_t offset, uint8_t *buffer, uint32_t length);
bool AT45DB_Buffer1ToPage(AT45DB_HandleTypeDef *handle, uint16_t page);
bool AT45DB_Buffer2ToPage(AT45DB_HandleTypeDef *handle, uint16_t page);

/* Erase Operations */
bool AT45DB_ErasePage(AT45DB_HandleTypeDef *handle, uint16_t page);
bool AT45DB_EraseBlock(AT45DB_HandleTypeDef *handle, uint16_t block);
bool AT45DB_EraseSector(AT45DB_HandleTypeDef *handle, uint16_t sector);
bool AT45DB_EraseChip(AT45DB_HandleTypeDef *handle);

/* Power Management */
bool AT45DB_EnterDeepPowerDown(AT45DB_HandleTypeDef *handle);
bool AT45DB_ResumeFromDeepPowerDown(AT45DB_HandleTypeDef *handle);
bool AT45DB_EnterUltraDeepPowerDown(AT45DB_HandleTypeDef *handle);
bool AT45DB_ExitUltraDeepPowerDown(AT45DB_HandleTypeDef *handle);

/* Internal Helper Functions */
void AT45DB_Select(AT45DB_HandleTypeDef *handle);
void AT45DB_Deselect(AT45DB_HandleTypeDef *handle);
void AT45DB_EncodeAddress(uint32_t address, uint8_t *addr_bytes, uint16_t page_size);

#endif /* AT45DB_H */
