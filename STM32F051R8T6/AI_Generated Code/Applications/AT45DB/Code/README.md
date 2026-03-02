# AT45DB DataFlash Driver Example

This directory contains a comprehensive example demonstrating the usage of the AT45DB041D/AT45DB041E DataFlash driver for STM32F051R8T6 microcontroller.

## Files

- `AT45DB.h` - Header file with driver API declarations
- `AT45DB.c` - Implementation of the AT45DB driver
- `AT45DBExample.c` - Comprehensive example demonstrating all driver features
- `README.md` - This documentation

## Features Demonstrated

The example demonstrates all major features of the AT45DB driver:

### 1. Device Initialization
- Initializes SPI interface and AT45DB driver
- Verifies device presence and reads device information
- Prints manufacturer ID, device ID, and page size

### 2. Buffer Operations
- Writes data to Buffer 1
- Reads data back from Buffer 1
- Verifies buffer contents

### 3. Page Operations
- Erases a single page
- Programs data from Buffer 1 to a page
- Reads back data from a specific page
- Verifies page contents

### 4. Continuous Read
- Reads data continuously from memory
- Demonstrates sequential read operations

### 5. Multi-Page Operations
- Tests operations on multiple consecutive pages
- Verifies data retention across pages

### 6. Power Management
- Enters and resumes from Deep Power Down mode
- Verifies device functionality after power management operations

## Hardware Configuration

### SPI Interface
- **SPI Instance**: SPI1
- **SCK**: PA5
- **MISO**: PA6
- **MOSI**: PA7
- **CS**: PA4

### System Clock
- **Source**: HSI (Internal 8MHz oscillator)
- **PLL**: Enabled, HSI/2 * 12 = 48MHz
- **AHB Prescaler**: 1 (48MHz)
- **APB Prescaler**: 1 (48MHz)

## Compilation and Usage

1. Include all three files in your STM32F051R8T6 project
2. Make sure the driver files are correctly referenced in your include paths
3. Compile the project
4. Flash the binary to your STM32F051R8T6 microcontroller
5. Connect to the serial port (if UART output is configured)
6. Observe the test results

## Modifications for Your Application

### System Clock Configuration
Modify `SystemClock_Config()` if you need to use a different clock source or frequency.

### SPI Configuration
Adjust `SPI_Config()` to change SPI settings (clock polarity, phase, baud rate, etc.).

### GPIO Configuration
Modify `GPIO_Config()` to use different SPI pins or CS pin.

### Test Parameters
Change the following defines in `AT45DBExample.c` to customize the tests:
- `TEST_PAGE_START` - First page to test
- `TEST_BUFFER_SIZE` - Size of test buffer
- `TEST_PAGE_COUNT` - Number of pages to test

## Troubleshooting

### SPI Communication Issues
- Verify SPI pin connections
- Check SPI clock polarity and phase (should be Mode 0: CPOL=0, CPHA=0)
- Ensure CS pin is correctly configured and controlled
- Check SPI baud rate (should not exceed 20MHz for AT45DB041D/E)

### Device Not Detected
- Verify CS pin configuration and connection
- Check SPI communication settings
- Ensure device is powered correctly (2.7V - 3.6V)

### Write/Read Errors
- Make sure pages are erased before writing
- Check that write operations are completed before reading back
- Verify buffer sizes match read/write operations

## Driver API Reference

Refer to `AT45DB.h` for the complete driver API documentation.
