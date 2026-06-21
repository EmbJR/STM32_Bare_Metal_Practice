#ifndef GPIO_H743ZI_H
#define GPIO_H743ZI_H

#include <stdint.h>

/* =====================================================================
 *  STM32H743ZIT6 - Bare Metal GPIO Driver
 *  No CMSIS / HAL dependency. Direct register access only.
 *
 *  Reference: RM0433 (STM32H742/743/753/750)
 *  - GPIO ports live on the AHB4 bus.
 *  - Each GPIO port occupies a 0x400 byte block.
 * ===================================================================== */

/* ---------------------------------------------------------------------
 *  GPIO port base addresses (AHB4 bus)
 * ------------------------------------------------------------------- */
#define GPIOA_BASE              0x58020000UL
#define GPIOB_BASE              0x58020400UL
#define GPIOC_BASE              0x58020800UL
#define GPIOD_BASE              0x58020C00UL
#define GPIOE_BASE              0x58021000UL
#define GPIOF_BASE              0x58021400UL
#define GPIOG_BASE              0x58021800UL
#define GPIOH_BASE              0x58021C00UL
#define GPIOI_BASE              0x58022000UL
#define GPIOJ_BASE              0x58022400UL
#define GPIOK_BASE              0x58022800UL

/* ---------------------------------------------------------------------
 *  GPIO register block layout (offsets from a port base)
 * ------------------------------------------------------------------- */
typedef struct {
    volatile uint32_t MODER;    /* 0x00  Mode register                  */
    volatile uint32_t OTYPER;   /* 0x04  Output type register           */
    volatile uint32_t OSPEEDR;  /* 0x08  Output speed register          */
    volatile uint32_t PUPDR;    /* 0x0C  Pull-up/pull-down register     */
    volatile uint32_t IDR;      /* 0x10  Input data register            */
    volatile uint32_t ODR;      /* 0x14  Output data register           */
    volatile uint32_t BSRR;     /* 0x18  Bit set/reset register         */
    volatile uint32_t LCKR;     /* 0x1C  Configuration lock register    */
    volatile uint32_t AFR[2];   /* 0x20  Alternate function low/high    */
} GPIO_TypeDef;

#define GPIOA                   ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB                   ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC                   ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD                   ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE                   ((GPIO_TypeDef *)GPIOE_BASE)
#define GPIOF                   ((GPIO_TypeDef *)GPIOF_BASE)
#define GPIOG                   ((GPIO_TypeDef *)GPIOG_BASE)
#define GPIOH                   ((GPIO_TypeDef *)GPIOH_BASE)
#define GPIOI                   ((GPIO_TypeDef *)GPIOI_BASE)
#define GPIOJ                   ((GPIO_TypeDef *)GPIOJ_BASE)
#define GPIOK                   ((GPIO_TypeDef *)GPIOK_BASE)

/* ---------------------------------------------------------------------
 *  RCC clock enable for GPIO ports (AHB4ENR)
 *  RCC base in this project is 0x58024400, AHB4ENR offset = 0xE0.
 * ------------------------------------------------------------------- */
#define RCC_BASE_ADDR           0x58024400UL
#define RCC_AHB4ENR_REG         (*(volatile uint32_t *)(RCC_BASE_ADDR + 0xE0))
#define RCC_APB4ENR_REG         (*(volatile uint32_t *)(RCC_BASE_ADDR + 0xF4))
#define RCC_APB4ENR_SYSCFGEN    (1U << 1)

/* ---------------------------------------------------------------------
 *  SYSCFG - used to select the GPIO port for each EXTI line.
 *  SYSCFG base = 0x58000400, EXTICR[0..3] at offset 0x08..0x14.
 * ------------------------------------------------------------------- */
#define SYSCFG_BASE             0x58000400UL
typedef struct {
    volatile uint32_t RESERVED0;    /* 0x00 */
    volatile uint32_t PMCR;         /* 0x04 */
    volatile uint32_t EXTICR[4];    /* 0x08 - 0x14  external IRQ config  */
} SYSCFG_TypeDef;
#define SYSCFG                  ((SYSCFG_TypeDef *)SYSCFG_BASE)

/* ---------------------------------------------------------------------
 *  EXTI - External interrupt/event controller.
 *  EXTI base = 0x58000000. (CPU mask/pending registers used here.)
 * ------------------------------------------------------------------- */
#define EXTI_BASE               0x58000000UL
#define EXTI_RTSR1              (*(volatile uint32_t *)(EXTI_BASE + 0x00)) /* rising  trigger */
#define EXTI_FTSR1              (*(volatile uint32_t *)(EXTI_BASE + 0x04)) /* falling trigger */
#define EXTI_SWIER1             (*(volatile uint32_t *)(EXTI_BASE + 0x08)) /* software int    */
#define EXTI_CPUIMR1            (*(volatile uint32_t *)(EXTI_BASE + 0x80)) /* CPU int mask    */
#define EXTI_CPUEMR1            (*(volatile uint32_t *)(EXTI_BASE + 0x84)) /* CPU event mask  */
#define EXTI_CPUPR1             (*(volatile uint32_t *)(EXTI_BASE + 0x88)) /* CPU pending     */

/* ---------------------------------------------------------------------
 *  NVIC (Cortex-M7) - interrupt set-enable / clear-pending.
 * ------------------------------------------------------------------- */
#define NVIC_ISER_BASE          0xE000E100UL
#define NVIC_ICER_BASE          0xE000E180UL
#define NVIC_ISER(n)            (*(volatile uint32_t *)(NVIC_ISER_BASE + ((n) * 4)))
#define NVIC_ICER(n)            (*(volatile uint32_t *)(NVIC_ICER_BASE + ((n) * 4)))

/* EXTI line IRQ numbers (RM0433 vector table) */
#define EXTI0_IRQn              6
#define EXTI1_IRQn              7
#define EXTI2_IRQn              8
#define EXTI3_IRQn              9
#define EXTI4_IRQn              10
#define EXTI9_5_IRQn            23
#define EXTI15_10_IRQn          40

/* ---------------------------------------------------------------------
 *  Driver enumerations
 * ------------------------------------------------------------------- */
typedef enum {
    GPIO_MODE_INPUT     = 0x0,   /* digital input            */
    GPIO_MODE_OUTPUT    = 0x1,   /* general purpose output   */
    GPIO_MODE_ALTERNATE = 0x2,   /* alternate function       */
    GPIO_MODE_ANALOG    = 0x3    /* analog mode              */
} GPIO_Mode;

typedef enum {
    GPIO_OTYPE_PUSHPULL  = 0x0,
    GPIO_OTYPE_OPENDRAIN = 0x1
} GPIO_OutputType;

typedef enum {
    GPIO_SPEED_LOW       = 0x0,
    GPIO_SPEED_MEDIUM    = 0x1,
    GPIO_SPEED_HIGH      = 0x2,
    GPIO_SPEED_VERYHIGH  = 0x3
} GPIO_Speed;

typedef enum {
    GPIO_PULL_NONE = 0x0,
    GPIO_PULL_UP   = 0x1,
    GPIO_PULL_DOWN = 0x2
} GPIO_Pull;

typedef enum {
    GPIO_PIN_LOW  = 0,
    GPIO_PIN_HIGH = 1
} GPIO_PinState;

/* Interrupt edge selection for EXTI configuration */
typedef enum {
    GPIO_IT_RISING  = 0x1,
    GPIO_IT_FALLING = 0x2,
    GPIO_IT_BOTH    = 0x3
} GPIO_IntEdge;

/* Single pin configuration descriptor */
typedef struct {
    uint8_t          pin;        /* 0 .. 15                  */
    GPIO_Mode        mode;
    GPIO_OutputType  otype;
    GPIO_Speed       speed;
    GPIO_Pull        pull;
    uint8_t          alternate;  /* AF0 .. AF15 (only if mode = ALTERNATE) */
} GPIO_PinConfig;

/* ---------------------------------------------------------------------
 *  Public API
 * ------------------------------------------------------------------- */

/* Clock control */
void GPIO_EnableClock(GPIO_TypeDef *port);
void GPIO_DisableClock(GPIO_TypeDef *port);

/* Configuration */
void GPIO_Init(GPIO_TypeDef *port, const GPIO_PinConfig *config);
void GPIO_SetMode(GPIO_TypeDef *port, uint8_t pin, GPIO_Mode mode);
void GPIO_SetOutputType(GPIO_TypeDef *port, uint8_t pin, GPIO_OutputType otype);
void GPIO_SetSpeed(GPIO_TypeDef *port, uint8_t pin, GPIO_Speed speed);
void GPIO_SetPull(GPIO_TypeDef *port, uint8_t pin, GPIO_Pull pull);
void GPIO_SetAlternate(GPIO_TypeDef *port, uint8_t pin, uint8_t af);

/* Data access */
void          GPIO_WritePin(GPIO_TypeDef *port, uint8_t pin, GPIO_PinState state);
void          GPIO_SetPin(GPIO_TypeDef *port, uint8_t pin);
void          GPIO_ResetPin(GPIO_TypeDef *port, uint8_t pin);
void          GPIO_TogglePin(GPIO_TypeDef *port, uint8_t pin);
GPIO_PinState GPIO_ReadPin(GPIO_TypeDef *port, uint8_t pin);
uint16_t      GPIO_ReadPort(GPIO_TypeDef *port);
void          GPIO_WritePort(GPIO_TypeDef *port, uint16_t value);

/* Interrupt (EXTI) handling */
void GPIO_ConfigInterrupt(GPIO_TypeDef *port, uint8_t pin, GPIO_IntEdge edge);
void GPIO_EnableInterrupt(uint8_t pin);
void GPIO_DisableInterrupt(uint8_t pin);
uint32_t GPIO_GetPendingInterrupt(uint8_t pin);
void GPIO_ClearPendingInterrupt(uint8_t pin);

#endif /* GPIO_H743ZI_H */
