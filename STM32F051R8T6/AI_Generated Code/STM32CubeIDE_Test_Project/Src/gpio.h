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
 * EXTI Line Definitions
 *============================================================================*/
#define EXTI_BASE      (0x40010400UL)
#define EXTI_IMR        (*(volatile uint32_t *)(EXTI_BASE + 0x00))
#define EXTI_EMR        (*(volatile uint32_t *)(EXTI_BASE + 0x04))
#define EXTI_RTSR       (*(volatile uint32_t *)(EXTI_BASE + 0x08))
#define EXTI_FTSR       (*(volatile uint32_t *)(EXTI_BASE + 0x0C))
#define EXTI_SWIER      (*(volatile uint32_t *)(EXTI_BASE + 0x10))
#define EXTI_PR         (*(volatile uint32_t *)(EXTI_BASE + 0x14))

/* EXTI Line Configuration */
#define EXTI_LINE_0    (0x0001UL)
#define EXTI_LINE_1    (0x0002UL)
#define EXTI_LINE_2    (0x0004UL)
#define EXTI_LINE_3    (0x0008UL)
#define EXTI_LINE_4    (0x0010UL)
#define EXTI_LINE_5    (0x0020UL)
#define EXTI_LINE_6    (0x0040UL)
#define EXTI_LINE_7    (0x0080UL)
#define EXTI_LINE_8    (0x0100UL)
#define EXTI_LINE_9    (0x0200UL)
#define EXTI_LINE_10   (0x0400UL)
#define EXTI_LINE_11   (0x0800UL)
#define EXTI_LINE_12   (0x1000UL)
#define EXTI_LINE_13   (0x2000UL)
#define EXTI_LINE_14   (0x4000UL)
#define EXTI_LINE_15   (0x8000UL)

/* SYSCFG Base Address */
#define SYSCFG_BASE    (0x40010000UL)
#define SYSCFG_CR      (*(volatile uint32_t *)(SYSCFG_BASE + 0x00))
#define SYSCFG_EXTICR1 (*(volatile uint32_t *)(SYSCFG_BASE + 0x08))
#define SYSCFG_EXTICR2 (*(volatile uint32_t *)(SYSCFG_BASE + 0x0C))
#define SYSCFG_EXTICR3 (*(volatile uint32_t *)(SYSCFG_BASE + 0x10))
#define SYSCFG_EXTICR4 (*(volatile uint32_t *)(SYSCFG_BASE + 0x14))

/* EXTI Trigger Types */
typedef enum {
    EXTI_TRIGGER_RISING    = 0x01,  /*!< Rising edge trigger */
    EXTI_TRIGGER_FALLING   = 0x02,  /*!< Falling edge trigger */
    EXTI_TRIGGER_BOTH      = 0x03   /*!< Rising and falling edge trigger */
} EXTI_TriggerTypeDef;

/*============================================================================
 * NVIC Definitions
 *============================================================================*/
#define NVIC_BASE       (0xE000E100UL)
#define NVIC_ISER       (*(volatile uint32_t *)(NVIC_BASE + 0x00))
#define NVIC_ICER       (*(volatile uint32_t *)(NVIC_BASE + 0x80))
#define NVIC_IPR        ((volatile uint8_t *)(0xE000E400UL))

/* Interrupt Numbers */
#define EXTI0_1_IRQn       5   /*!< EXTI Line 0 and 1 Interrupt */
#define EXTI2_3_IRQn       6   /*!< EXTI Line 2 and 3 Interrupt */
#define EXTI4_15_IRQn      7   /*!< EXTI Line 4 to 15 Interrupt */

/*============================================================================
 * RCC Clock Enable/Disable for GPIO Ports
 *============================================================================*/
#define RCC_AHBENR_IOPAEN  (0x00020000UL)  /*!< GPIOA clock enable */
#define RCC_AHBENR_IOPBEN  (0x00040000UL)  /*!< GPIOB clock enable */
#define RCC_AHBENR_IOPCEN  (0x00080000UL)  /*!< GPIOC clock enable */
#define RCC_AHBENR_IOPDEN  (0x00100000UL)  /*!< GPIOD clock enable */
#define RCC_AHBENR_IOPEEN  (0x00200000UL)  /*!< GPIOE clock enable */
#define RCC_AHBENR_IOPFEN  (0x00400000UL)  /*!< GPIOF clock enable */

#define RCC_BASE           (0x40021000UL)
#define RCC_AHBENR         (*(volatile uint32_t *)(RCC_BASE + 0x14))

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
 * EXTI Driver API Prototypes
 *============================================================================*/
void EXTI_DeInit(void);
void EXTI_LineConfig(uint32_t EXTI_Line, GPIO_TypeDef *GPIOx, uint16_t Pin);
void EXTI_EnableExtiLine(uint32_t EXTI_Line, uint8_t Enable);
void EXTI_SetTrigger(uint32_t EXTI_Line, EXTI_TriggerTypeDef Trigger);
void EXTI_ClearFlag(uint32_t EXTI_Line);
uint32_t EXTI_GetFlag(uint32_t EXTI_Line);
uint32_t EXTI_GetPending(uint32_t EXTI_Line);
void EXTI_ClearPending(uint32_t EXTI_Line);

/*============================================================================
 * IRQn_Type Definition (Simplified for STM32F0xx)
 *============================================================================*/
typedef int32_t IRQn_Type;

/*============================================================================
 * NVIC Driver API Prototypes
 *============================================================================*/
void NVIC_EnableIRQ(IRQn_Type IRQn);
void NVIC_DisableIRQ(IRQn_Type IRQn);
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t Priority);
void NVIC_ClearPendingIRQ(IRQn_Type IRQn);

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
