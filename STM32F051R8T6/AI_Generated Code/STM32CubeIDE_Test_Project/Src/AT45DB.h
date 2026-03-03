/**
 * @file    AT45DB.h
 * @brief   AT45DB DataFlash Driver Header
 * @details Driver for AT45DB041D/AT45DB041E DataFlash memory
 *          via SPI interface
 */

#ifndef AT45DB_H
#define AT45DB_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*============================================================================
 * AT45DB Device Definitions
 *============================================================================*/

/* AT45DB Page Sizes */
#define AT45DB_PAGE_SIZE_264    264     /* Standard DataFlash page size */
#define AT45DB_PAGE_SIZE_256    256     /* Binary page size */
#define AT45DB_PAGES            2048    /* Total number of pages */
#define AT45DB_BUFFER_SIZE      264     /* SRAM buffer size */

/* AT45DB JEDEC ID */
#define AT45DB_MANUFACTURER_ID  0x1F     /* Atmel */
#define AT45DB_DEVICE_ID_041D   0x2400   /* AT45DB041D */
#define AT45DB_DEVICE_ID_041E   0x2600   /* AT45DB041E */

/*============================================================================
 * AT45DB Opcodes
 *============================================================================*/

/* Status Register */
#define AT45DB_OPCODE_STATUS_READ    0xD7    /* Read Status Register */

/* JEDEC ID */
#define AT45DB_OPCODE_JEDEC_READ      0x9F    /* Read JEDEC ID */

/* Memory Read Operations */
#define AT45DB_OPCODE_CONTINUOUS_READ_HS   0x0B    /* High-speed continuous read (with dummy byte) */
#define AT45DB_OPCODE_CONTINUOUS_READ_LS   0x03    /* Low-frequency continuous read */
#define AT45DB_OPCODE_PAGE_READ         0xD2    /* Main memory page read */

/* Buffer Read Operations */
#define AT45DB_OPCODE_BUFFER1_READ_HF   0xD4    /* Buffer 1 read (high frequency) */
#define AT45DB_OPCODE_BUFFER2_READ_HF   0xD6    /* Buffer 2 read (high frequency) */

/* Buffer Write Operations */
#define AT45DB_OPCODE_BUFFER1_WRITE    0x84    /* Buffer 1 write */
#define AT45DB_OPCODE_BUFFER2_WRITE    0x87    /* Buffer 2 write */

/* Program Operations */
#define AT45DB_OPCODE_PROGRAM_THROUGH_BUF1_ERASE   0x82    /* Main memory program through buffer 1 (with erase) */
#define AT45DB_OPCODE_PROGRAM_THROUGH_BUF2_ERASE   0x85    /* Main memory program through buffer 2 (with erase) */
#define AT45DB_OPCODE_BUF1_TO_PAGE_ERASE            0x83    /* Buffer 1 to main memory (with erase) */
#define AT45DB_OPCODE_BUF2_TO_PAGE_ERASE            0x86    /* Buffer 2 to main memory (with erase) */
#define AT45DB_OPCODE_BUF1_TO_PAGE_NO_ERASE        0x88    /* Buffer 1 to main memory (without erase) */
#define AT45DB_OPCODE_BUF2_TO_PAGE_NO_ERASE        0x89    /* Buffer 2 to main memory (without erase) */

/* Erase Operations */
#define AT45DB_OPCODE_PAGE_ERASE         0x81    /* Page erase */
#define AT45DB_OPCODE_BLOCK_ERASE        0x50    /* Block erase (8 pages) */
#define AT45DB_OPCODE_SECTOR_ERASE       0x7C    /* Sector erase */
#define AT45DB_OPCODE_CHIP_ERASE1        0xC7    /* Chip erase - byte 1 */
#define AT45DB_OPCODE_CHIP_ERASE2        0x94    /* Chip erase - byte 2 */
#define AT45DB_OPCODE_CHIP_ERASE3        0x80    /* Chip erase - byte 3 */
#define AT45DB_OPCODE_CHIP_ERASE4        0x9A    /* Chip erase - byte 4 */

/* Power Management */
#define AT45DB_OPCODE_DEEP_POWER_DOWN   0xB9    /* Enter deep power-down */
#define AT45DB_OPCODE_RESUME             0xAB    /* Resume from deep power-down */

/*============================================================================
 * Status Register Bit Definitions
 *============================================================================*/
#define AT45DB_STATUS_RDY_BUSY       0x80    /* Ready/Busy flag (1=ready, 0=busy) */
#define AT45DB_STATUS_COMP           0x40    /* Compare result (1=match, 0=differ) */
#define AT45DB_STATUS_PROTECT        0x02    /* Sector protection enabled */
#define AT45DB_STATUS_PAGE_SIZE      0x01    /* Page size (1=256, 0=264) */

/*============================================================================
 * AT45DB Handle Structure
 *============================================================================*/
typedef struct {
    void *SPIx;                  /* SPI peripheral (SPI1 or SPI2) */
    void *CS_Port;              /* Chip select GPIO port */
    uint16_t CS_Pin;            /* Chip select GPIO pin */
    uint16_t PageSize;          /* Current page size (264 or 256) */
    bool DeviceFound;           /* Device detection flag */
    uint16_t DeviceID;          /* Device ID */
    uint8_t ManufacturerID;     /* Manufacturer ID */
} AT45DB_HandleTypeDef;

/*============================================================================
 * AT45DB Driver API
 *============================================================================*/

/**
 * @brief  Initialize AT45DB DataFlash
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @retval true if initialization successful, false otherwise
 */
bool AT45DB_Init(AT45DB_HandleTypeDef *handle);

/**
 * @brief  Check if AT45DB device is present
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @retval true if device found, false otherwise
 */
bool AT45DB_CheckDevicePresence(AT45DB_HandleTypeDef *handle);

/**
 * @brief  Read JEDEC ID from AT45DB
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  manufacturer_id: pointer to store manufacturer ID
 * @param  device_id: pointer to store device ID
 * @retval true if read successful, false otherwise
 */
bool AT45DB_ReadJEDECID(AT45DB_HandleTypeDef *handle, uint8_t *manufacturer_id, uint16_t *device_id);

/**
 * @brief  Read status register
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  status: pointer to store status value
 * @retval true if read successful, false otherwise
 */
bool AT45DB_ReadStatus(AT45DB_HandleTypeDef *handle, uint8_t *status);

/**
 * @brief  Wait for AT45DB to be ready (poll RDY/BUSY flag)
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  timeout: maximum wait time in milliseconds
 * @retval true if device ready, false if timeout
 */
bool AT45DB_WaitReady(AT45DB_HandleTypeDef *handle, uint32_t timeout);

/**
 * @brief  Read data from continuous memory array
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  address: starting address (byte address)
 * @param  buffer: pointer to data buffer
 * @param  length: number of bytes to read
 * @retval true if read successful, false otherwise
 */
bool AT45DB_ContinuousRead(AT45DB_HandleTypeDef *handle, uint32_t address, uint8_t *buffer, uint32_t length);

/**
 * @brief  Read a page from main memory
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  page_number: page number (0 to 2047)
 * @param  buffer: pointer to data buffer (must be at least page size)
 * @retval true if read successful, false otherwise
 */
bool AT45DB_PageRead(AT45DB_HandleTypeDef *handle, uint16_t page_number, uint8_t *buffer);

/**
 * @brief  Write data to buffer 1
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  offset: starting offset in buffer (0 to page_size-1)
 * @param  data: pointer to data buffer
 * @param  length: number of bytes to write
 * @retval true if write successful, false otherwise
 */
bool AT45DB_Buffer1Write(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *data, uint16_t length);

/**
 * @brief  Write data to buffer 2
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  offset: starting offset in buffer (0 to page_size-1)
 * @param  data: pointer to data buffer
 * @param  length: number of bytes to write
 * @retval true if write successful, false otherwise
 */
bool AT45DB_Buffer2Write(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *data, uint16_t length);

/**
 * @brief  Program buffer to main memory page with built-in erase
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  page_number: target page number (0 to 2047)
 * @param  buffer: source buffer (use NULL to program existing buffer content)
 * @param  length: number of bytes to program (0 = program whole page)
 * @param  use_buffer1: true for buffer 1, false for buffer 2
 * @retval true if program successful, false otherwise
 */
bool AT45DB_ProgramPage(AT45DB_HandleTypeDef *handle, uint16_t page_number, uint8_t *buffer, uint16_t length, bool use_buffer1);

/**
 * @brief  Program main memory page through buffer (one-step operation)
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  page_number: target page number (0 to 2047)
 * @param  offset: starting offset in page
 * @param  data: pointer to data buffer
 * @param  length: number of bytes to program
 * @param  use_buffer1: true for buffer 1, false for buffer 2
 * @retval true if program successful, false otherwise
 */
bool AT45DB_ProgramThroughBuffer(AT45DB_HandleTypeDef *handle, uint16_t page_number, 
                                   uint16_t offset, uint8_t *data, uint16_t length, bool use_buffer1);

/**
 * @brief  Erase a page
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  page_number: page number to erase (0 to 2047)
 * @retval true if erase started successfully, false otherwise
 */
bool AT45DB_PageErase(AT45DB_HandleTypeDef *handle, uint16_t page_number);

/**
 * @brief  Erase a block (8 pages)
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  block_number: block number (0 to 255)
 * @retval true if erase started successfully, false otherwise
 */
bool AT45DB_BlockErase(AT45DB_HandleTypeDef *handle, uint16_t block_number);

/**
 * @brief  Enter deep power-down mode
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @retval true if command successful, false otherwise
 */
bool AT45DB_EnterDeepPowerDown(AT45DB_HandleTypeDef *handle);

/**
 * @brief  Resume from deep power-down mode
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @retval true if command successful, false otherwise
 */
bool AT45DB_ResumeFromDeepPowerDown(AT45DB_HandleTypeDef *handle);

/**
 * @brief  Get page size
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @retval page size in bytes
 */
uint16_t AT45DB_GetPageSize(AT45DB_HandleTypeDef *handle);

#endif /* AT45DB_H */
