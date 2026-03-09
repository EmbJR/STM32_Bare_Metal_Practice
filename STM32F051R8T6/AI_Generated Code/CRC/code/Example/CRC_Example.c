/**
 * @file    CRC_Example.c
 * @brief   Example code demonstrating CRC driver usage for STM32F051R8T6
 * @details This example shows how to use the CRC driver library to calculate
 *          CRC-32, CRC-16-CCITT, and CRC-8 checksums.
 *
 * @note    This example assumes the CRC peripheral clock is enabled.
 *          In a real application, you would need to enable the CRC clock
 *          in the RCC peripheral. See below for details.
 *
 * @note    To use this code:
 *          1. Include crcF051.h in your project
 *          2. Add crcF051.c to your build
 *          3. Enable CRC clock in RCC (see CRC_EnableClock function below)
 *          4. Call the CRC functions as shown in the examples
 */

/*============================================================================
 * Includes
 *============================================================================*/
#include "crcF051.h"
#include <stdint.h>
#include <stdio.h>

/*============================================================================
 * Hardware Definitions
 *============================================================================*/
/* RCC Base Address */
#define RCC_BASE_ADDR         0x40021000U

/* RCC AHB peripheral clock enable register */
#define RCC_AHBENR_OFFSET     0x14U
#define RCC_AHBENR            (*(volatile uint32_t *)(RCC_BASE_ADDR + RCC_AHBENR_OFFSET))

/* CRC clock enable bit in RCC_AHBENR */
#define RCC_AHBENR_CRCEN      (1U << 6)

/*============================================================================
 * Function Prototypes
 *============================================================================*/
void CRC_EnableClock(void);
void Example_CRC32(void);
void Example_CRC16CCITT(void);
void Example_CRC8(void);
void Example_CustomCRC(void);

/*============================================================================
 * Main Function
 *============================================================================*/
int main(void)
{
    /* Enable CRC peripheral clock */
    CRC_EnableClock();
    
    /* Run all CRC examples */
    Example_CRC32();
    Example_CRC16CCITT();
    Example_CRC8();
    Example_CustomCRC();
    
    /* Infinite loop */
    while (1)
    {
        /* Add your application code here */
    }
    
    return 0;
}

/*============================================================================
 * CRC Clock Enable Function
 *============================================================================*/
/**
 * @brief  Enable CRC peripheral clock
 * @retval None
 * @note   This function enables the CRC clock in RCC AHBENR register
 */
void CRC_EnableClock(void)
{
    /* Set CRCEN bit to enable CRC clock */
    RCC_AHBENR |= RCC_AHBENR_CRCEN;
}

/*============================================================================
 * Example 1: CRC-32 (Ethernet/USB)
 *============================================================================*/
/**
 * @brief  Example: Calculate CRC-32 checksum
 * @retval None
 * @note   CRC-32 is commonly used in Ethernet and ZIP files
 *         Polynomial: 0x04C11DB7
 *         Initial value: 0xFFFFFFFF
 *         Final XOR: 0xFFFFFFFF
 */
void Example_CRC32(void)
{
    /* Example data: "123456789" */
    uint8_t data[] = "123456789";
    
    /* Configure CRC for CRC-32 mode */
    CRC_ConfigCRC32();
    
    /* Feed data to CRC calculator */
    CRC_FeedBuffer(data, sizeof(data) - 1);  /* -1 to exclude null terminator */
    
    /* Get CRC result */
    uint32_t crcResult = CRC_GetResult();
    
    /* For CRC-32, typically XOR with 0xFFFFFFFF for standard result */
    crcResult ^= 0xFFFFFFFF;
    
    /* Expected CRC-32 of "123456789" is: 0xCBF43926 */
    /* After XOR with 0xFFFFFFFF: 0x342178D9 */
    (void)crcResult;  /* Prevent unused variable warning */
}

/*============================================================================
 * Example 2: CRC-16-CCITT
 *============================================================================*/
/**
 * @brief  Example: Calculate CRC-16-CCITT checksum
 * @retval None
 * @note   CRC-16-CCITT is commonly used in X.25, Bluetooth, and SD cards
 *         Polynomial: 0x1021
 *         Initial value: 0xFFFF
 */
void Example_CRC16CCITT(void)
{
    /* Example data: "123456789" */
    uint8_t data[] = "123456789";
    
    /* Configure CRC for CRC-16-CCITT mode */
    CRC_ConfigCRC16CCITT();
    
    /* Feed data byte by byte */
    for (uint32_t i = 0; i < sizeof(data) - 1; i++)
    {
        CRC_FeedData8(data[i]);
    }
    
    /* Get CRC result (lower 16 bits) */
    uint16_t crcResult = (uint16_t)(CRC_GetResult() & 0xFFFF);
    
    /* Expected CRC-16-CCITT of "123456789" with init=0xFFFF is: 0x29B1 */
    (void)crcResult;  /* Prevent unused variable warning */
}

/*============================================================================
 * Example 3: CRC-8 (I2C)
 *============================================================================*/
/**
 * @brief  Example: Calculate CRC-8 checksum
 * @retval None
 * @note   CRC-8 is used in I2C protocol and SMBus
 *         Polynomial: 0x07
 *         Initial value: 0x00
 */
void Example_CRC8(void)
{
    /* Example data: I2C slave address + data */
    uint8_t data[] = {0xA0, 0x00, 0x01, 0x02, 0x03};
    
    /* Configure CRC for CRC-8 mode */
    CRC_ConfigCRC8();
    
    /* Feed data as 32-bit word first, then remaining bytes */
    /* In this case, let's feed byte by byte for clarity */
    for (uint32_t i = 0; i < sizeof(data); i++)
    {
        CRC_FeedData8(data[i]);
    }
    
    /* Get CRC result (lower 8 bits) */
    uint8_t crcResult = (uint8_t)(CRC_GetResult() & 0xFF);
    
    /* Example result for I2C usage */
    (void)crcResult;  /* Prevent unused variable warning */
}

/*============================================================================
 * Example 4: Custom CRC Configuration
 *============================================================================*/
/**
 * @brief  Example: Calculate custom CRC with user-defined parameters
 * @retval None
 * @note   This shows how to configure CRC with custom polynomial and settings
 */
void Example_CustomCRC(void)
{
    /* Example data */
    uint8_t data[] = "Hello World";
    
    /* Custom CRC configuration */
    CRC_Config_t customConfig;
    
    /* Set custom polynomial (example: CRC-7 for MMC/SD cards) */
    customConfig.polynomial = 0x09;         /* CRC-7 polynomial */
    customConfig.initialValue = 0x00;       /* Initial value */
    customConfig.polynomialSize = CRC_POLY_SIZE_7;
    customConfig.reverseInput = CRC_REV_IN_NONE;
    customConfig.reverseOutput = 0;
    
    /* Initialize CRC with custom configuration */
    CRC_Init(&customConfig);
    
    /* Feed data to CRC calculator */
    CRC_FeedBuffer(data, sizeof(data) - 1);
    
    /* Get CRC result */
    uint32_t crcResult = CRC_GetResult();
    
    /* Extract lower 8 bits for CRC-7 result */
    uint8_t crc7Result = (uint8_t)(crcResult & 0x7F);
    
    (void)crc7Result;  /* Prevent unused variable warning */
}

/*============================================================================
 * Additional Usage Examples (for reference)
 *============================================================================*/

/**
 * @brief  Example: Using IDR for temporary storage
 * @retval None
 * @note   The Independent Data Register (IDR) can be used to store
 *         temporary data that doesn't participate in CRC calculation
 */
void Example_IDRUsage(void)
{
    /* Store a value in IDR */
    CRC_WriteIDR(0x5A);
    
    /* Read value from IDR */
    uint8_t idrValue = CRC_ReadIDR();
    
    (void)idrValue;  /* Prevent unused variable warning */
}

/**
 * @brief  Example: Resetting CRC for new calculation
 * @retval None
 * @note   Use CRC_Reset() to clear current calculation and start fresh
 */
void Example_ResetUsage(void)
{
    uint8_t data[] = "First";
    
    /* Calculate CRC for first data */
    CRC_ConfigCRC32();
    CRC_FeedBuffer(data, sizeof(data) - 1);
    uint32_t crc1 = CRC_GetResult();
    
    /* Reset CRC for new calculation */
    CRC_Reset();
    
    /* Calculate CRC for new data */
    uint8_t data2[] = "Second";
    CRC_FeedBuffer(data2, sizeof(data2) - 1);
    uint32_t crc2 = CRC_GetResult();
    
    (void)crc1;
    (void)crc2;
}

/**
 * @brief  Example: Incremental CRC calculation
 * @retval None
 * @note   CRC can be calculated incrementally by feeding data in chunks
 */
void Example_IncrementalCRC(void)
{
    /* Initialize CRC */
    CRC_ConfigCRC32();
    
    /* Feed data in multiple chunks */
    uint8_t chunk1[] = "Part1";
    uint8_t chunk2[] = "Part2";
    uint8_t chunk3[] = "Part3";
    
    CRC_FeedBuffer(chunk1, sizeof(chunk1) - 1);
    CRC_FeedBuffer(chunk2, sizeof(chunk2) - 1);
    CRC_FeedBuffer(chunk3, sizeof(chunk3) - 1);
    
    /* Get final CRC result */
    uint32_t finalCRC = CRC_GetResult();
    
    (void)finalCRC;
}
