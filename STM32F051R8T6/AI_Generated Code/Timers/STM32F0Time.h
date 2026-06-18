#ifndef STM32F0TIME_H
#define STM32F0TIME_H

#include <stdint.h>
#include <stdbool.h>
#include "rcc.h"

#ifdef __cplusplus
extern "C" {
#endif

// RCC register addresses
//#define RCC_BASE          0x40021000UL
//#define RCC_APB1ENR       (*(volatile uint32_t *)(RCC_BASE + 0x1CUL))
//#define RCC_APB2ENR       (*(volatile uint32_t *)(RCC_BASE + 0x18UL))

// NVIC registers
#define NVIC_ISER0        (*(volatile uint32_t *)0xE000E100UL)
#define NVIC_ICPR0        (*(volatile uint32_t *)0xE000E280UL)
#define NVIC_IPR0         (volatile uint32_t *)0xE000E400UL

// Timer base addresses
#define TIM1_BASE         0x40012C00UL
#define TIM2_BASE         0x40000000UL
#define TIM3_BASE         0x40000400UL
#define TIM14_BASE        0x40002000UL
#define TIM15_BASE        0x40014000UL
#define TIM16_BASE        0x40014400UL
#define TIM17_BASE        0x40014800UL

// Timer register layout for basic timer operation
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t BDTR;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
} TIM_TypeDef;

#define TIM1              ((TIM_TypeDef *) TIM1_BASE)
#define TIM2              ((TIM_TypeDef *) TIM2_BASE)
#define TIM3              ((TIM_TypeDef *) TIM3_BASE)
#define TIM14             ((TIM_TypeDef *) TIM14_BASE)
#define TIM15             ((TIM_TypeDef *) TIM15_BASE)
#define TIM16             ((TIM_TypeDef *) TIM16_BASE)
#define TIM17             ((TIM_TypeDef *) TIM17_BASE)

// Interrupt enable bits
#define TIMER_UPDATE_IT   (1U << 0)
#define TIMER_UIF_FLAG    (1U << 0)

typedef enum {
    TIMER_INVALID = 0,
    TIMER1,
    TIMER2,
    TIMER3,
    TIMER14,
    TIMER15,
    TIMER16,
    TIMER17,
    TIMER_COUNT
} TimerId;

typedef enum {
    TIMER_MODE_UP = 0,
    TIMER_MODE_DOWN = 1
} TimerCountMode;

typedef struct {
    TimerId timer;
    uint32_t prescaler;
    uint32_t autoReload;
    bool updateInterrupt;
    uint8_t irqPriority;
    TimerCountMode countMode;
    bool onePulseMode;
    bool autoReloadPreload;
} TimerConfig;

typedef void (*TimerCallback)(void);

// Initialization and configuration
bool STM32F0Timer_Init(const TimerConfig *config);
bool STM32F0Timer_ConfigurePeriodUs(TimerId timer, uint32_t timerClockHz, uint32_t periodUs);
void STM32F0Timer_SetPeriod(TimerId timer, uint32_t prescaler, uint32_t autoReload);
void STM32F0Timer_SetUpdateCallback(TimerId timer, TimerCallback callback);
// Returns the input clock (Hz) used by the timer peripheral (accounts APB prescaler behavior)
uint32_t STM32F0Timer_GetTimerClockHz(TimerId timer);

// Control
void STM32F0Timer_Start(TimerId timer);
void STM32F0Timer_Stop(TimerId timer);
void STM32F0Timer_GenerateUpdateEvent(TimerId timer);
void STM32F0Timer_ClearUpdateFlag(TimerId timer);
void STM32F0Timer_EnableUpdateInterrupt(TimerId timer, bool enable);

#ifdef __cplusplus
}
#endif

#endif // STM32F0TIME_H
