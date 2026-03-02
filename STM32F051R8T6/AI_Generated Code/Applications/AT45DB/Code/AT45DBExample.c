/**
 * @file    AT45DBExample.c
 * @brief   Example usage of AT45DB041D/AT45DB041E DataFlash Driver
 * @author  AI Generated
 * @date    2026-02-28
 */

#include "AT45DB.h"
#include "../../DeepSeek_Generated/RCC/rcc.h"
#include "../../GPIO_AI/gpio.h"
#include "../../SPI/Code/spiF051.h"
#include <string.h>

/*============================================================================
 * Example Configuration
 *============================================================================*/
#define AT45DB_SPI_INSTANCE        SPI1
#define AT45DB_CS_PORT             GPIOA
#define AT45DB_CS_PIN              GPIO_PIN_4
#define TEST_PAGE_ADDRESS          0x0000 // First page
#define TEST_BUFFER_SIZE           64

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
    spi_handle.Init.baud_rate = SPI_BAUD_RATE_DIV_4; // 12MHz for 48MHz system clock
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
static void FillTestBuffer(void) {
    for (uint32_t i = 0; i < TEST_BUFFER_SIZE; i++) {
        tx_buffer[i] = (uint8_t)(i + 0x55);
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

/*============================================================================
 * Main Application
 *============================================================================*/

int main(void) {
    // Initialize system
    SystemClock_Config();
    SPI_Config();
    GPIO_Config();
    
    // Initialize AT45DB driver
    if (!AT45DB_Init(&at45db_handle, &spi_handle, AT45DB_CS_PORT, AT45DB_CS_PIN)) {
        // Handle initialization failure
        while (1);
    }
    
    // Check device presence
    uint8_t manufacturer_id, device_id;
    if (!AT45DB_ReadJEDECID(&at45db_handle, &manufacturer_id, &device_id)) {
        while (1);
    }
    
    // Print device information (assuming UART is available)
    // printf("Manufacturer ID: 0x%02X\n", manufacturer_id);
    // printf("Device ID: 0x%04X\n", device_id);
    // printf("Page Size: %u bytes\n", AT45DB_GetPageSize(&at45db_handle));
    
    // Fill test buffer
    FillTestBuffer();
    
    // Erase test page before writing
    if (!AT45DB_ErasePage(&at45db_handle, TEST_PAGE_ADDRESS / AT45DB_GetPageSize(&at45db_handle))) {
        while (1);
    }
    
    // Program buffer 1 to test page
    if (!AT45DB_ProgramBuffer1(&at45db_handle, 
                              TEST_PAGE_ADDRESS / AT45DB_GetPageSize(&at45db_handle),
                              TEST_PAGE_ADDRESS % AT45DB_GetPageSize(&at45db_handle),
                              tx_buffer, TEST_BUFFER_SIZE)) {
        while (1);
    }
    
    // Read back the data
    memset(rx_buffer, 0, sizeof(rx_buffer));
    if (!AT45DB_ReadContinuous(&at45db_handle, TEST_PAGE_ADDRESS, rx_buffer, TEST_BUFFER_SIZE)) {
        while (1);
    }
    
    // Verify data
    if (!VerifyBuffers(tx_buffer, rx_buffer, TEST_BUFFER_SIZE)) {
        // Handle verification failure
        while (1);
    }
    
    // If all tests passed, we should reach here
    // Toggle a LED or send a success message via UART
    
    while (1) {
        // Application loop
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
