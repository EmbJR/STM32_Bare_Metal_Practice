#include "GPIO_Examples.h"

/* =====================================================================
 *  Example 3: Read switch on PG0 (interrupt) -> drive LED on PB14.
 *
 *  PG0 is mapped to EXTI line 0. A falling edge (switch press, since the
 *  pin uses a pull-up and goes LOW when pressed) triggers the EXTI0
 *  interrupt. The handler sets the LED on PB14 HIGH.
 *
 *  PG0 -> EXTI0 -> EXTI0_IRQHandler (vector position 6).
 * ===================================================================== */

void Example_3_SwitchInterrupt(void) {
    LED_Init();   /* PB14 push-pull output             */
    SW_Init();    /* PG0 input with pull-up (active-low) */

    /* Route PG0 to EXTI0, trigger on falling edge (button press) */
    GPIO_ConfigInterrupt(SW_PORT, SW_PIN, GPIO_IT_FALLING);
    GPIO_EnableInterrupt(SW_PIN);

    while (1) {
        /* Work is done in the interrupt handler */
    }
}

/* ---------------------------------------------------------------------
 *  EXTI0 interrupt service routine.
 *  The name must match the entry in the startup vector table.
 * ------------------------------------------------------------------- */
void EXTI0_IRQHandler(void) {
    if (GPIO_GetPendingInterrupt(SW_PIN)) {
        GPIO_ClearPendingInterrupt(SW_PIN);    /* acknowledge first */
        GPIO_SetPin(LED_PORT, LED_PIN);        /* switch pressed -> LED on */
    }
}
