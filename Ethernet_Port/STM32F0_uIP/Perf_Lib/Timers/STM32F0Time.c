#include "STM32F0Time.h"
#include <stddef.h>
#include "F051NVIC.h"

// Use RCC helper to get system clock
extern uint32_t RCC_GetSystemClockFrequency(void);

static TimerCallback TimerCallbacks[TIMER_COUNT] = { 0 };

static TIM_TypeDef *const TimerRegisters[TIMER_COUNT] = {
    0,
    TIM1,
    TIM2,
    TIM3,
    TIM14,
    TIM15,
    TIM16,
    TIM17
};

static const uint32_t TimerRccMask[TIMER_COUNT] = {
    0,
    (1U << 11), // TIM1 on APB2ENR
    (1U << 0),  // TIM2 on APB1ENR
    (1U << 1),  // TIM3 on APB1ENR
    (1U << 8),  // TIM14 on APB1ENR
    (1U << 16), // TIM15 on APB2ENR
    (1U << 17), // TIM16 on APB2ENR
    (1U << 18)  // TIM17 on APB2ENR
};

static const bool TimerOnAPB2[TIMER_COUNT] = {
    false,
    true,
    false,
    false,
    false,
    true,
    true,
    true
};

static const int TimerIRQn[TIMER_COUNT] = {
    -1,
    13, // TIM1_BRK_UP_TRG_COM_IRQn
    15, // TIM2_IRQn
    16, // TIM3_IRQn
    19, // TIM14_IRQn
    20, // TIM15_IRQn
    21, // TIM16_IRQn
    22  // TIM17_IRQn
};
/*
void NVIC_SetPriority(int irq, uint8_t priority) {
    if (irq < 0) {
        return;
    }
    uint32_t index = irq >> 2;
    uint32_t shift = (irq & 3u) * 8u;
    volatile uint32_t *ipr = NVIC_IPR0 + index;
    uint32_t value = *ipr;
    value &= ~(0xFFUL << shift);
    value |= ((uint32_t)priority << shift);
    *ipr = value;
}

void NVIC_EnableIRQ(int irq) {
    if (irq < 0) {
        return;
    }
    NVIC_ISER0 = (1UL << irq);
}

void NVIC_DisableIRQ(int irq) {
    if (irq < 0) {
        return;
    }
    NVIC_ICPR0 = (1UL << irq);
}
*/
TIM_TypeDef *GetTimer(uint32_t timerId) {
    if (timerId <= TIMER_INVALID || timerId >= TIMER_COUNT) {
        return NULL;
    }
    return TimerRegisters[timerId];
}

uint32_t STM32F0Timer_GetTimerClockHz(TimerId timer) {
    if (timer <= TIMER_INVALID || timer >= TIMER_COUNT) {
        return 0;
    }

    uint32_t sysclk = RCC_GetSystemClockFrequency();
    uint32_t ppre = (RCC_CFGR >> 8) & 0x7U; // APB prescaler encoding

    uint32_t apb_div = 1;
    if ((ppre & 0x4U) == 0) {
        apb_div = 1;
    } else {
        switch (ppre) {
            case 4: apb_div = 2; break;
            case 5: apb_div = 4; break;
            case 6: apb_div = 8; break;
            case 7: apb_div = 16; break;
            default: apb_div = 1; break;
        }
    }

    uint32_t pclk = sysclk / apb_div;

    // On STM32 families timers on APB run at PCLK when APB prescaler == 1,
    // otherwise they run at PCLK * 2. This matches standard STM32 behaviour.
    uint32_t timerclk = (apb_div == 1) ? pclk : (pclk * 2U);

    return timerclk;
}

int GetTimerIRQn(TimerId timer) {
    if (timer <= TIMER_INVALID || timer >= TIMER_COUNT) {
        return -1;
    }
    return TimerIRQn[timer];
}

void EnableTimerClock(TimerId timer) {
    if (timer <= TIMER_INVALID || timer >= TIMER_COUNT) {
        return;
    }
    if (TimerOnAPB2[timer]) {
        RCC_APB2ENR |= TimerRccMask[timer];
    } else {
        RCC_APB1ENR |= TimerRccMask[timer];
    }
}

void DisableTimerClock(TimerId timer) {
    if (timer <= TIMER_INVALID || timer >= TIMER_COUNT) {
        return;
    }
    if (TimerOnAPB2[timer]) {
        RCC_APB2ENR &= ~TimerRccMask[timer];
    } else {
        RCC_APB1ENR &= ~TimerRccMask[timer];
    }
}

bool STM32F0Timer_Init(const TimerConfig *config) {
    if (!config) {
        return false;
    }

    TIM_TypeDef *tim = GetTimer(config->timer);
    if (!tim) {
        return false;
    }

    EnableTimerClock(config->timer);

    tim->CR1 = 0;
    tim->PSC = config->prescaler;
    tim->ARR = config->autoReload;
    tim->CNT = 0;

    if (config->countMode == TIMER_MODE_DOWN) {
        tim->CR1 |= (1U << 4);
    }
    if (config->onePulseMode) {
        tim->CR1 |= (1U << 3);
    }
    if (config->autoReloadPreload) {
        tim->CR1 |= (1U << 7);
    }

    if (config->updateInterrupt) {
        tim->DIER |= TIMER_UPDATE_IT;

        int irqn = GetTimerIRQn(config->timer);
        if (irqn >= 0) {
            NVIC_SetPriority(irqn, config->irqPriority);
            NVIC_EnableIRQ(irqn);
        }
    } else {
        tim->DIER &= ~TIMER_UPDATE_IT;
    }

    tim->EGR = 1U;
    return true;
}

bool STM32F0Timer_ConfigurePeriodUs(TimerId timer, uint32_t timerClockHz, uint32_t periodUs) {
    if (timer <= TIMER_INVALID || timer >= TIMER_COUNT || timerClockHz == 0 || periodUs == 0) {
        return false;
    }

    uint64_t ticks = (uint64_t)timerClockHz * periodUs / 1000000ULL;
    if (ticks == 0) {
        ticks = 1;
    }
    if (ticks > 0x100000000ULL) {
        return false;
    }

    uint32_t prescaler = (uint32_t)((ticks + 0xFFFFULL) / 0x10000ULL);
    if (prescaler == 0) {
        prescaler = 1;
    }
    if (prescaler > 0x10000U) {
        prescaler = 0x10000U;
    }

    uint32_t arr = (uint32_t)((ticks + prescaler - 1ULL) / prescaler);
    if (arr == 0) {
        arr = 1;
    }
    if (arr > 0x10000U) {
        return false;
    }

    TimerConfig config = {
        .timer = timer,
        .prescaler = prescaler - 1U,
        .autoReload = arr - 1U,
        .updateInterrupt = false,
        .irqPriority = 0,
        .countMode = TIMER_MODE_UP,
        .onePulseMode = false,
        .autoReloadPreload = false
    };

    return STM32F0Timer_Init(&config);
}

void STM32F0Timer_SetPeriod(TimerId timer, uint32_t prescaler, uint32_t autoReload) {
    TIM_TypeDef *tim = GetTimer(timer);
    if (!tim) {
        return;
    }
    tim->PSC = prescaler;
    tim->ARR = autoReload;
    tim->EGR = 1U;
}

void STM32F0Timer_SetUpdateCallback(TimerId timer, TimerCallback callback) {
    if (timer <= TIMER_INVALID || timer >= TIMER_COUNT) {
        return;
    }
    TimerCallbacks[timer] = callback;
}

void STM32F0Timer_Start(TimerId timer) {
    TIM_TypeDef *tim = GetTimer(timer);
    if (!tim) {
        return;
    }
    tim->CR1 |= 1U;
}

void STM32F0Timer_Stop(TimerId timer) {
    TIM_TypeDef *tim = GetTimer(timer);
    if (!tim) {
        return;
    }
    tim->CR1 &= ~1U;
}

void STM32F0Timer_GenerateUpdateEvent(TimerId timer) {
    TIM_TypeDef *tim = GetTimer(timer);
    if (!tim) {
        return;
    }
    tim->EGR = 1U;
}

void STM32F0Timer_ClearUpdateFlag(TimerId timer) {
    TIM_TypeDef *tim = GetTimer(timer);
    if (!tim) {
        return;
    }
    tim->SR &= ~TIMER_UIF_FLAG;
}

void STM32F0Timer_EnableUpdateInterrupt(TimerId timer, bool enable) {
    TIM_TypeDef *tim = GetTimer(timer);
    if (!tim) {
        return;
    }

    if (enable) {
        tim->DIER |= TIMER_UPDATE_IT;
        int irqn = GetTimerIRQn(timer);
        if (irqn >= 0) {
            NVIC_EnableIRQ(irqn);
        }
    } else {
        tim->DIER &= ~TIMER_UPDATE_IT;
        int irqn = GetTimerIRQn(timer);
        if (irqn >= 0) {
            NVIC_DisableIRQ(irqn);
        }
    }
}

static void HandleTimerInterrupt(TimerId timer) {
    TIM_TypeDef *tim = GetTimer(timer);
    if (!tim) {
        return;
    }

    if (tim->SR & TIMER_UIF_FLAG) {
        tim->SR &= ~TIMER_UIF_FLAG;
        if (TimerCallbacks[timer]) {
            TimerCallbacks[timer]();
        }
    }
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void) {
    HandleTimerInterrupt(TIMER1);
}

void TIM2_IRQHandler(void) {
    HandleTimerInterrupt(TIMER2);
}

void TIM3_IRQHandler(void) {
    HandleTimerInterrupt(TIMER3);
}

void TIM14_IRQHandler(void) {
    HandleTimerInterrupt(TIMER14);
}

void TIM15_IRQHandler(void) {
    HandleTimerInterrupt(TIMER15);
}

void TIM16_IRQHandler(void) {
    HandleTimerInterrupt(TIMER16);
}

void TIM17_IRQHandler(void) {
    HandleTimerInterrupt(TIMER17);
}
