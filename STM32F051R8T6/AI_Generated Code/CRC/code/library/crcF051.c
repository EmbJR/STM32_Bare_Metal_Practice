/**
 * @file    crcF051.c
 * @brief   CRC Driver Implementation for STM32F051R8T6
 * @details This file contains the implementation of CRC hardware driver.
 *          It is independent of HAL or CMSIS libraries.
 */

#include "crcF051.h"

/*============================================================================
 * Local Functions
 *============================================================================*/

/**
 * @brief  Configure CRC control register
 * @param  polySize: Polynomial size
 * @param  revIn: Input reversal mode
 * @param  revOut: Output reversal enable
 * @retval None
 */
static void CRC_ConfigureControl(uint8_t polySize, uint8_t revIn, uint8_t revOut)
{
    uint32_t cr = 0;
    
    /* Set polynomial size */
    cr |= (polySize << 3);
    
    /* Set input reversal */
    cr |= (revIn << 5);
    
    /* Set output reversal */
    if (revOut != 0)
    {
        cr |= CRC_CR_REV_OUT;
    }
    
    /* Write to control register (keep RESET bit cleared) */
    CRC_PTR->CR = cr;
}

/*============================================================================
 * API Functions Implementation
 *============================================================================*/

/**
 * @brief  Initialize CRC peripheral with given configuration
 * @param  config: Pointer to CRC configuration structure
 * @retval None
 */
void CRC_Init(CRC_Config_t *config)
{
    /* Reset CRC unit first */
    CRC_Reset();
    
    /* Set polynomial */
    CRC_SetPolynomial(config->polynomial);
    
    /* Set initial value */
    CRC_SetInitialValue(config->initialValue);
    
    /* Configure control register */
    CRC_ConfigureControl(config->polynomialSize, 
                         config->reverseInput, 
                         config->reverseOutput);
}

/**
 * @brief  Reset CRC calculation unit to initial values
 * @retval None
 */
void CRC_Reset(void)
{
    /* Set RESET bit to reset CRC unit */
    CRC_PTR->CR = CRC_CR_RESET;
    
    /* Clear RESET bit (hardware clears it automatically, but ensure) */
    CRC_PTR->CR = 0;
}

/**
 * @brief  Feed data to CRC calculator (32-bit)
 * @param  data: 32-bit data to calculate CRC
 * @retval None
 */
void CRC_FeedData32(uint32_t data)
{
    CRC_PTR->DR = data;
}

/**
 * @brief  Feed data to CRC calculator (16-bit)
 * @param  data: 16-bit data to calculate CRC
 * @retval None
 */
void CRC_FeedData16(uint16_t data)
{
    /* CRC peripheral processes 32-bit at a time */
    /* We write 16-bit and the hardware handles it */
    volatile uint8_t *data8ptr = (volatile uint8_t *)&CRC_PTR->DR;
    
    /* Write lower 8 bits first (depending on configuration) */
    data8ptr[0] = (uint8_t)(data & 0xFF);
    data8ptr[1] = (uint8_t)((data >> 8) & 0xFF);
}

/**
 * @brief  Feed data to CRC calculator (8-bit)
 * @param  data: 8-bit data to calculate CRC
 * @retval None
 */
void CRC_FeedData8(uint8_t data)
{
    /* Write 8-bit to data register */
    volatile uint8_t *data8ptr = (volatile uint8_t *)&CRC_PTR->DR;
    data8ptr[0] = data;
}

/**
 * @brief  Feed buffer of data to CRC calculator
 * @param  buffer: Pointer to data buffer
 * @param  length: Number of bytes to process
 * @retval None
 */
void CRC_FeedBuffer(const uint8_t *buffer, uint32_t length)
{
    uint32_t i;
    uint32_t word;
    
    /* Process 32-bit words when possible */
    for (i = 0; i <= (length - 4); i += 4)
    {
        word = ((uint32_t)buffer[i+3] << 24) |
               ((uint32_t)buffer[i+2] << 16) |
               ((uint32_t)buffer[i+1] << 8)  |
               ((uint32_t)buffer[i]);
        CRC_FeedData32(word);
    }
    
    /* Process remaining bytes */
    for (; i < length; i++)
    {
        CRC_FeedData8(buffer[i]);
    }
}

/**
 * @brief  Get current CRC result
 * @retval Current CRC value
 */
uint32_t CRC_GetResult(void)
{
    return CRC_PTR->DR;
}

/**
 * @brief  Set CRC initial value
 * @param  initValue: Initial CRC value
 * @retval None
 */
void CRC_SetInitialValue(uint32_t initValue)
{
    CRC_PTR->INIT = initValue;
}

/**
 * @brief  Set CRC polynomial
 * @param  polynomial: Polynomial value
 * @retval None
 */
void CRC_SetPolynomial(uint32_t polynomial)
{
    CRC_PTR->POL = polynomial;
}

/**
 * @brief  Configure polynomial size
 * @param  size: Polynomial size (CRC_POLY_SIZE_7, 8, 16, or 32)
 * @retval None
 */
void CRC_SetPolynomialSize(CRC_PolySize_t size)
{
    uint32_t cr = CRC_PTR->CR;
    
    /* Clear polynomial size bits */
    cr &= ~(0x03 << 3);
    
    /* Set new polynomial size */
    cr |= ((uint32_t)size << 3);
    
    CRC_PTR->CR = cr;
}

/**
 * @brief  Configure input reversal
 * @param  revIn: Input reversal mode
 * @retval None
 */
void CRC_SetInputReverse(CRC_RevIn_t revIn)
{
    uint32_t cr = CRC_PTR->CR;
    
    /* Clear reverse input bits */
    cr &= ~(0x03 << 5);
    
    /* Set new reverse input mode */
    cr |= ((uint32_t)revIn << 5);
    
    CRC_PTR->CR = cr;
}

/**
 * @brief  Configure output reversal
 * @param  revOut: Enable (1) or disable (0) output reversal
 * @retval None
 */
void CRC_SetOutputReverse(uint8_t revOut)
{
    uint32_t cr = CRC_PTR->CR;
    
    if (revOut != 0)
    {
        cr |= CRC_CR_REV_OUT;
    }
    else
    {
        cr &= ~CRC_CR_REV_OUT;
    }
    
    CRC_PTR->CR = cr;
}

/**
 * @brief  Store data in Independent Data Register (IDR)
 * @param  data: Data to store
 * @retval None
 */
void CRC_WriteIDR(uint8_t data)
{
    CRC_PTR->IDR = data;
}

/**
 * @brief  Read data from Independent Data Register (IDR)
 * @retval Data stored in IDR
 */
uint8_t CRC_ReadIDR(void)
{
    return (uint8_t)(CRC_PTR->IDR & 0xFF);
}

/**
 * @brief  Configure CRC for CRC-32 (Ethernet) mode
 * @retval None
 */
void CRC_ConfigCRC32(void)
{
    CRC_Config_t config;
    
    /* Reset CRC unit */
    CRC_Reset();
    
    /* Configure for CRC-32 */
    config.polynomial = CRC_POLY_32;
    config.initialValue = CRC_INIT_32;
    config.polynomialSize = CRC_POLY_SIZE_32;
    config.reverseInput = CRC_REV_IN_BYTE;
    config.reverseOutput = 1;  /* Enable reverse output for CRC-32 */
    
    /* Apply configuration */
    CRC_Init(&config);
}

/**
 * @brief  Configure CRC for CRC-16-CCITT mode
 * @retval None
 */
void CRC_ConfigCRC16CCITT(void)
{
    CRC_Config_t config;
    
    /* Reset CRC unit */
    CRC_Reset();
    
    /* Configure for CRC-16-CCITT */
    config.polynomial = CRC_POLY_16_CCITT;
    config.initialValue = CRC_INIT_16_CCITT;
    config.polynomialSize = CRC_POLY_SIZE_16;
    config.reverseInput = CRC_REV_IN_BYTE;
    config.reverseOutput = 0;
    
    /* Apply configuration */
    CRC_Init(&config);
}

/**
 * @brief  Configure CRC for CRC-8 (I2C) mode
 * @retval None
 */
void CRC_ConfigCRC8(void)
{
    CRC_Config_t config;
    
    /* Reset CRC unit */
    CRC_Reset();
    
    /* Configure for CRC-8 */
    config.polynomial = CRC_POLY_8_I2C;
    config.initialValue = CRC_INIT_8;
    config.polynomialSize = CRC_POLY_SIZE_8;
    config.reverseInput = CRC_REV_IN_NONE;
    config.reverseOutput = 0;
    
    /* Apply configuration */
    CRC_Init(&config);
}
