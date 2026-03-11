/**
 * @file    AT45DB.c
 * @brief   AT45DB DataFlash Driver Implementation
 * @details Driver for AT45DB041D/AT45DB041E DataFlash memory
 *          via SPI interface
 */

#include "AT45DB.h"
#include "spiF051.h"
#include "gpio.h"
#include "rcc.h"

/*============================================================================
 * Private Macros
 *============================================================================*/

/* CS Pin control macros - assuming CS is on GPIO port B, pin 12 (common for SPI1) */
#define AT45DB_CS_LOW(handle)     GPIO_ResetPin((GPIO_TypeDef*)handle->CS_Port, handle->CS_Pin)
#define AT45DB_CS_HIGH(handle)     GPIO_SetPin((GPIO_TypeDef*)handle->CS_Port, handle->CS_Pin)

/* Timeout for status polling */
#define AT45DB_TIMEOUT_MAX        50000UL

/*============================================================================
 * Private Function Prototypes
 *============================================================================*/

static void AT45DB_CSelect(AT45DB_HandleTypeDef *handle);
static void AT45DB_CDeselect(AT45DB_HandleTypeDef *handle);
static void AT45DB_EncodeAddress(uint32_t address, uint8_t *addr_bytes, uint16_t page_size);

/*============================================================================
 * Private Functions
 *============================================================================*/

/**
 * @brief  Assert chip select
 */
static void AT45DB_CSelect(AT45DB_HandleTypeDef *handle) {
    AT45DB_CS_LOW(handle);
}

/**
 * @brief  De-assert chip select
 */
static void AT45DB_CDeselect(AT45DB_HandleTypeDef *handle) {
    AT45DB_CS_HIGH(handle);
}

/**
 * @brief  Encode address for AT45DB commands
 * @param  address: linear byte address
 * @param  addr_bytes: 3-byte buffer to store encoded address
 * @param  page_size: current page size (264 or 256)
 */
static void AT45DB_EncodeAddress(uint32_t address, uint8_t *addr_bytes, uint16_t page_size) {
    uint16_t page_addr;
    uint16_t byte_addr;
    
    if (page_size == AT45DB_PAGE_SIZE_256) {
        /* Binary page size (256 bytes) */
        /* A18..A8 = page address (11 bits) */
        /* A7..A0 = byte address within page (8 bits) */
        page_addr = (uint16_t)((address >> 8) & 0x07FF);
        byte_addr = (uint16_t)(address & 0xFF);
    } else {
        /* Standard page size (264 bytes) */
        /* PA10..PA0 = page address (11 bits) */
        /* BA8..BA0 = byte address within page (9 bits) */
        page_addr = (uint16_t)((address / page_size) & 0x07FF);
        byte_addr = (uint16_t)(address % page_size);
    }
    
    /* Encode as: [PageAddr High][PageAddr Low & ByteAddr High][ByteAddr Low] */
    /* For 264-byte mode: 9 bits for byte address (BA8-BA0) - bits 7-3 in byte 1, bits 2-0 in byte 2 */
    /* For 256-byte mode: 8 bits for byte address (A7-A0) - bits 7-3 in byte 1, bits 2-0 in byte 2 */
    if (page_size == AT45DB_PAGE_SIZE_256) {
        /* 256-byte mode: 8-bit byte address */
        addr_bytes[0] = (uint8_t)((page_addr >> 3) & 0xFF);
        addr_bytes[1] = (uint8_t)(((page_addr & 0x07) << 5) | ((byte_addr >> 3) & 0x1F));
        addr_bytes[2] = (uint8_t)((byte_addr & 0x1F) << 3);
    } else {
        /* 264-byte mode: 9-bit byte address */
        addr_bytes[0] = (uint8_t)((page_addr >> 3) & 0xFF);
        addr_bytes[1] = (uint8_t)(((page_addr & 0x07) << 5) | ((byte_addr >> 3) & 0x1F));
        addr_bytes[2] = (uint8_t)((byte_addr & 0x1F) << 3);
    }
}

/*============================================================================
 * Public Functions - Initialization
 *============================================================================*/

/**
 * @brief  Initialize AT45DB DataFlash
 */
bool AT45DB_Init(AT45DB_HandleTypeDef *handle) {
    uint8_t status;
    
    if (handle == NULL) {
        return false;
    }
    
    /* Default to 264-byte page size (factory default for AT45DB041D/E) */
    handle->PageSize = AT45DB_PAGE_SIZE_264;
    handle->DeviceFound = false;
    
    /* Check device presence */
    if (!AT45DB_CheckDevicePresence(handle)) {
        return false;
    }
    
    /* Read status to get actual page size configuration */
    if (AT45DB_ReadStatus(handle, &status)) {
        /* Check the page size bit in status register */
        /* Bit 0: 0 = 264-byte pages, 1 = 256-byte pages */
        if (status & AT45DB_STATUS_PAGE_SIZE) {
            handle->PageSize = AT45DB_PAGE_SIZE_256;
        } else {
            handle->PageSize = AT45DB_PAGE_SIZE_264;
        }
    }
    
    /* Wait for device to be ready */
    return AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX);
}

/**
 * @brief  Check if AT45DB device is present
 */
bool AT45DB_CheckDevicePresence(AT45DB_HandleTypeDef *handle) {
    uint8_t manufacturer_id;
    uint16_t device_id;
    
    if (!AT45DB_ReadJEDECID(handle, &manufacturer_id, &device_id)) {
        handle->DeviceFound = false;
        return false;
    }
    
    handle->ManufacturerID = manufacturer_id;
    handle->DeviceID = device_id;
    
    /* Check for valid manufacturer ID */
    if (manufacturer_id != AT45DB_MANUFACTURER_ID) {
        handle->DeviceFound = false;
        return false;
    }
    
    /* Check for supported device IDs */
    if ((device_id == AT45DB_DEVICE_ID_041D) || 
        (device_id == AT45DB_DEVICE_ID_041E) ||
        (device_id == 0x2401) || (device_id == 0x2601)) {
        handle->DeviceFound = true;
        return true;
    }
    
    handle->DeviceFound = false;
    return false;
}

/**
 * @brief  Read JEDEC ID from AT45DB
 */
bool AT45DB_ReadJEDECID(AT45DB_HandleTypeDef *handle, uint8_t *manufacturer_id, uint16_t *device_id) {
    uint8_t jedec_data[4];
    
    if (handle == NULL || manufacturer_id == NULL || device_id == NULL) {
        return false;
    }
    
    /* Assert CS */
    AT45DB_CSelect(handle);
    
    /* Send JEDEC ID read command */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, AT45DB_OPCODE_JEDEC_READ);

    /* Read 4 bytes: Manufacturer ID + Device ID (2 bytes) + Additional info */
    for (int i = 0; i < 4; i++) {
        jedec_data[i] = SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, 0xFF);
    }
    
    /* De-assert CS */
    AT45DB_CDeselect(handle);
    
    /* Extract IDs - JEDEC format: 
     * Byte 0: Manufacturer ID (0x1F for Atmel)
     * Byte 1: Device ID (first byte, e.g., 0x24 for AT45DB041D)
     * Byte 2: Device ID (second byte, e.g., 0x00)
     */
    *manufacturer_id = jedec_data[0];
    *device_id = ((uint16_t)jedec_data[1] << 8) | jedec_data[2];
    
    return true;
}

/**
 * @brief  Read status register
 */
bool AT45DB_ReadStatus(AT45DB_HandleTypeDef *handle, uint8_t *status) {
    uint8_t status_val;
    
    if (handle == NULL || status == NULL) {
        return false;
    }
    
    /* Assert CS */
    AT45DB_CSelect(handle);
    
    /* Send status read command */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, AT45DB_OPCODE_STATUS_READ);
    
    /* Read status byte (send dummy byte) */
    status_val = SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, 0xFF);
    
    /* De-assert CS */
    AT45DB_CDeselect(handle);
    
    *status = status_val;

    if((status_val == 0x00) || (status_val == 0xFF))
    	return false;

    return true;
}

/**
 * @brief  Wait for AT45DB to be ready (poll RDY/BUSY flag)
 */
bool AT45DB_WaitReady(AT45DB_HandleTypeDef *handle, uint32_t timeout) {
    uint8_t status;
    uint32_t count = 0;
    
    if (handle == NULL) {
        return false;
    }
    
    /* Poll status register until ready */
    do {
        if (!AT45DB_ReadStatus(handle, &status)) {
            return false;
        }
        count++;
        if (count > timeout) {
            return false;
        }
    } while ((status & AT45DB_STATUS_RDY_BUSY) == 0);
    
    return true;
}

/*============================================================================
 * Public Functions - Read Operations
 *============================================================================*/

/**
 * @brief  Read data from continuous memory array
 */
bool AT45DB_ContinuousRead(AT45DB_HandleTypeDef *handle, uint32_t address, uint8_t *buffer, uint32_t length) {
    uint8_t addr_bytes[3];
    uint32_t i;
    
    if (handle == NULL || buffer == NULL || length == 0) {
        return false;
    }
    
    /* Wait for device to be ready */
    if (!AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX)) {
        return false;
    }
    
    /* Encode address */
    AT45DB_EncodeAddress(address, addr_bytes, handle->PageSize);
    
    /* Assert CS */
    AT45DB_CSelect(handle);
    
    /* Send continuous read command (high frequency) */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, AT45DB_OPCODE_CONTINUOUS_READ_HS);
    
    /* Send 3 address bytes */
    SPI_TransmitReceiveBuffer((SPI_TypeDef*)handle->SPIx, addr_bytes, 0,  3);
    
    /* Send dummy byte (required for high-frequency read) */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, 0xFF);
    
    /* Read data bytes */
    for (i = 0; i < length; i++) {
        buffer[i] = SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, 0xFF);
    }
    
    /* De-assert CS */
    AT45DB_CDeselect(handle);
    
    return true;
}

/**
 * @brief  Read a page from main memory
 */
bool AT45DB_PageRead(AT45DB_HandleTypeDef *handle, uint16_t page_number, uint8_t *buffer) {
    uint8_t addr_bytes[3];
    uint16_t i;
    uint32_t address;
    
    if (handle == NULL || buffer == NULL) {
        return false;
    }
    
    /* Validate page number */
    if (page_number >= AT45DB_PAGES) {
        return false;
    }
    
    /* Wait for device to be ready */
    if (!AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX)) {
        return false;
    }
    
    /* Calculate address */
    address = (uint32_t)page_number * handle->PageSize;
    AT45DB_EncodeAddress(address, addr_bytes, handle->PageSize);
    
    /* Assert CS */
    AT45DB_CSelect(handle);
    
    /* Send page read command */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, AT45DB_OPCODE_PAGE_READ);
    
    /* Send 3 address bytes */
    SPI_TransmitReceiveBuffer((SPI_TypeDef*)handle->SPIx, addr_bytes, 0, 3);
    
    /* Send 4 dummy bytes */
    for (i = 0; i < 4; i++) {
        SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, 0xFF);
    }
    
    /* Read page data */
    for (i = 0; i < handle->PageSize; i++) {
        buffer[i] = SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, 0xFF);
    }
    
    /* De-assert CS */
    AT45DB_CDeselect(handle);
    
    return true;
}


/**
 * @brief Constructs the 4-byte command for Continuous Array Read (Opcode 03h).
 * * @param page_addr   The target page index (0 to 2047).
 * @param byte_offset The starting byte within the page.
 * @param is_binary   True for 256B page size, false for 264B.
 * @param cmd_buffer  4-byte array to store the sequence.
 */
void make_03h_read_cmd(uint16_t page_addr, uint16_t byte_offset, uint16_t page_size, uint8_t *cmd_buffer)
{
    cmd_buffer[0] = 0x03; // Opcode for Continuous Array Read (Low Frequency)

    if (page_size == AT45DB_PAGE_SIZE_256) {
        // 256B: Address bits A18-A8 (Page) and A7-A0 (Byte)
        // [3 dummy][A18-A16] | [A15-A8] | [A7-A0]
        cmd_buffer[1] = (uint8_t)((page_addr >> 8) & 0x07);
        cmd_buffer[2] = (uint8_t)(page_addr & 0xFF);
        cmd_buffer[3] = (uint8_t)(byte_offset & 0xFF);
    } else {
        // 264B: Address bits PA10-PA0 (Page) and BA8-BA0 (Byte)
        // [4 dummy][PA10-PA7] | [PA6-PA0][BA8] | [BA7-BA0]
        cmd_buffer[1] = (uint8_t)((page_addr >> 7) & 0x0F);
        cmd_buffer[2] = (uint8_t)(((page_addr << 1) & 0xFE) | ((byte_offset >> 8) & 0x01));
        cmd_buffer[3] = (uint8_t)(byte_offset & 0xFF);
    }
}

/**
 * @brief API to read data from a specific page and offset.
 * * @param page_addr   Page index to read from.
 * @param byte_offset Starting byte offset within the page.
 * @param data_out    Buffer to store retrieved data.
 * @param len         Number of bytes to read.
 * @param is_binary   Current chip configuration (256B or 264B).
 */
bool flash_read_page(AT45DB_HandleTypeDef *handle, uint16_t page_number, uint8_t *buffer)
{
	uint8_t addr_bytes[4];
	uint16_t i;

	if (handle == NULL || buffer == NULL) {
		return false;
	}

	/* Validate page number */
	if (page_number >= AT45DB_PAGES) {
		return false;
	}

if(page_number == 158)
{
	page_number = 158;
}

	/* Wait for device to be ready */
	if (!AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX)) {
		return false;
	}

	//----------------

    make_03h_read_cmd(page_number, 0, handle->PageSize, addr_bytes);

    /* Assert CS */
	AT45DB_CSelect(handle);

	/* Send page read command */
	//SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, AT45DB_OPCODE_PAGE_READ);

	/* Send 3 address bytes */
	SPI_TransmitReceiveBuffer((SPI_TypeDef*)handle->SPIx, addr_bytes, 0, 4);

//	/* Send 4 dummy bytes */
//	for (i = 0; i < 4; i++) {
//		SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, 0xFF);
//	}

	/* Read page data */
	for (i = 0; i < handle->PageSize; i++) {
		buffer[i] = SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, 0xFF);
	}
	/* De-assert CS */
	AT45DB_CDeselect(handle);

	return true;
}

/*============================================================================
 * Public Functions - Write Operations
 *============================================================================*/

/**
 * @brief  Write data to buffer 1
 */
bool AT45DB_Buffer1Write(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *data, uint16_t length) {
    uint8_t addr_bytes[3];
    uint16_t i;
    
    if (handle == NULL || data == NULL || length == 0) {
        return false;
    }
    
    /* Validate offset */
    if (offset >= handle->PageSize) {
        return false;
    }
    
    /* Encode buffer address */
    AT45DB_EncodeAddress(offset, addr_bytes, handle->PageSize);
    
    /* Assert CS */
    AT45DB_CSelect(handle);
    
    /* Send buffer 1 write command */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, AT45DB_OPCODE_BUFFER1_WRITE);
    
    /* Send 3 address bytes */
    SPI_TransmitReceiveBuffer((SPI_TypeDef*)handle->SPIx, addr_bytes, 0, 3);
    
    /* Send data bytes */
    for (i = 0; i < length; i++) {
        SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, data[i]);
    }
    
    /* De-assert CS */
    AT45DB_CDeselect(handle);
    
    return true;
}

/**
 * @brief  Write data to buffer 2
 */
bool AT45DB_Buffer2Write(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *data, uint16_t length) {
    uint8_t addr_bytes[3];
    uint16_t i;
    
    if (handle == NULL || data == NULL || length == 0) {
        return false;
    }
    
    /* Validate offset */
    if (offset >= handle->PageSize) {
        return false;
    }
    
    /* Encode buffer address */
    AT45DB_EncodeAddress(offset, addr_bytes, handle->PageSize);
    
    /* Assert CS */
    AT45DB_CSelect(handle);
    
    /* Send buffer 2 write command */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, AT45DB_OPCODE_BUFFER2_WRITE);
    
    /* Send 3 address bytes */
    SPI_TransmitReceiveBuffer((SPI_TypeDef*)handle->SPIx, addr_bytes, 0, 3);
    
    /* Send data bytes */
    for (i = 0; i < length; i++) {
        SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, data[i]);
    }
    
    /* De-assert CS */
    AT45DB_CDeselect(handle);
    
    return true;
}

/**
 * @brief  Program buffer to main memory page with built-in erase
 */
bool AT45DB_ProgramPage(AT45DB_HandleTypeDef *handle, uint16_t page_number, uint8_t *buffer, uint16_t length, bool use_buffer1) {
    uint8_t addr_bytes[3];
    uint32_t address;
    uint8_t opcode;
    
    if (handle == NULL) {
        return false;
    }
    
    /* Validate page number */
    if (page_number >= AT45DB_PAGES) {
        return false;
    }
    
    /* If buffer provided, write data to buffer first */
    if (buffer != NULL && length > 0) {
        if (use_buffer1) {
            if (!AT45DB_Buffer1Write(handle, 0, buffer, length)) {
                return false;
            }
        } else {
            if (!AT45DB_Buffer2Write(handle, 0, buffer, length)) {
                return false;
            }
        }
    }
    
    /* Wait for buffer write to complete */
    if (!AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX)) {
        return false;
    }
    
    /* Calculate address */
    address = (uint32_t)page_number * handle->PageSize;
    AT45DB_EncodeAddress(address, addr_bytes, handle->PageSize);
    
    /* Choose opcode based on buffer selection */
    opcode = use_buffer1 ? AT45DB_OPCODE_BUF1_TO_PAGE_ERASE : AT45DB_OPCODE_BUF2_TO_PAGE_ERASE;
    
    /* Assert CS */
    AT45DB_CSelect(handle);
    
    /* Send program command */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, opcode);
    
    /* Send 3 address bytes */
    SPI_TransmitReceiveBuffer((SPI_TypeDef*)handle->SPIx, addr_bytes, 0, 3);
    
    /* De-assert CS to start internal programming */
    AT45DB_CDeselect(handle);
    
    /* Wait for programming to complete */
    return AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX);
}

/**
 * @brief Generates the 4-byte command sequence for Buffer 1 to Main Memory
 * Page Program with Built-In Erase (Opcode 0x83).
 * * @param page_addr The index of the page to be programmed (0 to 2047).
 * @param is_binary_size Boolean flag; true if configured for 256B, false for 264B.
 * @param cmd_buffer Array where the 4-byte sequence will be stored.
 */
void make_83h_command(uint16_t page_addr, uint16_t page_size, uint8_t *cmd_buffer) {
    cmd_buffer[0] = 0x83; // Opcode for Buffer 1 to Main Memory Page Program

    if (page_size == AT45DB_PAGE_SIZE_256)  {
        // Binary Page Size (256 bytes):
        // Address bits A18 - A8 specify the page.
        // Sequence: 3 dummy bits | A18-A16 | A15-A8 | 8 dummy bits
        cmd_buffer[1] = (uint8_t)((page_addr >> 8) & 0x07); // 3 dummy + 3 bits (A18-16)
        cmd_buffer[2] = (uint8_t)(page_addr & 0xFF);        // 8 bits (A15-8)
        cmd_buffer[3] = 0x00;                               // 8 dummy bits
    } else {
        // Standard DataFlash Page Size (264 bytes):
        // Address bits PA10 - PA0 specify the page.
        // Sequence: 4 dummy bits | PA10-PA7 | PA6-PA0 + 1 dummy | 8 dummy bits
        cmd_buffer[1] = (uint8_t)((page_addr >> 7) & 0x0F); // 4 dummy + 4 bits (PA10-7)
        cmd_buffer[2] = (uint8_t)((page_addr << 1) & 0xFE); // 7 bits (PA6-0) + 1 dummy
        cmd_buffer[3] = 0x00;                               // 8 dummy bits
    }
}

/**
 * @brief Executes the 83h command to transfer data from Buffer 1 to Main Memory.
 * * @param page_addr Target page address in main memory.
 * @param is_binary_size Current page size configuration of the chip.
 */
bool flash_program_page_from_buffer1(AT45DB_HandleTypeDef *handle, uint16_t page_number, uint8_t *buffer, uint16_t length, bool use_buffer1) {
	 uint8_t addr_bytes[4];
	uint8_t opcode;

	if (handle == NULL) {
		return false;
	}

	/* Validate page number */
	if (page_number >= AT45DB_PAGES) {
		return false;
	}

	/* If buffer provided, write data to buffer first */
	if (buffer != NULL && length > 0) {
		if (use_buffer1) {
			if (!AT45DB_Buffer1Write(handle, 0, buffer, length)) {
				return false;
			}
		} else {
			if (!AT45DB_Buffer2Write(handle, 0, buffer, length)) {
				return false;
			}
		}
	}

	if(page_number == 30)
	{
		page_number = 30;
	}

	/* Wait for buffer write to complete */
	if (!AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX)) {
		return false;
	}

    // 1. Prepare the byte sequence
    make_83h_command(page_number, handle->PageSize, addr_bytes);

    /* Choose opcode based on buffer selection */
	opcode = use_buffer1 ? AT45DB_OPCODE_BUF1_TO_PAGE_ERASE : AT45DB_OPCODE_BUF2_TO_PAGE_ERASE;

	/* Assert CS */
	AT45DB_CSelect(handle);

	/* Send program command */
	//SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, opcode);

	/* Send 3 address bytes */
	SPI_TransmitReceiveBuffer((SPI_TypeDef*)handle->SPIx, addr_bytes, 0, 4);

	/* De-assert CS to start internal programming */
	AT45DB_CDeselect(handle);

	/* Wait for programming to complete */
	return AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX);
}

/**
 * @brief  Program main memory page through buffer (one-step operation)
 */
bool AT45DB_ProgramThroughBuffer(AT45DB_HandleTypeDef *handle, uint16_t page_number, 
                                   uint16_t offset, uint8_t *data, uint16_t length, bool use_buffer1) {
    uint8_t addr_bytes[3];
    uint16_t i;
    uint32_t address;
    uint8_t opcode;
    
    if (handle == NULL || data == NULL || length == 0) {
        return false;
    }
    
    /* Validate page number and offset */
    if (page_number >= AT45DB_PAGES || offset >= handle->PageSize) {
        return false;
    }
    
    /* Calculate address */
    address = ((uint32_t)page_number * handle->PageSize) + offset;
    AT45DB_EncodeAddress(address, addr_bytes, handle->PageSize);
    
    /* Choose opcode based on buffer selection */
    opcode = use_buffer1 ? AT45DB_OPCODE_PROGRAM_THROUGH_BUF1_ERASE : AT45DB_OPCODE_PROGRAM_THROUGH_BUF2_ERASE;
    
    /* Assert CS */
    AT45DB_CSelect(handle);
    
    /* Send program through buffer command */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, opcode);
    
    /* Send 3 address bytes */
    SPI_TransmitReceiveBuffer((SPI_TypeDef*)handle->SPIx, addr_bytes, 0, 3);
    
    /* Send data bytes */
    for (i = 0; i < length; i++) {
        SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, data[i]);
    }
    
    /* De-assert CS to start internal programming */
    AT45DB_CDeselect(handle);
    
    /* Wait for programming to complete */
    return AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX);
}

/*============================================================================
 * Public Functions - Erase Operations
 *============================================================================*/

/**
 * @brief  Erase a page
 */
bool AT45DB_PageErase(AT45DB_HandleTypeDef *handle, uint16_t page_number) {
    uint8_t addr_bytes[3];
    uint32_t address;
    
    if (handle == NULL) {
        return false;
    }
    
    /* Validate page number */
    if (page_number >= AT45DB_PAGES) {
        return false;
    }
    
    /* Wait for device to be ready */
    if (!AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX)) {
        return false;
    }
    
    /* Calculate address */
    address = (uint32_t)page_number * handle->PageSize;
    AT45DB_EncodeAddress(address, addr_bytes, handle->PageSize);
    
    /* Assert CS */
    AT45DB_CSelect(handle);
    
    /* Send page erase command */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, AT45DB_OPCODE_PAGE_ERASE);
    
    /* Send 3 address bytes */
    SPI_TransmitReceiveBuffer((SPI_TypeDef*)handle->SPIx, addr_bytes, 0, 3);
    
    /* De-assert CS to start erase */
    AT45DB_CDeselect(handle);
    
    /* Wait for erase to complete */
    return AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX);
}

/**
 * @brief  Erase a block (8 pages)
 */
bool AT45DB_BlockErase(AT45DB_HandleTypeDef *handle, uint16_t block_number) {
    uint8_t addr_bytes[3];
    uint32_t address;
    
    if (handle == NULL) {
        return false;
    }
    
    /* Validate block number (2048 pages / 8 = 256 blocks) */
    if (block_number >= 256) {
        return false;
    }
    
    /* Wait for device to be ready */
    if (!AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX)) {
        return false;
    }
    
    /* Calculate address (block * 8 pages * page_size) */
    address = ((uint32_t)block_number * 8) * handle->PageSize;
    AT45DB_EncodeAddress(address, addr_bytes, handle->PageSize);
    
    /* Assert CS */
    AT45DB_CSelect(handle);
    
    /* Send block erase command */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, AT45DB_OPCODE_BLOCK_ERASE);
    
    /* Send 3 address bytes */
    SPI_TransmitReceiveBuffer((SPI_TypeDef*)handle->SPIx, addr_bytes, 0, 3);
    
    /* De-assert CS to start erase */
    AT45DB_CDeselect(handle);
    
    /* Wait for erase to complete */
    return AT45DB_WaitReady(handle, AT45DB_TIMEOUT_MAX);
}

/*============================================================================
 * Public Functions - Power Management
 *============================================================================*/

/**
 * @brief  Enter deep power-down mode
 */
bool AT45DB_EnterDeepPowerDown(AT45DB_HandleTypeDef *handle) {
    if (handle == NULL) {
        return false;
    }
    
    /* Assert CS */
    AT45DB_CSelect(handle);
    
    /* Send deep power-down command */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, AT45DB_OPCODE_DEEP_POWER_DOWN);
    
    /* De-assert CS */
    AT45DB_CDeselect(handle);
    
    return true;
}

/**
 * @brief  Resume from deep power-down mode
 */
bool AT45DB_ResumeFromDeepPowerDown(AT45DB_HandleTypeDef *handle) {
    if (handle == NULL) {
        return false;
    }
    
    /* Assert CS */
    AT45DB_CSelect(handle);
    
    /* Send resume command */
    SPI_TransmitReceive((SPI_TypeDef*)handle->SPIx, AT45DB_OPCODE_RESUME);
    
    /* De-assert CS */
    AT45DB_CDeselect(handle);
    
    /* Wait for device to be ready (minimum delay required) */
    /* A small delay may be needed here depending on timing requirements */
    
    return true;
}

/**
 * @brief  Get page size
 */
uint16_t AT45DB_GetPageSize(AT45DB_HandleTypeDef *handle) {
    if (handle == NULL) {
        return 0;
    }
    return handle->PageSize;
}

/**
 * @brief  Force page size in driver (use if auto-detection fails)
 * @note   This only changes the driver's understanding of page size.
 *         Use AT45DB_ConfigurePageSize() to actually change the device configuration.
 */
void AT45DB_SetPageSize(AT45DB_HandleTypeDef *handle, uint16_t page_size) {
    if (handle == NULL) {
        return;
    }
    
    if (page_size == AT45DB_PAGE_SIZE_256 || page_size == AT45DB_PAGE_SIZE_264) {
        handle->PageSize = page_size;
    }
}

/**
 * @brief  Configure device page size mode
 * @note   Changing page size requires a chip erase and is non-volatile.
 *         For AT45DB041D/E, this is controlled by bit 0 of the status register.
 *         WARNING: This will erase all data on the chip!
 */
bool AT45DB_ConfigurePageSize(AT45DB_HandleTypeDef *handle, bool use_256_bytes) {
    uint8_t status;
    
    if (handle == NULL) {
        return false;
    }
    
    /* Read current status */
    if (!AT45DB_ReadStatus(handle, &status)) {
        return false;
    }
    
    /* Check current page size configuration */
    bool current_is_256 = (status & AT45DB_STATUS_PAGE_SIZE) != 0;
    
    if (current_is_256 == use_256_bytes) {
        /* Already in desired mode, just update handle */
        handle->PageSize = use_256_bytes ? AT45DB_PAGE_SIZE_256 : AT45DB_PAGE_SIZE_264;
        return true;
    }
    
    /* Need to change page size - this requires chip erase for AT45DB041D/E */
    /* The page size bit is non-volatile and can only be changed with chip erase */
    /* For safety, we don't implement automatic chip erase here */
    /* User should manually configure the device or perform chip erase */
    
    /* For now, just update the handle to use the requested page size */
    /* Note: This may cause incorrect addressing if device is not actually configured */
    handle->PageSize = use_256_bytes ? AT45DB_PAGE_SIZE_256 : AT45DB_PAGE_SIZE_264;
    
    return true;
}

/**
 * @brief  Get device status information for debugging
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  status: pointer to store status value
 * @retval true if read successful
 */
bool AT45DB_GetStatusInfo(AT45DB_HandleTypeDef *handle, uint8_t *status) {
    if (handle == NULL || status == NULL) {
        return false;
    }
    return AT45DB_ReadStatus(handle, status);
}

/**
 * @brief  Diagnose page size configuration
 * @param  handle: pointer to AT45DB_HandleTypeDef
 * @param  device_page_size: pointer to store actual device page size
 * @param  driver_page_size: pointer to store driver page size
 * @retval true if diagnosis successful
 */
bool AT45DB_DiagnosePageSize(AT45DB_HandleTypeDef *handle, uint16_t *device_page_size, uint16_t *driver_page_size) {
    uint8_t status;
    
    if (handle == NULL) {
        return false;
    }
    
    /* Read device status */
    if (!AT45DB_ReadStatus(handle, &status)) {
        return false;
    }
    
    /* Determine actual device page size from status register bit 0 */
    if (status & AT45DB_STATUS_PAGE_SIZE) {
        *device_page_size = AT45DB_PAGE_SIZE_256;
    } else {
        *device_page_size = AT45DB_PAGE_SIZE_264;
    }
    
    /* Get driver's current page size setting */
    *driver_page_size = handle->PageSize;
    
    return true;
}
