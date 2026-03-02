/**
 * @file    AT45DBExample.c
 * @brief   Comprehensive Example Usage of AT45DB041D/AT45DB041E DataFlash Driver
 * @author  AI Generated
 * @date    2026-03-02
 */

#include "AT45DB.h"
#include "rcc.h"
#include "gpio.h"
#include "spiF051.h"
#include <string.h>

/*============================================================================
 * Example Configuration
 *============================================================================*/
#define AT45DB_SPI_INSTANCE        SPI1
#define AT45DB_CS_PORT             GPIOA
#define AT45DB_CS_PIN              GPIO_PIN_4
#define TEST_PAGE_START            0x00    // First page to test
#define TEST_BUFFER_SIZE           256     // Test buffer size
#define TEST_PAGE_COUNT            2       // Number of pages to test

/*============================================================================
 * Global Variables
 *============================================================================*/
static AT45DB_HandleTypeDef at45db_handle;
static SPI_HandleTypeDef spi_handle;
static uint8_t tx_buffer[TEST_BUFFER_SIZE];
static uint8_t rx_buffer[TEST_BUFFER_SIZE];

/*============================================================================
 * Helper Functions
 *============================================================================*/

/**
 * @brief   System clock configuration
 */
static void SystemClock_Config(void) {
    RCC_Config rcc_config;

    rcc_config.system_clock_source = CLOCK_SOURCE_HSI;
    rcc_config.target_frequency = SYSTEM_CLOCK_48MHZ;
    rcc_config.hse_enabled = false;
    rcc_config.pll_enabled = true;
    rcc_config.pll_source = PLL_SOURCE_HSI_DIV2;
    rcc_config.pll_multiplier = 12; // HSI/2 * 12 = 48MHz
    rcc_config.ahb_prescaler = AHB_PRESCALER_1;
    rcc_config.apb_prescaler = APB_PRESCALER_1;
    rcc_config.hsi48_enabled = false;
    rcc_config.css_enabled = false;

    RCC_Init(&rcc_config);
}

/**
 * @brief   SPI peripheral configuration
 */
static void SPI_Config(void) {
    memset(&spi_handle, 0, sizeof(SPI_HandleTypeDef));

    spi_handle.Instance = AT45DB_SPI_INSTANCE;
    spi_handle.Init.mode = SPI_MODE_MASTER;
    spi_handle.Init.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    spi_handle.Init.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE; // Mode 0
    spi_handle.Init.frame_format = SPI_FRAME_FORMAT_MSB_FIRST;
    spi_handle.Init.data_size = SPI_DATA_SIZE_8BIT;
    spi_handle.Init.baud_rate = SPI_BAUD_RATE_DIV_64; // 12MHz for 48MHz system clock
    spi_handle.Init.comm_mode = SPI_COMM_MODE_FULL_DUPLEX;
    spi_handle.Init.nss_mode = SPI_NSS_SOFTWARE;
    spi_handle.Init.protocol = SPI_FRAME_FORMAT_MOTOROLA;
    spi_handle.Init.rx_fifo_threshold = SPI_FIFO_THRESHOLD_1_2;
    spi_handle.Init.crc_enabled = false;
    spi_handle.Init.crc_polynomial = 0x07;
    spi_handle.Init.nss_pulse_enabled = false;
    spi_handle.is_busy = false;

    SPI_Init(&spi_handle);
}

/**
 * @brief   GPIO configuration
 */
static void GPIO_Config(void) {
    // Enable GPIO clocks
    RCC_EnablePeripheralClock(PERIPH_GPIOA, 0);
    RCC_EnablePeripheralClock(PERIPH_GPIOB, 0);

    // Configure SPI1 pins
    GPIO_InitTypeDef gpio_init;

    // SCK (PA5), MISO (PA6), MOSI (PA7)
    gpio_init.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio_init.Mode = GPIO_MODE_ALTERNATE;
    gpio_init.Ot = GPIO_OTYPE_PP;
    gpio_init.Speed = GPIO_SPEED_HIGH;
    gpio_init.Pull = GPIO_PULL_NO;
    gpio_init.AF = GPIO_AF0; // SPI1 alternate function
    GPIO_Init(GPIOA, &gpio_init);

    // CS pin (PA4) - will be configured by AT45DB_Init
}

/**
 * @brief   Fill test buffer with pattern
 */
static void FillTestBuffer(uint8_t *buffer, uint32_t size, uint8_t seed) {
    for (uint32_t i = 0; i < size; i++) {
        buffer[i] = (uint8_t)(i + seed);
    }
}

/**
 * @brief   Verify that two buffers contain the same data
 */
static bool VerifyBuffers(uint8_t *buffer1, uint8_t *buffer2, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        if (buffer1[i] != buffer2[i]) {
            return false;
        }
    }
    return true;
}

/**
 * @brief   Print device information (placeholder for UART implementation)
 */
static void PrintDeviceInfo(uint8_t manufacturer_id, uint8_t device_id, uint16_t page_size) {
    // Assuming UART is available
    // printf("AT45DB Device Information:\n");
    // printf("  Manufacturer ID: 0x%02X\n", manufacturer_id);
    // printf("  Device ID: 0x%04X\n", device_id);
    // printf("  Page Size: %u bytes\n", page_size);
    // printf("  Total Pages: %u\n", AT45DB_TOTAL_PAGES);
    // printf("  Total Memory: %u KB\n", (AT45DB_TOTAL_PAGES * page_size) / 1024);
}

/**
 * @brief   Print buffer contents (placeholder for UART implementation)
 */
static void PrintBuffer(const char *label, uint8_t *buffer, uint32_t size) {
    // printf("\n%s:\n", label);
    // for (uint32_t i = 0; i < size; i++) {
    //     if (i % 16 == 0) {
    //         printf("\n0x%04X: ", i);
    //     }
    //     printf("%02X ", buffer[i]);
    // }
    // printf("\n");
}

/*============================================================================
 * Test Functions
 *============================================================================*/

/**
 * @brief   Test 1: Device Initialization and Identification
 */
static bool TestDeviceInit(void) {
    // Initialize AT45DB driver
    if (!AT45DB_Init(&at45db_handle, &spi_handle, AT45DB_CS_PORT, AT45DB_CS_PIN)) {
        return false;
    }

    // Check device presence
    uint8_t manufacturer_id, device_id;
    if (!AT45DB_ReadJEDECID(&at45db_handle, &manufacturer_id, &device_id)) {
        return false;
    }

    PrintDeviceInfo(manufacturer_id, device_id, AT45DB_GetPageSize(&at45db_handle));

    return true;
}

/**
 * @brief   Test 2: Buffer Operations
 */
static bool TestBufferOperations(void) {
    // Fill test buffer
    FillTestBuffer(tx_buffer, TEST_BUFFER_SIZE, 0x55);

    // Write to Buffer 1
    if (!AT45DB_WriteBuffer1(&at45db_handle, 0, tx_buffer, TEST_BUFFER_SIZE)) {
        return false;
    }

    // Read back from Buffer 1
    memset(rx_buffer, 0, sizeof(rx_buffer));
    if (!AT45DB_ReadBuffer1(&at45db_handle, 0, rx_buffer, TEST_BUFFER_SIZE)) {
        return false;
    }

    // Verify buffer contents
    if (!VerifyBuffers(tx_buffer, rx_buffer, TEST_BUFFER_SIZE)) {
        return false;
    }

    return true;
}

/**
 * @brief   Test 3: Page Operations
 */
static bool TestPageOperations(void) {
    // Fill test buffer
    FillTestBuffer(tx_buffer, TEST_BUFFER_SIZE, 0xAA);

    // Erase test page
    if (!AT45DB_ErasePage(&at45db_handle, TEST_PAGE_START)) {
        return false;
    }

    // Program Buffer 1 to test page
    if (!AT45DB_ProgramBuffer1(&at45db_handle, TEST_PAGE_START, 0, tx_buffer, TEST_BUFFER_SIZE)) {
        return false;
    }

    // Read back from page
    memset(rx_buffer, 0, sizeof(rx_buffer));
    if (!AT45DB_ReadPage(&at45db_handle, TEST_PAGE_START, 0, rx_buffer, TEST_BUFFER_SIZE)) {
        return false;
    }

    // Verify data
    if (!VerifyBuffers(tx_buffer, rx_buffer, TEST_BUFFER_SIZE)) {
        return false;
    }

    return true;
}

/**
 * @brief   Test 4: Continuous Read
 */
static bool TestContinuousRead(void) {
    uint32_t address = TEST_PAGE_START * AT45DB_GetPageSize(&at45db_handle);

    // Read continuously from memory
    memset(rx_buffer, 0, sizeof(rx_buffer));
    if (!AT45DB_ReadContinuous(&at45db_handle, address, rx_buffer, TEST_BUFFER_SIZE)) {
        return false;
    }

    // Verify data
    if (!VerifyBuffers(tx_buffer, rx_buffer, TEST_BUFFER_SIZE)) {
        return false;
    }

    return true;
}

/**
 * @brief   Test 5: Multi-Page Operations
 */
static bool TestMultiPageOperations(void) {
    for (uint16_t page = TEST_PAGE_START; page < TEST_PAGE_START + TEST_PAGE_COUNT; page++) {
        FillTestBuffer(tx_buffer, TEST_BUFFER_SIZE, page);

        if (!AT45DB_ErasePage(&at45db_handle, page)) {
            return false;
        }

        if (!AT45DB_ProgramBuffer2(&at45db_handle, page, 0, tx_buffer, TEST_BUFFER_SIZE)) {
            return false;
        }

        memset(rx_buffer, 0, sizeof(rx_buffer));
        if (!AT45DB_ReadPage(&at45db_handle, page, 0, rx_buffer, TEST_BUFFER_SIZE)) {
            return false;
        }

        if (!VerifyBuffers(tx_buffer, rx_buffer, TEST_BUFFER_SIZE)) {
            return false;
        }
    }

    return true;
}

/**
 * @brief   Test 6: Power Management
 */
static bool TestPowerManagement(void) {
    // Test Deep Power Down
    if (!AT45DB_EnterDeepPowerDown(&at45db_handle)) {
        return false;
    }

    // Wait for power down to complete
    for (volatile int i = 0; i < 100000; i++);

    // Resume from Deep Power Down
    if (!AT45DB_ResumeFromDeepPowerDown(&at45db_handle)) {
        return false;
    }

    // Wait for device to become ready
    if (!AT45DB_WaitReady(&at45db_handle, 1000)) {
        return false;
    }

    // Verify device is still accessible
    uint8_t status;
    if (!AT45DB_ReadStatus(&at45db_handle, &status)) {
        return false;
    }

    return true;
}

/*============================================================================
 * Main Application
 *============================================================================*/

int main(void) {
    // Initialize system
    SystemClock_Config();
    SPI_Config();
    GPIO_Config();

    // Run all tests
    bool test1 = TestDeviceInit();
    bool test2 = TestBufferOperations();
    bool test3 = TestPageOperations();
    bool test4 = TestContinuousRead();
    bool test5 = TestMultiPageOperations();
    bool test6 = TestPowerManagement();

    // If all tests passed, indicate success
    if (test1 && test2 && test3 && test4 && test5 && test6) {
        // Toggle LED or send success message
        while (1) {
            // Success loop
        }
    } else {
        // If any test failed, indicate error
        while (1) {
            // Error loop
        }
    }
}

/*============================================================================
 * Interrupt Handlers
 *============================================================================*/

void SPI1_IRQHandler(void) {
    // Handle SPI interrupt if needed
}

void HardFault_Handler(void) {
    while (1);
}

void NMI_Handler(void) {
    while (1);
}

void SVC_Handler(void) {
    while (1);
}

void PendSV_Handler(void) {
    while (1);
}

void SysTick_Handler(void) {
    // Implement SysTick timer for timeout calculations if needed
}
