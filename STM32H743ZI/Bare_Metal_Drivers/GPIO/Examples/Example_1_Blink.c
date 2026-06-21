#include "GPIO_Examples.h"

/* =====================================================================
 *  Example 1: Toggle PB14 every 1 second.
 *
 *  PB14 is configured as a push-pull output. The pin is toggled in an
 *  endless loop with a 1000 ms delay between transitions.
 * ===================================================================== */

void Example_1_BlinkLED_1s(void) {
    LED_Init();

    while (1) {
        GPIO_TogglePin(LED_PORT, LED_PIN);
        Delay_ms(1000);
    }
}
