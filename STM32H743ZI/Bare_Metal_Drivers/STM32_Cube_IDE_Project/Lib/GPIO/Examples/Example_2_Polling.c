#include "GPIO_Examples.h"

/* =====================================================================
 *  Example 2: Read switch on PG0 (polling) -> drive LED on PB14.
 *
 *  PG0 is configured as an input with an internal pull-up, so the pin
 *  reads HIGH when the switch is released and LOW when pressed
 *  (active-low). The LED on PB14 is driven HIGH while the switch is
 *  pressed and LOW otherwise.
 * ===================================================================== */

void Example_2_SwitchPolling(void) {
    LED_Init();
    SW_Init();

    while (1) {
        if (GPIO_ReadPin(SW_PORT, SW_PIN) == GPIO_PIN_LOW) {
            GPIO_SetPin(LED_PORT, LED_PIN);    /* switch pressed -> LED on  */
        } else {
            GPIO_ResetPin(LED_PORT, LED_PIN);  /* released       -> LED off */
        }
    }
}
