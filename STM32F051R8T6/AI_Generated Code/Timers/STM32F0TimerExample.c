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
    // Compute actual timer input clock (accounts APB prescaler)
    uint32_t t2clk = STM32F0Timer_GetTimerClockHz(TIMER2);
    uint32_t t3clk = STM32F0Timer_GetTimerClockHz(TIMER3);

    // Configure timers by period in microseconds for clarity
    // TIMER2: 1 second = 1,000,000 us (1 Hz)
    if (!STM32F0Timer_ConfigurePeriodUs(TIMER2, t2clk, 1000000U)) {
        while (1);
    }
    // TIMER3: 0.5 second = 500,000 us (2 Hz)
    if (!STM32F0Timer_ConfigurePeriodUs(TIMER3, t3clk, 500000U)) {
        while (1);
    }

    STM32F0Timer_SetUpdateCallback(TIMER2, Timer2_Callback);
    STM32F0Timer_SetUpdateCallback(TIMER3, Timer3_Callback);

    // Enable update interrupts (NVIC priority in helper)
    STM32F0Timer_EnableUpdateInterrupt(TIMER2, true);
    STM32F0Timer_EnableUpdateInterrupt(TIMER3, true);

    // Start timers
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
