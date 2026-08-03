/**
 * @file    gpio.h
 * @brief   GPIO Driver for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-02-09
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/*============================================================================
 * GPIO Base Addresses
 *============================================================================*/
#define GPIOA_BASE    (0x48000000UL)
#define GPIOB_BASE    (0x48000400UL)
#define GPIOC_BASE    (0x48000800UL)
#define GPIOD_BASE    (0x48000C00UL)
#define GPIOE_BASE    (0x48001000UL)
#define GPIOF_BASE    (0x48001400UL)

/*============================================================================
 * GPIO Register Structure
 *============================================================================*/
typedef struct {
    volatile uint32_t MODER;   /*!< Mode Register,         Address offset: 0x00 */
    volatile uint32_t OTYPER;  /*!< Output Type Register,  Address offset: 0x04 */
    volatile uint32_t OSPEEDR; /*!< Output Speed Register, Address offset: 0x08 */
    volatile uint32_t PUPDR;   /*!< Pull-up/Pull-down Reg,  Address offset: 0x0C */
    volatile uint32_t IDR;     /*!< Input Data Register,    Address offset: 0x10 */
    volatile uint32_t ODR;     /*!< Output Data Register,    Address offset: 0x14 */
    volatile uint32_t BSRR;    /*!< Bit Set/Reset Register,  Address offset: 0x18 */
    volatile uint32_t LCKR;    /*!< Configuration Lock Reg,  Address offset: 0x1C */
    volatile uint32_t AFR[2];  /*!< Alternate Function Reg,  Address offset: 0x20-0x24 */
    volatile uint32_t BRR;     /*!< Bit Reset Register,      Address offset: 0x28 */
} GPIO_TypeDef;

/* GPIO Port Macros */
#define GPIOA    ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB    ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC    ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD    ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE    ((GPIO_TypeDef *)GPIOE_BASE)
#define GPIOF    ((GPIO_TypeDef *)GPIOF_BASE)

/*============================================================================
 * GPIO Pin Definitions
 *============================================================================*/
#define GPIO_PIN_0      (0x0001UL)
#define GPIO_PIN_1      (0x0002UL)
#define GPIO_PIN_2      (0x0004UL)
#define GPIO_PIN_3      (0x0008UL)
#define GPIO_PIN_4      (0x0010UL)
#define GPIO_PIN_5      (0x0020UL)
#define GPIO_PIN_6      (0x0040UL)
#define GPIO_PIN_7      (0x0080UL)
#define GPIO_PIN_8      (0x0100UL)
#define GPIO_PIN_9      (0x0200UL)
#define GPIO_PIN_10     (0x0400UL)
#define GPIO_PIN_11     (0x0800UL)
#define GPIO_PIN_12     (0x1000UL)
#define GPIO_PIN_13     (0x2000UL)
#define GPIO_PIN_14     (0x4000UL)
#define GPIO_PIN_15     (0x8000UL)
#define GPIO_PIN_ALL    (0xFFFFUL)

/* LCKR Register */
#define GPIO_LCKR_LCKK  (0x00010000UL)  /*!< Lock key */

/*============================================================================
 * GPIO Mode Definitions
 *============================================================================*/
typedef enum {
    GPIO_MODE_INPUT     = 0x00,  /*!< Input mode */
    GPIO_MODE_OUTPUT    = 0x01,  /*!< General purpose output mode */
    GPIO_MODE_ALTERNATE = 0x02,  /*!< Alternate function mode */
    GPIO_MODE_ANALOG    = 0x03   /*!< Analog mode */
} GPIO_ModeTypeDef;

/*============================================================================
 * GPIO Output Type Definitions
 *============================================================================*/
typedef enum {
    GPIO_OTYPE_PP = 0x00,  /*!< Push-pull */
    GPIO_OTYPE_OD = 0x01   /*!< Open-drain */
} GPIO_OTypeTypeDef;

/*============================================================================
 * GPIO Speed Definitions
 *============================================================================*/
typedef enum {
    GPIO_SPEED_LOW    = 0x00,  /*!< Low speed (2 MHz) */
    GPIO_SPEED_MEDIUM = 0x01,  /*!< Medium speed (10 MHz) */
    GPIO_SPEED_HIGH   = 0x03   /*!< High speed (50 MHz) */
} GPIO_SpeedTypeDef;

/*============================================================================
 * GPIO Pull-up/Pull-down Definitions
 *============================================================================*/
typedef enum {
    GPIO_PULL_NO   = 0x00,  /*!< No pull-up/pull-down */
    GPIO_PULL_UP   = 0x01,  /*!< Pull-up */
    GPIO_PULL_DOWN = 0x02   /*!< Pull-down */
} GPIO_PullTypeDef;

/*============================================================================
 * GPIO Alternate Function Definitions
 *============================================================================*/
typedef enum {
    GPIO_AF0  = 0x00,  /*!< Alternate function 0 */
    GPIO_AF1  = 0x01,  /*!< Alternate function 1 */
    GPIO_AF2  = 0x02,  /*!< Alternate function 2 */
    GPIO_AF3  = 0x03,  /*!< Alternate function 3 */
    GPIO_AF4  = 0x04,  /*!< Alternate function 4 */
    GPIO_AF5  = 0x05,  /*!< Alternate function 5 */
    GPIO_AF6  = 0x06,  /*!< Alternate function 6 */
    GPIO_AF7  = 0x07   /*!< Alternate function 7 */
} GPIO_AFTypeDef;

/*============================================================================
 * GPIO Configuration Structure
 *============================================================================*/
typedef struct {
    uint32_t Pin;           /*!< Specifies the GPIO pins to configure */
    GPIO_ModeTypeDef Mode;  /*!< Specifies the operating mode for the selected pins */
    GPIO_OTypeTypeDef Ot;   /*!< Specifies the output type for the selected pins */
    GPIO_SpeedTypeDef Speed;/*!< Specifies the speed for the selected pins */
    GPIO_PullTypeDef Pull;  /*!< Specifies the pull-up or pull-down activation */
    GPIO_AFTypeDef AF;     /*!< Specifies the alternate function for the selected pins */
} GPIO_InitTypeDef;


/*============================================================================
 * RCC Clock Enable/Disable for GPIO Ports
 *============================================================================*/
#define RCC_AHBENR_IOPAEN  (0x00020000UL)  /*!< GPIOA clock enable */
#define RCC_AHBENR_IOPBEN  (0x00040000UL)  /*!< GPIOB clock enable */
#define RCC_AHBENR_IOPCEN  (0x00080000UL)  /*!< GPIOC clock enable */
#define RCC_AHBENR_IOPDEN  (0x00100000UL)  /*!< GPIOD clock enable */
#define RCC_AHBENR_IOPEEN  (0x00200000UL)  /*!< GPIOE clock enable */
#define RCC_AHBENR_IOPFEN  (0x00400000UL)  /*!< GPIOF clock enable */

// #define RCC_BASE           (0x40021000UL)
// #define RCC_AHBENR         (*(volatile uint32_t *)(RCC_BASE + 0x14))

/*============================================================================
 * GPIO Driver API Prototypes
 *============================================================================*/

/* Initialization and Configuration */
void GPIO_DeInit(GPIO_TypeDef *GPIOx);
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_Init);

/* Pin Operations */
void GPIO_SetPin(GPIO_TypeDef *GPIOx, uint32_t Pin);
void GPIO_ResetPin(GPIO_TypeDef *GPIOx, uint32_t Pin);
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t Pin);
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint32_t Pin);
uint16_t GPIO_ReadPort(GPIO_TypeDef *GPIOx);
void GPIO_WritePort(GPIO_TypeDef *GPIOx, uint16_t PortVal);

/* Clock Control */
void GPIO_EnableClock(GPIO_TypeDef *GPIOx);
void GPIO_DisableClock(GPIO_TypeDef *GPIOx);

/* Locking */
void GPIO_LockPin(GPIO_TypeDef *GPIOx, uint32_t Pin);
uint8_t GPIO_IsPinLocked(GPIO_TypeDef *GPIOx, uint32_t Pin);
uint8_t GPIO_IsPortLocked(GPIO_TypeDef *GPIOx);

/*============================================================================
 * Extended GPIO Driver API
 *============================================================================*/

/* Advanced Pin Configuration */
void GPIO_SetSpeed(GPIO_TypeDef *GPIOx, uint16_t Pin, GPIO_SpeedTypeDef Speed);
void GPIO_SetPull(GPIO_TypeDef *GPIOx, uint16_t Pin, GPIO_PullTypeDef Pull);
void GPIO_SetMode(GPIO_TypeDef *GPIOx, uint16_t Pin, GPIO_ModeTypeDef Mode);
void GPIO_SetOutputType(GPIO_TypeDef *GPIOx, uint16_t Pin, GPIO_OTypeTypeDef Ot);

/* Alternate Function Configuration */
void GPIO_SetAF(GPIO_TypeDef *GPIOx, uint16_t Pin, GPIO_AFTypeDef AF);

/* Fast Pin Operations (Atomic) */
__attribute__((always_inline)) static inline void GPIO_SetPinFast(GPIO_TypeDef *GPIOx, uint16_t Pin) {
    GPIOx->BSRR = Pin;
}

__attribute__((always_inline)) static inline void GPIO_ResetPinFast(GPIO_TypeDef *GPIOx, uint16_t Pin) {
    GPIOx->BRR = Pin;
}

/*============================================================================
 * Inline Helper Functions
 *============================================================================*/
__attribute__((always_inline)) static inline uint32_t GPIO_GetPortIndex(GPIO_TypeDef *GPIOx) {
    if (GPIOx == GPIOA) return 0;
    if (GPIOx == GPIOB) return 1;
    if (GPIOx == GPIOC) return 2;
    if (GPIOx == GPIOD) return 3;
    if (GPIOx == GPIOE) return 4;
    if (GPIOx == GPIOF) return 5;
    return 0;
}

#endif /* GPIO_H */
