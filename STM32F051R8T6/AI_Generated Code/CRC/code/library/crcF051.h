/**
 * @file    crcF051.h
 * @brief   CRC Driver for STM32F051R8T6
 * @details This driver provides a simple API for CRC hardware calculations.
 *          It is independent of HAL or CMSIS libraries.
 *
 * @note    The CRC peripheral supports:
 *          - CRC-32 (Ethernet/USB) polynomial: 0x04C11DB7
 *          - CRC-16 (CCITT) polynomial: 0x1021
 *          - CRC-8 (I2C) polynomial: 0x07
 *          - Configurable polynomial and initial value
 */

#ifndef __CRCF051_H
#define __CRCF051_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 * Includes
 *============================================================================*/
#include <stdint.h>

/*============================================================================
 * Base Address Definition
 *============================================================================*/
/* CRC Peripheral Base Address for STM32F051 */
#define CRC_BASE_ADDR         0x40022000U

/*============================================================================
 * Register Definitions
 *============================================================================*/
/* CRC Register Structure */
typedef struct {
    volatile uint32_t DR;       /*!< Data Register, offset: 0x00 */
    volatile uint32_t IDR;      /*!< Independent Data Register, offset: 0x04 */
    volatile uint32_t CR;       /*!< Control Register, offset: 0x08 */
    volatile uint32_t RESERVED; /*!< Reserved, offset: 0x0C */
    volatile uint32_t INIT;     /*!< Initial Value Register, offset: 0x10 */
    volatile uint32_t POL;      /*!< Polynomial Register, offset: 0x14 */
} CRC_RegDef_t;

/* CRC Peripheral Pointer */
#define CRC_PTR               ((CRC_RegDef_t *)CRC_BASE_ADDR)

/*============================================================================
 * CRC Control Register Bits
 *============================================================================*/
#define CRC_CR_RESET          (1U << 0)  /*!< RESET bit - Reset CRC calculation unit */
#define CRC_CR_POLYSIZE_32    (0U << 3)  /*!< POLYSIZE[1:0] - 32-bit polynomial */
#define CRC_CR_POLYSIZE_16    (1U << 3)  /*!< POLYSIZE[1:0] - 16-bit polynomial */
#define CRC_CR_POLYSIZE_8     (2U << 3)  /*!< POLYSIZE[1:0] - 8-bit polynomial */
#define CRC_CR_POLYSIZE_7     (3U << 3)  /*!< POLYSIZE[1:0] - 7-bit polynomial */
#define CRC_CR_REV_IN_BYTE    (1U << 5)  /*!< REV_IN - Byte reverse input */
#define CRC_CR_REV_IN_HALFWORD (2U << 5) /*!< REV_IN - Halfword reverse input */
#define CRC_CR_REV_IN_WORD    (3U << 5)  /*!< REV_IN - Word reverse input */
#define CRC_CR_REV_OUT        (1U << 7)  /*!< REV_OUT - Reverse output bit order */

/*============================================================================
 * CRC Standard Polynomials
 *============================================================================*/
#define CRC_POLY_32           0x04C11DB7U  /*!< CRC-32 (Ethernet/USB) polynomial */
#define CRC_POLY_16_CCITT     0x00001021U  /*!< CRC-16-CCITT polynomial */
#define CRC_POLY_8_I2C        0x00000007U  /*!< CRC-8 (I2C) polynomial */
#define CRC_POLY_7            0x00000009U  /*!< CRC-7 polynomial */

/*============================================================================
 * CRC Default Initial Values
 *============================================================================*/
#define CRC_INIT_32           0xFFFFFFFFU  /*!< CRC-32 default initial value */
#define CRC_INIT_16_CCITT     0x0000FFFFU  /*!< CRC-16-CCITT default initial value */
#define CRC_INIT_8            0x00000000U  /*!< CRC-8 default initial value */

/*============================================================================
 * CRC Configuration Structure
 *============================================================================*/
typedef struct {
    uint32_t polynomial;     /*!< Polynomial value */
    uint32_t initialValue;  /*!< Initial CRC value */
    uint8_t polynomialSize; /*!< Polynomial size: 7, 8, 16, or 32 bits */
    uint8_t reverseInput;   /*!< 0: No reverse, 1: Byte, 2: Halfword, 3: Word */
    uint8_t reverseOutput;  /*!< 0: No reverse, 1: Reverse output */
} CRC_Config_t;

/*============================================================================
 * CRC Polynominal Size Options
 *============================================================================*/
typedef enum {
    CRC_POLY_SIZE_32 = 0,   /*!< 32-bit polynomial */
    CRC_POLY_SIZE_16 = 1,   /*!< 16-bit polynomial */
    CRC_POLY_SIZE_8  = 2,   /*!< 8-bit polynomial */
    CRC_POLY_SIZE_7  = 3    /*!< 7-bit polynomial */
} CRC_PolySize_t;

/*============================================================================
 * CRC Input Reverse Options
 *============================================================================*/
typedef enum {
    CRC_REV_IN_NONE     = 0,   /*!< No input reversal */
    CRC_REV_IN_BYTE     = 1,   /*!< Reverse bits within each byte */
    CRC_REV_IN_HALFWORD = 2,   /*!< Reverse bytes within each halfword */
    CRC_REV_IN_WORD     = 3    /*!< Reverse bytes within each word */
} CRC_RevIn_t;

/*============================================================================
 * API Functions
 *============================================================================*/

/**
 * @brief  Initialize CRC peripheral with given configuration
 * @param  config: Pointer to CRC configuration structure
 * @retval None
 */
void CRC_Init(CRC_Config_t *config);

/**
 * @brief  Reset CRC calculation unit to initial values
 * @retval None
 */
void CRC_Reset(void);

/**
 * @brief  Feed data to CRC calculator (32-bit)
 * @param  data: 32-bit data to calculate CRC
 * @retval None
 */
void CRC_FeedData32(uint32_t data);

/**
 * @brief  Feed data to CRC calculator (16-bit)
 * @param  data: 16-bit data to calculate CRC
 * @retval None
 */
void CRC_FeedData16(uint16_t data);

/**
 * @brief  Feed data to CRC calculator (8-bit)
 * @param  data: 8-bit data to calculate CRC
 * @retval None
 */
void CRC_FeedData8(uint8_t data);

/**
 * @brief  Feed buffer of data to CRC calculator
 * @param  buffer: Pointer to data buffer
 * @param  length: Number of bytes to process
 * @retval None
 */
void CRC_FeedBuffer(const uint8_t *buffer, uint32_t length);

/**
 * @brief  Get current CRC result
 * @retval Current CRC value
 */
uint32_t CRC_GetResult(void);

/**
 * @brief  Set CRC initial value
 * @param  initValue: Initial CRC value
 * @retval None
 */
void CRC_SetInitialValue(uint32_t initValue);

/**
 * @brief  Set CRC polynomial
 * @param  polynomial: Polynomial value
 * @retval None
 */
void CRC_SetPolynomial(uint32_t polynomial);

/**
 * @brief  Configure polynomial size
 * @param  size: Polynomial size (CRC_POLY_SIZE_7, 8, 16, or 32)
 * @retval None
 */
void CRC_SetPolynomialSize(CRC_PolySize_t size);

/**
 * @brief  Configure input reversal
 * @param  revIn: Input reversal mode
 * @retval None
 */
void CRC_SetInputReverse(CRC_RevIn_t revIn);

/**
 * @brief  Configure output reversal
 * @param  revOut: Enable (1) or disable (0) output reversal
 * @retval None
 */
void CRC_SetOutputReverse(uint8_t revOut);

/**
 * @brief  Store data in Independent Data Register (IDR)
 * @param  data: Data to store
 * @retval None
 */
void CRC_WriteIDR(uint8_t data);

/**
 * @brief  Read data from Independent Data Register (IDR)
 * @retval Data stored in IDR
 */
uint8_t CRC_ReadIDR(void);

/**
 * @brief  Configure CRC for CRC-32 (Ethernet) mode
 * @retval None
 */
void CRC_ConfigCRC32(void);

/**
 * @brief  Configure CRC for CRC-16-CCITT mode
 * @retval None
 */
void CRC_ConfigCRC16CCITT(void);

/**
 * @brief  Configure CRC for CRC-8 (I2C) mode
 * @retval None
 */
void CRC_ConfigCRC8(void);

#ifdef __cplusplus
}
#endif

#endif /* __CRCF051_H */
