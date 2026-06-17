#include "STM32F0Time.h"
#include <stdint.h>
#include <stdbool.h>

volatile uint32_t timerTickCount = 0;
volatile uint32_t timerSecondCount = 0;

static void Timer2_Callback(void) {
    timerTickCount++;
}

static void Timer3_Callback(void) {
    timerSecondCount++;
}

int main(void) {
    TimerConfig timer2Config = {
        .timer = TIMER2,
        .prescaler = 4799,      // 48 MHz / (4799 + 1) = 10 kHz timer clock
        .autoReload = 9999,     // 10 kHz / (9999 + 1) = 1 Hz update event
        .updateInterrupt = true,
        .irqPriority = 2,
        .countMode = TIMER_MODE_UP,
        .onePulseMode = false,
        .autoReloadPreload = false
    };

    TimerConfig timer3Config = {
        .timer = TIMER3,
        .prescaler = 4799,
        .autoReload = 4999,     // 10 kHz / (4999 + 1) = 2 Hz update event
        .updateInterrupt = true,
        .irqPriority = 3,
        .countMode = TIMER_MODE_UP,
        .onePulseMode = false,
        .autoReloadPreload = false
    };

    STM32F0Timer_SetUpdateCallback(TIMER2, Timer2_Callback);
    STM32F0Timer_SetUpdateCallback(TIMER3, Timer3_Callback);

    if (!STM32F0Timer_Init(&timer2Config)) {
        while (1);
    }
    if (!STM32F0Timer_Init(&timer3Config)) {
        while (1);
    }

    STM32F0Timer_Start(TIMER2);
    STM32F0Timer_Start(TIMER3);

    while (1) {
        if (timerTickCount > 0) {
            timerTickCount = 0;
            // Place GPIO toggle or application logic that should run every second here.
        }

        if (timerSecondCount >= 10) {
            timerSecondCount = 0;
            // Example: use the timer callback counter to trigger a lower-frequency event.
        }
    }

    return 0;
}
