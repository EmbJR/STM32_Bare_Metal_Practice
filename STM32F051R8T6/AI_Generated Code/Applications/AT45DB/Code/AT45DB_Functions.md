# AT45DB DataFlash Driver Functions

A comprehensive overview of all functions available in the AT45DB041D/AT45DB041E DataFlash driver for STM32F051R8T6.

## Initialization and Configuration

### AT45DB_Init
```c
bool AT45DB_Init(AT45DB_HandleTypeDef *handle, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint32_t cs_pin);
```
- **Description**: Initializes the AT45DB driver and configures the SPI interface
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `hspi`: Pointer to SPI handle structure
  - `cs_port`: GPIO port for chip select pin
  - `cs_pin`: GPIO pin number for chip select
- **Returns**: `true` if initialization successful, `false` otherwise

### AT45DB_DeInit
```c
void AT45DB_DeInit(AT45DB_HandleTypeDef *handle);
```
- **Description**: De-initializes the AT45DB driver
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure

## Device Identification

### AT45DB_ReadJEDECID
```c
bool AT45DB_ReadJEDECID(AT45DB_HandleTypeDef *handle, uint8_t *manufacturer_id, uint8_t *device_id);
```
- **Description**: Reads the JEDEC identification from the AT45DB chip
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `manufacturer_id`: Pointer to store manufacturer ID (should be 0x1F for Atmel)
  - `device_id`: Pointer to store device ID (0x2600 for AT45DB041D, 0x2601 for AT45DB041E)
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_CheckDevicePresence
```c
bool AT45DB_CheckDevicePresence(AT45DB_HandleTypeDef *handle);
```
- **Description**: Verifies that the AT45DB chip is present and supported
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
- **Returns**: `true` if device is present and supported, `false` otherwise

## Status and Configuration

### AT45DB_ReadStatus
```c
bool AT45DB_ReadStatus(AT45DB_HandleTypeDef *handle, uint8_t *status);
```
- **Description**: Reads the status register from the AT45DB chip
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `status`: Pointer to store status register value
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_IsReady
```c
bool AT45DB_IsReady(AT45DB_HandleTypeDef *handle);
```
- **Description**: Checks if the AT45DB chip is ready for operations
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
- **Returns**: `true` if device is ready, `false` if busy

### AT45DB_WaitReady
```c
bool AT45DB_WaitReady(AT45DB_HandleTypeDef *handle, uint32_t timeout_ms);
```
- **Description**: Waits for the AT45DB chip to become ready
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `timeout_ms`: Maximum time to wait in milliseconds
- **Returns**: `true` if device became ready, `false` if timeout

### AT45DB_GetPageSize
```c
uint16_t AT45DB_GetPageSize(AT45DB_HandleTypeDef *handle);
```
- **Description**: Returns the current page size of the AT45DB chip
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
- **Returns**: Page size in bytes (256 or 264)

## Read Operations

### AT45DB_ReadContinuous
```c
bool AT45DB_ReadContinuous(AT45DB_HandleTypeDef *handle, uint32_t address, uint8_t *buffer, uint32_t length);
```
- **Description**: Reads data continuously from memory starting at a specific address
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `address`: Start address for read operation
  - `buffer`: Pointer to buffer to store read data
  - `length`: Number of bytes to read
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_ReadPage
```c
bool AT45DB_ReadPage(AT45DB_HandleTypeDef *handle, uint16_t page, uint16_t offset, uint8_t *buffer, uint32_t length);
```
- **Description**: Reads data from a specific page and offset
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `page`: Page number to read from
  - `offset`: Offset within the page to start reading
  - `buffer`: Pointer to buffer to store read data
  - `length`: Number of bytes to read
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_ReadBuffer1
```c
bool AT45DB_ReadBuffer1(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *buffer, uint32_t length);
```
- **Description**: Reads data from the AT45DB chip's Buffer 1
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `offset`: Offset within Buffer 1 to start reading
  - `buffer`: Pointer to buffer to store read data
  - `length`: Number of bytes to read
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_ReadBuffer2
```c
bool AT45DB_ReadBuffer2(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *buffer, uint32_t length);
```
- **Description**: Reads data from the AT45DB chip's Buffer 2
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `offset`: Offset within Buffer 2 to start reading
  - `buffer`: Pointer to buffer to store read data
  - `length`: Number of bytes to read
- **Returns**: `true` if successful, `false` otherwise

## Buffer Operations

### AT45DB_WriteBuffer1
```c
bool AT45DB_WriteBuffer1(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *buffer, uint32_t length);
```
- **Description**: Writes data to the AT45DB chip's Buffer 1
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `offset`: Offset within Buffer 1 to start writing
  - `buffer`: Pointer to buffer containing data to write
  - `length`: Number of bytes to write
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_WriteBuffer2
```c
bool AT45DB_WriteBuffer2(AT45DB_HandleTypeDef *handle, uint16_t offset, uint8_t *buffer, uint32_t length);
```
- **Description**: Writes data to the AT45DB chip's Buffer 2
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `offset`: Offset within Buffer 2 to start writing
  - `buffer`: Pointer to buffer containing data to write
  - `length`: Number of bytes to write
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_ProgramBuffer1
```c
bool AT45DB_ProgramBuffer1(AT45DB_HandleTypeDef *handle, uint16_t page, uint16_t offset, uint8_t *buffer, uint32_t length);
```
- **Description**: Programs data from Buffer 1 directly to a specific page with built-in erase
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `page`: Page number to program
  - `offset`: Offset within the page to start programming
  - `buffer`: Pointer to buffer containing data to program
  - `length`: Number of bytes to program
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_ProgramBuffer2
```c
bool AT45DB_ProgramBuffer2(AT45DB_HandleTypeDef *handle, uint16_t page, uint16_t offset, uint8_t *buffer, uint32_t length);
```
- **Description**: Programs data from Buffer 2 directly to a specific page with built-in erase
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `page`: Page number to program
  - `offset`: Offset within the page to start programming
  - `buffer`: Pointer to buffer containing data to program
  - `length`: Number of bytes to program
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_Buffer1ToPage
```c
bool AT45DB_Buffer1ToPage(AT45DB_HandleTypeDef *handle, uint16_t page);
```
- **Description**: Transfers data from Buffer 1 to main memory page
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `page`: Page number to transfer data to
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_Buffer2ToPage
```c
bool AT45DB_Buffer2ToPage(AT45DB_HandleTypeDef *handle, uint16_t page);
```
- **Description**: Transfers data from Buffer 2 to main memory page
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `page`: Page number to transfer data to
- **Returns**: `true` if successful, `false` otherwise

## Erase Operations

### AT45DB_ErasePage
```c
bool AT45DB_ErasePage(AT45DB_HandleTypeDef *handle, uint16_t page);
```
- **Description**: Erases a single page in memory
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `page`: Page number to erase
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_EraseBlock
```c
bool AT45DB_EraseBlock(AT45DB_HandleTypeDef *handle, uint16_t block);
```
- **Description**: Erases an 8-page block in memory
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `block`: Block number to erase (each block is 8 pages)
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_EraseSector
```c
bool AT45DB_EraseSector(AT45DB_HandleTypeDef *handle, uint16_t sector);
```
- **Description**: Erases a 64KB sector in memory
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
  - `sector`: Sector number to erase (each sector is 64KB)
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_EraseChip
```c
bool AT45DB_EraseChip(AT45DB_HandleTypeDef *handle);
```
- **Description**: Erases the entire chip memory
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
- **Returns**: `true` if successful, `false` otherwise

## Power Management

### AT45DB_EnterDeepPowerDown
```c
bool AT45DB_EnterDeepPowerDown(AT45DB_HandleTypeDef *handle);
```
- **Description**: Places the AT45DB chip into Deep Power Down mode
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_ResumeFromDeepPowerDown
```c
bool AT45DB_ResumeFromDeepPowerDown(AT45DB_HandleTypeDef *handle);
```
- **Description**: Resumes the AT45DB chip from Deep Power Down mode
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_EnterUltraDeepPowerDown
```c
bool AT45DB_EnterUltraDeepPowerDown(AT45DB_HandleTypeDef *handle);
```
- **Description**: Places the AT45DB chip into Ultra Deep Power Down mode (AT45DB041E only)
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
- **Returns**: `true` if successful, `false` otherwise

### AT45DB_ExitUltraDeepPowerDown
```c
bool AT45DB_ExitUltraDeepPowerDown(AT45DB_HandleTypeDef *handle);
```
- **Description**: Exits the AT45DB chip from Ultra Deep Power Down mode (AT45DB041E only)
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure
- **Returns**: `true` if successful, `false` otherwise

## Helper Functions

### AT45DB_Select (static)
```c
static void AT45DB_Select(AT45DB_HandleTypeDef *handle);
```
- **Description**: Selects the AT45DB chip by pulling CS low
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure

### AT45DB_Deselect (static)
```c
static void AT45DB_Deselect(AT45DB_HandleTypeDef *handle);
```
- **Description**: Deselects the AT45DB chip by pulling CS high
- **Parameters**:
  - `handle`: Pointer to AT45DB handle structure

### AT45DB_EncodeAddress (static)
```c
static void AT45DB_EncodeAddress(uint32_t address, uint8_t *addr_bytes, uint16_t page_size);
```
- **Description**: Encodes an address based on the current page size mode
- **Parameters**:
  - `address`: Address to encode
  - `addr_bytes`: Pointer to buffer to store encoded address (3 bytes)
  - `page_size`: Current page size (256 or 264 bytes)
