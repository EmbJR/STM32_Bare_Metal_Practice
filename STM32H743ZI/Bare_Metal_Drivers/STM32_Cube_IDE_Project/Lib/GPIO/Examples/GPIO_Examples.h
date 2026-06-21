#ifndef GPIO_EXAMPLES_H
#define GPIO_EXAMPLES_H

#include "GPIOH743ZI.h"

/* Board mapping used by all examples:
 *   LED    -> PB14
 *   Switch -> PG0   (assumed active-low; pressed = pin reads LOW)
 */
#define LED_PORT        GPIOB
#define LED_PIN         14

#define SW_PORT         GPIOG
#define SW_PIN          0

/* Simple SysTick based millisecond delay (defined in main_example.c) */
void Delay_Init(void);
void Delay_ms(uint32_t ms);

/* Shared pin setup helpers (defined in main_example.c) */
void LED_Init(void);   /* PB14 push-pull output            */
void SW_Init(void);    /* PG0 input with pull-up (active-low) */

/* Example entry points */
void Example_1_BlinkLED_1s(void);
void Example_2_SwitchPolling(void);
void Example_3_SwitchInterrupt(void);

#endif /* GPIO_EXAMPLES_H */
