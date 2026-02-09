#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

// GPIO Base Addresses
#define GPIOA_BASE       0x48000000UL
#define GPIOB_BASE       0x48000400UL
#define GPIOC_BASE       0x48000800UL
#define GPIOD_BASE       0x48000C00UL
#define GPIOE_BASE       0x48001000UL
#define GPIOF_BASE       0x48001400UL

// GPIO Register Structure
typedef struct {
    volatile uint32_t MODER;     // GPIO port mode register
    volatile uint32_t OTYPER;    // GPIO port output type register
    volatile uint32_t OSPEEDR;   // GPIO port output speed register
    volatile uint32_t PUPDR;     // GPIO port pull-up/pull-down register
    volatile uint32_t IDR;       // GPIO port input data register
    volatile uint32_t ODR;       // GPIO port output data register
    volatile uint32_t BSRR;      // GPIO port bit set/reset register
    volatile uint32_t LCKR;      // GPIO port configuration lock register
    volatile uint32_t AFRL;      // GPIO alternate function low register
    volatile uint32_t AFRH;      // GPIO alternate function high register
    volatile uint32_t BRR;       // GPIO port bit reset register
} GPIO_TypeDef;

// GPIO Instances
#define GPIOA           ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB           ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC           ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD           ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE           ((GPIO_TypeDef *)GPIOE_BASE)
#define GPIOF           ((GPIO_TypeDef *)GPIOF_BASE)

// GPIO Modes
typedef enum {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT = 1,
    GPIO_MODE_ALTERNATE = 2,
    GPIO_MODE_ANALOG = 3
} GPIO_Mode;

// GPIO Output Types
typedef enum {
    GPIO_OUTPUT_PUSH_PULL = 0,
    GPIO_OUTPUT_OPEN_DRAIN = 1
} GPIO_OutputType;

// GPIO Output Speeds
typedef enum {
    GPIO_SPEED_LOW = 0,      // 2 MHz
    GPIO_SPEED_MEDIUM = 1,   // 10 MHz
    GPIO_SPEED_HIGH = 3      // 50 MHz
} GPIO_Speed;

// GPIO Pull-up/Pull-down
typedef enum {
    GPIO_NO_PULL = 0,
    GPIO_PULL_UP = 1,
    GPIO_PULL_DOWN = 2
} GPIO_Pull;

// Alternate Functions
typedef enum {
    GPIO_AF0 = 0,
    GPIO_AF1 = 1,
    GPIO_AF2 = 2,
    GPIO_AF3 = 3,
    GPIO_AF4 = 4,
    GPIO_AF5 = 5,
    GPIO_AF6 = 6,
    GPIO_AF7 = 7
} GPIO_AlternateFunction;

// GPIO Configuration Structure
typedef struct {
    GPIO_Mode mode;
    GPIO_OutputType output_type;
    GPIO_Speed speed;
    GPIO_Pull pull;
    GPIO_AlternateFunction alternate;  // Used when mode is GPIO_MODE_ALTERNATE
} GPIO_Config;

// Pin Definitions (0-15 for each port)
#define GPIO_PIN_0     0
#define GPIO_PIN_1     1
#define GPIO_PIN_2     2
#define GPIO_PIN_3     3
#define GPIO_PIN_4     4
#define GPIO_PIN_5     5
#define GPIO_PIN_6     6
#define GPIO_PIN_7     7
#define GPIO_PIN_8     8
#define GPIO_PIN_9     9
#define GPIO_PIN_10    10
#define GPIO_PIN_11    11
#define GPIO_PIN_12    12
#define GPIO_PIN_13    13
#define GPIO_PIN_14    14
#define GPIO_PIN_15    15

// Function Prototypes
void GPIO_Init(GPIO_TypeDef *GPIOx, uint32_t pin, const GPIO_Config *config);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint32_t pin, bool state);
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t pin);
bool GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint32_t pin);
void GPIO_SetPin(GPIO_TypeDef *GPIOx, uint32_t pin);
void GPIO_ResetPin(GPIO_TypeDef *GPIOx, uint32_t pin);
void GPIO_LockPin(GPIO_TypeDef *GPIOx, uint32_t pin);
void GPIO_SetAlternateFunction(GPIO_TypeDef *GPIOx, uint32_t pin, GPIO_AlternateFunction af);
void GPIO_EnableClock(GPIO_TypeDef *GPIOx);
void GPIO_DisableClock(GPIO_TypeDef *GPIOx);

// Helper Macros
#define GPIO_SET_PIN(GPIOx, pin)     ((GPIOx)->BSRR = (1U << (pin)))
#define GPIO_RESET_PIN(GPIOx, pin)   ((GPIOx)->BRR = (1U << (pin)))
#define GPIO_TOGGLE_PIN(GPIOx, pin)  ((GPIOx)->ODR ^= (1U << (pin)))
#define GPIO_READ_PIN(GPIOx, pin)    (((GPIOx)->IDR >> (pin)) & 0x1U)

#endif // GPIO_H
