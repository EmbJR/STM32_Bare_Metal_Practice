/**
 * @file    AT45DB_Example.c
 * @brief   AT45DB DataFlash Driver Example Code
 * @details Example demonstrating how to use the AT45DB DataFlash driver
 *          for memory read/write/erase operations
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* Include driver files */
#include "AT45DB.h"
#include "spiF051.h"
#include "gpio.h"
#include "rcc.h"

/*============================================================================
 * Configuration
 *============================================================================*/

/* SPI Configuration - using SPI1 */
#define AT45DB_SPI                SPI1

/* GPIO Configuration */
/* SCK: PA1 (check your hardware pinout)
   MISO: PA6
   MOSI: PA7
   CS: PA4 (GPIO output) */
#define AT45DB_GPIO_PORT          GPIOA
#define AT45DB_CS_PIN             GPIO_PIN_4
#define AT45DB_SCK_PIN            GPIO_PIN_1
#define AT45DB_MISO_PIN           GPIO_PIN_6
#define AT45DB_MOSI_PIN           GPIO_PIN_7

/*============================================================================
 * Function Prototypes
 *============================================================================*/
static void SystemClock_Config(void);
static void SPI_Config(void);
static void GPIO_Config(void);
static void AT45DB_Example_ReadWrite(void);
static void AT45DB_Example_Erase(void);
static void PrintBuffer(const char *label, uint8_t *buffer, uint32_t size);

/*============================================================================
 * Global Variables
 *============================================================================*/

/* AT45DB Handle */
AT45DB_HandleTypeDef at45db_handle;

/* Test data buffers */
uint8_t write_buffer[264];
uint8_t read_buffer[264];

/*============================================================================
 * Main Function
 *============================================================================*/

int main(void) {
    /* Configure system clock */
    SystemClock_Config();
    
    /* Configure GPIO */
    GPIO_Config();
    
    /* Configure SPI */
    SPI_Config();
    
    /* Initialize AT45DB handle */
    at45db_handle.SPIx = (void*)AT45DB_SPI;
    at45db_handle.CS_Port = (void*)AT45DB_GPIO_PORT;
    at45db_handle.CS_Pin = AT45DB_CS_PIN;
    
    /* Initialize AT45DB device */
    if (AT45DB_Init(&at45db_handle)) {
        /* Device initialized successfully */
        printf("AT45DB Initialized successfully\r\n");
        printf("Page Size: %d bytes\r\n", at45db_handle.PageSize);
        printf("Manufacturer ID: 0x%02X\r\n", at45db_handle.ManufacturerID);
        printf("Device ID: 0x%04X\r\n", at45db_handle.DeviceID);
    } else {
        /* Initialization failed */
        printf("AT45DB Initialization FAILED\r\n");
        while (1) {
            /* Hang here - error */
        }
    }
    
    /* Run examples */
    AT45DB_Example_ReadWrite();
    AT45DB_Example_Erase();
    
    /* Enter deep power-down (optional) */
    printf("\r\nEntering Deep Power-Down mode...\r\n");
    AT45DB_EnterDeepPowerDown(&at45db_handle);
    
    while (1) {
        /* Main loop - application code here */
    }
}

/*============================================================================
 * System and Peripheral Configuration
 *============================================================================*/

/**
 * @brief  Configure system clock
 */
static void SystemClock_Config(void) {
	RCC_Config rcc_config;
    /* Configure RCC for 48MHz using PLL with HSI */
    rcc_config.system_clock_source = CLOCK_SOURCE_PLL;
    rcc_config.target_frequency = SYSTEM_CLOCK_48MHZ;
    rcc_config.hse_enabled = false;
    rcc_config.pll_enabled = true;
    rcc_config.pll_source = PLL_SOURCE_HSI_DIV2;
    rcc_config.pll_multiplier = 12;  /* HSI (8MHz/2) * 12 = 48MHz */
    rcc_config.ahb_prescaler = AHB_PRESCALER_1;
    rcc_config.apb_prescaler = APB_PRESCALER_1;
    rcc_config.hsi48_enabled = false;
    rcc_config.css_enabled = false;

    /* Initialize RCC */
    RCC_Init(&rcc_config);
}

/**
 * @brief  Configure GPIO pins for SPI and CS
 */
static void GPIO_Config(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_EnableClock(GPIOA);

    /* Configure CS pin as output */
    GPIO_InitStruct.Pin = AT45DB_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_Init(AT45DB_GPIO_PORT, &GPIO_InitStruct);
    
    /* Set CS high (deselect) */
    GPIO_SetPin(AT45DB_GPIO_PORT, AT45DB_CS_PIN);
    
    /* Configure SCK pin (Alternate Function) */
    GPIO_InitStruct.Pin = AT45DB_SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_Init(AT45DB_GPIO_PORT, &GPIO_InitStruct);
    
    /* Configure MISO pin (Alternate Function) */
    GPIO_InitStruct.Pin = AT45DB_MISO_PIN;
    GPIO_Init(AT45DB_GPIO_PORT, &GPIO_InitStruct);
    
    /* Configure MOSI pin (Alternate Function) */
    GPIO_InitStruct.Pin = AT45DB_MOSI_PIN;
    GPIO_Init(AT45DB_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @brief  Configure SPI peripheral
 */
static void SPI_Config(void) {
    SPI_InitTypeDef SPI_InitStruct;
    
    /* SPI configuration:
       - Mode: Master
       - Clock Polarity: Low (CPOL = 0)
       - Clock Phase: 1 Edge (CPHA = 1) - for AT45DB use mode 0 or 3
       - Baud Rate: fPCLK/16 (adjust for your needs)
       - Data Size: 8-bit
       - Frame Format: MSB first
    */
    
    /* Use Mode 0: CPOL=0, CPHA=0 (clock idle low, data captured on rising edge) */
    /* Or Mode 3: CPOL=1, CPHA=1 (clock idle high, data captured on falling edge) */
    /* AT45DB supports both Mode 0 and Mode 3 */
    
    SPI_InitStruct.Mode = SPI_MODE_MASTER;
    SPI_InitStruct.ClockPolarity = SPI_CPOL_LOW;   /* Clock idle low */
    SPI_InitStruct.ClockPhase = SPI_CPHA_1EDGE;    /* Data captured on first edge */
    SPI_InitStruct.BaudRate = SPI_BAUDRATEPRESCALER_16;  /* Adjust as needed */
    SPI_InitStruct.FrameFormat = SPI_FRAME_FORMAT_MSBFIRST;
    SPI_InitStruct.DataSize = SPI_DATASIZE_8BIT;
    SPI_InitStruct.NSS = SPI_NSS_SOFT;  /* Software NSS control */
    SPI_InitStruct.FIFOThreshold = true;
    
    /* Initialize SPI */
    SPI_Init(AT45DB_SPI, &SPI_InitStruct);
    
    /* Enable SPI */
    SPI_Enable(AT45DB_SPI);
}

/*============================================================================
 * AT45DB Example Functions
 *============================================================================*/

/**
 * @brief  Example: Read and Write operations
 */
static void AT45DB_Example_ReadWrite(void) {
    uint16_t page_num = 0;
    uint16_t i;
    bool success;
    
    printf("\r\n=== AT45DB Read/Write Example ===\r\n");
    
    /* Prepare test data */
    for (i = 0; i < at45db_handle.PageSize; i++) {
        write_buffer[i] = (uint8_t)(i & 0xFF);
    }
    
    /* Method 1: Program through buffer (one-step operation with erase) */
    printf("Writing data to page %d using ProgramThroughBuffer...\r\n", page_num);
    success = AT45DB_ProgramThroughBuffer(&at45db_handle, page_num, 0, 
                                           write_buffer, at45db_handle.PageSize, true);
    if (success) {
        printf("Write successful!\r\n");
    } else {
        printf("Write FAILED!\r\n");
        return;
    }
    
    /* Clear read buffer */
    for (i = 0; i < at45db_handle.PageSize; i++) {
        read_buffer[i] = 0x00;
    }
    
    /* Read back the data using continuous read */
    printf("Reading data from page %d...\r\n", page_num);
    success = AT45DB_ContinuousRead(&at45db_handle, 
                                    (uint32_t)page_num * at45db_handle.PageSize,
                                    read_buffer, at45db_handle.PageSize);
    if (success) {
        printf("Read successful!\r\n");
    } else {
        printf("Read FAILED!\r\n");
        return;
    }
    
    /* Verify data */
    printf("Verifying data...\r\n");
    for (i = 0; i < at45db_handle.PageSize; i++) {
        if (read_buffer[i] != write_buffer[i]) {
            printf("Data mismatch at byte %d: wrote 0x%02X, read 0x%02X\r\n", 
                   i, write_buffer[i], read_buffer[i]);
            printf("Verification FAILED!\r\n");
            return;
        }
    }
    printf("Verification PASSED! Data matches.\r\n");
    
    /* Print first few bytes for verification */
    PrintBuffer("First 16 bytes written", write_buffer, 16);
    PrintBuffer("First 16 bytes read", read_buffer, 16);
}

/**
 * @brief  Example: Erase operations
 */
static void AT45DB_Example_Erase(void) {
    uint16_t page_num = 10;
    bool success;
    
    printf("\r\n=== AT45DB Erase Example ===\r\n");
    
    /* Erase a page */
    printf("Erasing page %d...\r\n", page_num);
    success = AT45DB_PageErase(&at45db_handle, page_num);
    if (success) {
        printf("Page erase successful!\r\n");
    } else {
        printf("Page erase FAILED!\r\n");
        return;
    }
    
    /* Read the erased page (should be 0xFF) */
    printf("Reading erased page...\r\n");
    success = AT45DB_PageRead(&at45db_handle, page_num, read_buffer);
    if (success) {
        printf("Read successful!\r\n");
    } else {
        printf("Read FAILED!\r\n");
        return;
    }
    
    /* Verify page is erased (all 0xFF) */
    printf("Verifying erase (checking for 0xFF)...\r\n");
    for (uint16_t i = 0; i < at45db_handle.PageSize; i++) {
        if (read_buffer[i] != 0xFF) {
            printf("Erase verification FAILED! Byte %d = 0x%02X\r\n", i, read_buffer[i]);
            return;
        }
    }
    printf("Erase verification PASSED! Page is erased (0xFF).\r\n");
    
    /* Block erase example */
    printf("\nErasing block 0 (pages 0-7)...\r\n");
    success = AT45DB_BlockErase(&at45db_handle, 0);
    if (success) {
        printf("Block erase successful!\r\n");
    } else {
        printf("Block erase FAILED!\r\n");
    }
}

/**
 * @brief  Print buffer contents (for debugging)
 */
static void PrintBuffer(const char *label, uint8_t *buffer, uint32_t size) {
    uint32_t i;
    
    printf("%s:\r\n  ", label);
    for (i = 0; i < size; i++) {
        printf("%02X ", buffer[i]);
        if ((i + 1) % 16 == 0 && i < size - 1) {
            printf("\r\n  ");
        }
    }
    printf("\r\n");
}

/*============================================================================
 * Optional: Implement printf functionality
 *============================================================================*/
/* 
 * Note: You'll need to implement a UART or use existing printf over SWD
 * For now, these are placeholder functions
 */

#ifdef __GNUC__
int __io_putchar(int ch) {
    /* Implement your UART output here */
    /* Example: UART_SendData(ch); */
    return ch;
}
#endif
