# STM32F051R8T6 Flash Driver Documentation

## Overview

This Flash driver provides a complete, modular API for programming, erasing, and configuring the internal Flash memory of the STM32F051R8T6 microcontroller. The driver is designed to be:

- **Independent**: No external dependencies (CMSIS, HAL, etc.)
- **Modular**: Clean API with well-organized functions
- **Low-footprint**: Minimal code space overhead
- **API-Rich**: Comprehensive functionality for various use cases

## Flash Memory Characteristics

- **Total Capacity**: 64 KB (0x08000000 - 0x0800FFFF)
- **Page Size**: 1024 bytes (0x400)
- **Number of Pages**: 64
- **Write Unit**: 16-bit (half-word) minimum
- **Erase Unit**: Page (full 1 KB)

## System Clock Timing

For proper Flash operation, latency must be configured based on system clock frequency:

| Frequency Range | Latency | Wait States |
|-----------------|---------|-------------|
| ≤ 24 MHz | 0 | 0 |
| 24-48 MHz | 1 | 1 |

## API Reference

### Configuration Functions

#### `FLASH_Init()`
- **Purpose**: Initialize Flash controller
- **Parameters**: None
- **Return**: None
- **Note**: Call this before any Flash operations

#### `FLASH_SetLatency(Flash_Latency_t latency)`
- **Purpose**: Set Flash latency (wait states)
- **Parameters**: 
  - `latency`: FLASH_LATENCY_0 or FLASH_LATENCY_1
- **Return**: None
- **Critical**: Must be set correctly for your CPU frequency

#### `FLASH_ConfigureForFrequency(uint32_t sysclk_freq_hz)`
- **Purpose**: Auto-configure Flash for given system clock frequency
- **Parameters**: 
  - `sysclk_freq_hz`: System clock in Hz
- **Return**: `Flash_Status_t`
- **Benefit**: Automatically sets optimal latency and enables prefetch

#### `FLASH_EnablePrefetch() / FLASH_DisablePrefetch()`
- **Purpose**: Enable/disable prefetch buffer
- **Return**: None
- **Note**: Prefetch improves performance, should be enabled unless low power is critical

### Status and Control Functions

#### `FLASH_GetStatus()`
- **Purpose**: Get current Flash operation status
- **Return**: `Flash_Status_t`
- **Status Values**:
  - `FLASH_STATUS_OK`: Operation successful
  - `FLASH_STATUS_BUSY`: Flash is busy
  - `FLASH_STATUS_PGERR`: Programming error
  - `FLASH_STATUS_WRPRTERR`: Write protection error
  - `FLASH_STATUS_OPTERR`: Option byte error

#### `FLASH_IsBusy()`
- **Purpose**: Quick check if Flash is busy
- **Return**: `true` if busy, `false` if ready
- **Use**: For polling-based operations

#### `FLASH_WaitForComplete(uint32_t timeout_ms)`
- **Purpose**: Wait for current operation with timeout
- **Parameters**: 
  - `timeout_ms`: Timeout in milliseconds
- **Return**: `Flash_Status_t`
- **Benefit**: Prevents infinite blocking

#### `FLASH_Unlock() / FLASH_Lock()`
- **Purpose**: Unlock/lock Flash for write/erase operations
- **Return**: `Flash_Status_t` (Unlock only)
- **Important**: 
  - Unlock writes keys in sequence
  - Lock prevents accidental modifications
  - Program/Erase functions handle unlock/lock automatically

#### `FLASH_ClearErrors() / FLASH_ClearEOP()`
- **Purpose**: Clear error flags and end-of-operation flag
- **Return**: None
- **Note**: Call after operations to clear status flags

### Erase Functions

#### `FLASH_ErasePage(uint32_t page_addr)`
- **Purpose**: Erase a single Flash page (1 KB)
- **Parameters**: 
  - `page_addr`: Any address within the page
- **Return**: `Flash_Status_t`
- **Important**: 
  - Function auto-aligns to page boundary
  - All data in page becomes 0xFFFF
  - Automatically unlocks/locks Flash
  - Safer than manual unlock/erase sequence

#### `FLASH_ErasePages(uint32_t start_addr, uint16_t page_count)`
- **Purpose**: Erase multiple consecutive pages
- **Parameters**:
  - `start_addr`: Start address (page-aligned)
  - `page_count`: Number of pages to erase
- **Return**: `Flash_Status_t`
- **Benefit**: Erases range efficiently

#### `FLASH_MassErase()`
- **Purpose**: Erase entire Flash memory
- **Return**: `Flash_Status_t`
- **Warning**: Erases ALL Flash content including application code
- **Use Cases**: Firmware updates with bootloader, complete memory reset

### Programming Functions

#### `FLASH_ProgramHalfWord(uint32_t addr, uint16_t data)`
- **Purpose**: Program a 16-bit value
- **Parameters**:
  - `addr`: Target address (half-word aligned)
  - `data`: 16-bit value to program
- **Return**: `Flash_Status_t`
- **Requirements**: 
  - Address must be half-word aligned (addr & 0x01 == 0)
  - Destination must be erased (0xFFFF) first
- **Note**: Automatically unlocks and locks Flash

#### `FLASH_ProgramWord(uint32_t addr, uint32_t data)`
- **Purpose**: Program a 32-bit value
- **Parameters**:
  - `addr`: Target address (word aligned)
  - `data`: 32-bit value to program
- **Return**: `Flash_Status_t`
- **Implementation**: Performs two consecutive 16-bit programs
- **Requirements**: Address must be word aligned (addr & 0x03 == 0)

#### `FLASH_ProgramBuffer(uint32_t dest_addr, const uint8_t *src_data, uint16_t length)`
- **Purpose**: Program buffer to Flash
- **Parameters**:
  - `dest_addr`: Destination address (half-word aligned)
  - `src_data`: Source data pointer
  - `length`: Number of bytes (must be even)
- **Return**: `Flash_Status_t`
- **Advantages**:
  - More efficient than individual half-word programs
  - Handles endianness conversion automatically
  - Ideal for configuration data
- **Requirements**: 
  - Length must be even
  - Destination must be erased first

### Verification Functions

#### `FLASH_VerifyHalfWord(uint32_t addr, uint16_t expected_value)`
- **Purpose**: Verify 16-bit value in Flash
- **Parameters**:
  - `addr`: Address to verify
  - `expected_value`: Expected value
- **Return**: `true` if match, `false` otherwise
- **Use**: Post-programming verification

#### `FLASH_VerifyBuffer(uint32_t dest_addr, const uint8_t *src_data, uint16_t length)`
- **Purpose**: Verify entire buffer in Flash
- **Parameters**:
  - `dest_addr`: Flash address
  - `src_data`: Expected data
  - `length`: Number of bytes to verify
- **Return**: `true` if all match, `false` otherwise

### Utility Functions

#### `FLASH_IsValidAddress(uint32_t addr)`
- **Purpose**: Check if address is within Flash memory
- **Return**: `true` if valid, `false` otherwise

#### `FLASH_GetPageNumber(uint32_t addr)`
- **Purpose**: Get page number from address
- **Parameters**: `addr` - Flash address
- **Return**: Page number (0-63), or -1 if invalid

#### `FLASH_GetPageAddress(uint8_t page_num)`
- **Purpose**: Get page start address from number
- **Parameters**: `page_num` - Page number (0-63)
- **Return**: Page start address, or 0 if invalid

## Usage Examples

### Example 1: Basic Initialization

```c
Flash_Status_t status = FLASH_Init();
if (status == FLASH_STATUS_OK) {
    FLASH_ConfigureForFrequency(48000000);  // 48 MHz operation
}
```

### Example 2: Erase and Program Page

```c
Flash_Status_t status;
uint8_t data[256] = {0x01, 0x02, 0x03, ...};

// Erase page 60
status = FLASH_ErasePage(FLASH_GetPageAddress(60));
if (status != FLASH_STATUS_OK) return status;

// Program data
status = FLASH_ProgramBuffer(FLASH_GetPageAddress(60), data, 256);
if (status != FLASH_STATUS_OK) return status;

// Verify
if (!FLASH_VerifyBuffer(FLASH_GetPageAddress(60), data, 256)) {
    return FLASH_STATUS_PGERR;
}
```

### Example 3: Store Configuration

```c
typedef struct {
    uint32_t magic;
    uint32_t parameter1;
    uint32_t parameter2;
} ConfigData_t;

ConfigData_t config = {0xDEADBEEF, 0x12345678, 0x9ABCDEF0};

// Erase config page
FLASH_ErasePage(0x0800F000);  // Last page

// Program config
FLASH_ProgramBuffer(0x0800F000, (uint8_t *)&config, sizeof(config));

// Later: Read back
ConfigData_t read_config;
volatile uint8_t *src = (volatile uint8_t *)0x0800F000;
for (int i = 0; i < sizeof(config); i++) {
    ((uint8_t *)&read_config)[i] = src[i];
}
```

### Example 4: Safe Programming with Verification

```c
Flash_Status_t SafeProgram(uint32_t addr, uint8_t *data, uint16_t len) {
    Flash_Status_t status;
    
    // Program
    status = FLASH_ProgramBuffer(addr, data, len);
    if (status != FLASH_STATUS_OK) return status;
    
    // Verify
    if (!FLASH_VerifyBuffer(addr, data, len)) {
        return FLASH_STATUS_PGERR;
    }
    
    return FLASH_STATUS_OK;
}
```

## Important Notes

1. **Flash Unlocking**: 
   - Keys must be written in exact sequence
   - Wrong order or wrong keys result in lock-up
   - All program/erase functions handle unlocking automatically

2. **Destination Erasing**:
   - Flash must be erased before programming
   - Writing to non-erased locations causes errors
   - Example code includes erase operations

3. **Address Alignment**:
   - Half-word operations: 2-byte alignment
   - Word operations: 4-byte alignment
   - Page operations: 1024-byte alignment

4. **Data Size**:
   - Buffer programming requires even length
   - Total Flash capacity is 64 KB
   - User data typically stored in last few pages

5. **System Clock Configuration**:
   - Latency MUST match clock frequency
   - Incorrect latency causes read errors
   - Use `FLASH_ConfigureForFrequency()` for automatic setup

6. **Optimization**:
   - Prefetch buffer improves read performance
   - Enabled automatically by `FLASH_ConfigureForFrequency()`
   - Minimal power overhead when enabled

## Code Size Considerations

The driver is optimized for low code footprint:
- **Header**: ~4 KB (comprehensive documentation and macros)
- **Implementation**: ~3.5 KB (core functionality)
- **Example**: ~5 KB (12 different usage patterns)
- **Total**: ~12.5 KB (approximately)

Unused functions can be removed to further reduce size.

## Typical Workflow

```
1. Call FLASH_Init()
2. Call FLASH_ConfigureForFrequency() with your system clock
3. Validate address with FLASH_IsValidAddress()
4. Erase pages with FLASH_ErasePage() or FLASH_ErasePages()
5. Program data with FLASH_ProgramBuffer() or FLASH_ProgramHalfWord()
6. Verify with FLASH_VerifyBuffer() or FLASH_VerifyHalfWord()
7. Handle errors based on return status
```

## Error Handling

All functions return `Flash_Status_t`. Always check return values:

```c
Flash_Status_t status = FLASH_ErasePage(0x08000000);
if (status != FLASH_STATUS_OK) {
    // Handle error
    switch (status) {
        case FLASH_STATUS_PGERR:
            // Programming error - retry or fail
            break;
        case FLASH_STATUS_WRPRTERR:
            // Write protection error - cannot write
            break;
        case FLASH_STATUS_ADDRESS_INVALID:
            // Address out of range
            break;
        // ...
    }
}
```

## Integration with Other Drivers

This driver is independent and works with:
- GPIO driver (for status LEDs during programming)
- UART driver (for debug output/logging)
- Timer driver (for timeout management in critical operations)

No modifications needed for other drivers.

## Version Information

- Driver Version: 1.0
- Target MCU: STM32F051R8T6
- Flash Capacity: 64 KB
- Page Size: 1 KB
- Generated: 2026
