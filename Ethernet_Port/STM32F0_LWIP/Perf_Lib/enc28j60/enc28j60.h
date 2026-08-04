#ifndef ENC28J60_H
#define ENC28J60_H

#include "spiF051.h"

/* SPI Configuration - using SPI1 */
#define ESP28J60_SPI                SPI2

/* GPIO Configuration */
/* SCK: PA1 (check your hardware pinout)
   MISO: PA6
   MOSI: PA7
   CS: PA4 (GPIO output) */
#define ESP28J60_GPIO_PORT          GPIOB
#define ESP28J60_CS_PIN             GPIO_PIN_12
#define ESP28J60_SCK_PIN            GPIO_PIN_13
#define ESP28J60_MISO_PIN           GPIO_PIN_14
#define ESP28J60_MOSI_PIN           GPIO_PIN_15

//-------- Registers -------//
#define ECON1   0x1f

//-------- config -------//
#define BANK0   0
#define BANK1   1
#define BANK2   2
#define BANK3   3

/* CS Pin control macros - assuming CS is on GPIO port B, pin 12 (common for SPI1) */
#define ESP28J60_CS_LOW(handle)     GPIO_ResetPin((GPIO_TypeDef*)handle->CS_Port, handle->CS_Pin)
#define ESP28J60_CS_HIGH(handle)     GPIO_SetPin((GPIO_TypeDef*)handle->CS_Port, handle->CS_Pin)

/*============================================================================
 * ESP28J60 Handle Structure
 *============================================================================*/
typedef struct {
    void *SPIx;                  /* SPI peripheral (SPI1 or SPI2) */
    void *CS_Port;              /* Chip select GPIO port */
    uint16_t CS_Pin;            /* Chip select GPIO pin */
    /*Extra configuration define here*/
} ESP28J60_HandleTypeDef;

static void ESP28J60_CSelect(ESP28J60_HandleTypeDef *handle);
static void ESP28J60_CDeselect(ESP28J60_HandleTypeDef *handle);

void enc28j60_init(void);
bool enc28j60_test(void);

#endif
