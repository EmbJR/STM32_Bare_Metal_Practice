/**
 * @file gpioRF52_Example.c
 * @brief nRF52832 GPIO Driver Example Code
 * @description This file contains example code demonstrating how to use the
 *              GPIO driver for basic I/O operations and external interrupts.
 * 
 * ============================================================================
 * HARDWARE SETUP (nRF52 DK):
 * ============================================================================
 * Example 1 - LED Toggle:
 *   - Pin P0.17: Connected to LED1 (output)
 * 
 * Example 2 - Button Interrupt:
 *   - Pin P0.13: Connected to Button 1 (input with pull-up)
 *   - When button is pressed, LED toggles
 * 
 * Example 3 - Multiple Pin Control:
 *   - Pin P0.18: LED2 (output)
 *   - Pin P0.19: LED3 (output)
 *   - Pin P0.20: LED4 (output)
 * 
 * ============================================================================
 * USAGE NOTES:
 * ============================================================================
 * 1. This code is designed for bare-metal programming (no RTOS/HAL)
 * 2. For interrupt-driven applications, call GPIO_ProcessEvents() in the
 *    GPIO interrupt handler or main loop
 * 3. The driver uses direct register access without any external libraries
 * ============================================================================
 */

/*============================================================================
 * INCLUDES
 *============================================================================*/
#include "gpioRF52.h"
#include <stdint.h>
#include <stdbool.h>

/*============================================================================
 * EXAMPLE CONFIGURATION
 *============================================================================*/
/* Define LED and Button pins based on nRF52 DK default layout */
#define LED1_PIN               GPIO_PIN_17
#define LED2_PIN               GPIO_PIN_18
#define LED3_PIN               GPIO_PIN_19
#define LED4_PIN               GPIO_PIN_20

#define BUTTON1_PIN            GPIO_PIN_13
#define BUTTON2_PIN            GPIO_PIN_14
#define BUTTON3_PIN            GPIO_PIN_15
#define BUTTON4_PIN            GPIO_PIN_16

/*============================================================================
 * PRIVATE VARIABLES
 *============================================================================*/
static volatile bool g_ButtonPressed = false;
static volatile uint32_t g_InterruptCount = 0;

/*============================================================================
 * PRIVATE FUNCTION PROTOTYPES
 *============================================================================*/
void Example_BasicIO(void);
void Example_Interrupt(void);
void Example_PortInterrupt(void);
void Example_PinConfiguration(void);

/* Interrupt callback functions */
static void Button1_Callback(GPIO_Pin_t pin);
static void Button2_Callback(GPIO_Pin_t pin);
static void PortInterrupt_Callback(GPIO_Pin_t pin);

/* Simple delay function (requires clock initialization for accurate timing) */
void Delay(uint32_t count);

/*============================================================================
 * MAIN FUNCTION
 *============================================================================*/

/**
 * @brief Main function demonstrating GPIO driver usage
 */
int main(void)
{
    /* Initialize GPIO driver */
    GPIO_Init();
    
    /* Initialize GPIO interrupts */
    GPIO_Interrupt_Init();
    
    /* Run examples */
    while(1)
    {
        /* Example 1: Basic LED toggle on button press */
        Example_BasicIO();
        
        /* Example 2: Interrupt-driven button handling */
        Example_Interrupt();
        
        /* Example 3: Port interrupt for multiple buttons */
        Example_PortInterrupt();
        
        /* Process GPIO events (call this in main loop for event-driven approach) */
        GPIO_ProcessEvents();
        
        /* Small delay to prevent tight looping */
        Delay(1000);
    }
    
    return 0;
}

/*============================================================================
 * EXAMPLE 1: BASIC INPUT/OUTPUT
 *============================================================================*/

/**
 * @brief Example 1: Basic LED control using push button
 * @description This example shows how to:
 *              - Configure a pin as output (LED)
 *              - Configure a pin as input with pull-up (Button)
 *              - Read button state and control LED
 */
void Example_BasicIO(void)
{
    /* Configure LED1 pin as output */
    GPIO_SetDirection(LED1_PIN, GPIO_DIR_OUTPUT);
    
    /* Configure Button1 pin as input with pull-up resistor */
    GPIO_ConfigPin(BUTTON1_PIN, GPIO_DIR_INPUT, GPIO_PULL_PULLUP);
    
    /* Check if button is pressed (active low) */
    if (GPIO_ReadPin(BUTTON1_PIN) == GPIO_STATE_LOW)
    {
        /* Turn on LED */
        GPIO_WritePin(LED1_PIN, GPIO_STATE_HIGH);
    }
    else
    {
        /* Turn off LED */
        GPIO_WritePin(LED1_PIN, GPIO_STATE_LOW);
    }
}

/*============================================================================
 * EXAMPLE 2: EXTERNAL INTERRUPTS (GPIOTE)
 *============================================================================*/

/**
 * @brief Example 2: External interrupt configuration and handling
 * @description This example shows how to:
 *              - Configure GPIO interrupts on specific pins
 *              - Register callback functions for interrupts
 *              - Handle interrupt events
 */
void Example_Interrupt(void)
{
    /* Configure Button2 as input with pull-up */
    GPIO_ConfigPin(BUTTON2_PIN, GPIO_DIR_INPUT, GPIO_PULL_PULLUP);
    
    /* Configure LED2 as output */
    GPIO_SetDirection(LED2_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED2_PIN, GPIO_STATE_LOW);
    
    /* Configure interrupt on Button2 pin
     * - Channel 0: GPIOTE channel 0
     * - Pin: BUTTON2_PIN
     * - Trigger: Falling edge (button press = active low)
     */
    GPIO_Interrupt_Config(GPIO_INT_PIN_0, BUTTON2_PIN, GPIO_INT_TRIGGER_FALLING);
    
    /* Register callback function for the interrupt on channel 0 */
    GPIO_Interrupt_Callback(GPIO_INT_PIN_0, Button1_Callback);
    
    /* Enable the interrupt */
    GPIO_Interrupt_Enable(GPIO_INT_PIN_0);
}

/**
 * @brief Interrupt callback for Button 1
 * @param pin The pin that triggered the interrupt
 */
static void Button1_Callback(GPIO_Pin_t pin)
{
    (void)pin;  /* Unused parameter */
    
    /* Toggle LED2 on button press */
    GPIO_TogglePin(LED2_PIN);
    
    /* Set flag to indicate button was pressed */
    g_ButtonPressed = true;
}

/*============================================================================
 * EXAMPLE 3: PORT INTERRUPT (MULTIPLE PINS)
 *============================================================================*/

/**
 * @brief Example 3: Port interrupt for wake-up from deep sleep
 * @description This example shows how to:
 *              - Configure multiple pins with sense capability
 *              - Use port interrupt to detect any pin change
 *              - Wake up from low power mode on pin change
 */
void Example_PortInterrupt(void)
{
    /* Configure Button3 and Button4 with pull-ups and sense for low level */
    GPIO_ConfigPin(BUTTON3_PIN, GPIO_DIR_INPUT, GPIO_PULL_PULLUP);
    GPIO_ConfigPin(BUTTON4_PIN, GPIO_DIR_INPUT, GPIO_PULL_PULLUP);
    
    /* Configure sense to trigger on low level (button press) */
    GPIO_SetSense(BUTTON3_PIN, GPIO_SENSE_LOW);
    GPIO_SetSense(BUTTON4_PIN, GPIO_SENSE_LOW);
    
    /* Configure LED3 and LED4 as outputs */
    GPIO_SetDirection(LED3_PIN, GPIO_DIR_OUTPUT);
    GPIO_SetDirection(LED4_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED3_PIN, GPIO_STATE_LOW);
    GPIO_WritePin(LED4_PIN, GPIO_STATE_LOW);
    
    /* Register port interrupt callback */
    GPIO_Interrupt_CallbackPort(PortInterrupt_Callback);
    
    /* Enable port interrupt */
    GPIO_Interrupt_EnablePort();
}

/**
 * @brief Port interrupt callback
 * @param pin Placeholder pin number (not used for port interrupt)
 */
static void PortInterrupt_Callback(GPIO_Pin_t pin)
{
    (void)pin;
    
    /* Increment interrupt counter */
    g_InterruptCount++;
    
    /* Check which button caused the interrupt */
    if (GPIO_ReadPin(BUTTON3_PIN) == GPIO_STATE_LOW)
    {
        /* Button 3 pressed - toggle LED3 */
        GPIO_TogglePin(LED3_PIN);
    }
    
    if (GPIO_ReadPin(BUTTON4_PIN) == GPIO_STATE_LOW)
    {
        /* Button 4 pressed - toggle LED4 */
        GPIO_TogglePin(LED4_PIN);
    }
    
    /* Note: After port interrupt, SENSE needs to be re-enabled
     * The hardware clears SENSE after triggering, so re-configure if needed */
    GPIO_SetSense(BUTTON3_PIN, GPIO_SENSE_LOW);
    GPIO_SetSense(BUTTON4_PIN, GPIO_SENSE_LOW);
}

/*============================================================================
 * EXAMPLE 4: PIN CONFIGURATION OPTIONS
 *============================================================================*/

/**
 * @brief Example 4: Various pin configuration options
 * @description This example demonstrates different pin configuration options
 *              including drive strength and pull settings
 */
void Example_PinConfiguration(void)
{
    /* Example: Configure pin with high drive strength */
    GPIO_ConfigPin(LED1_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_DISABLED);
    GPIO_SetDrive(LED1_PIN, GPIO_DRIVE_H0H1);  /* High drive both edges */
    
    /* Example: Configure pin with pull-down (for open-collector sensors) */
    GPIO_ConfigPin(BUTTON1_PIN, GPIO_DIR_INPUT, GPIO_PULL_PULLDOWN);
    
    /* Example: Read pin direction */
    GPIO_Direction_t dir = GPIO_GetDirection(LED1_PIN);
    (void)dir;
    
    /* Example: Write to entire port (all 32 pins at once) */
    GPIO_WritePort(0xFFFFFFFF);  /* Set all pins high */
    GPIO_WritePort(0x00000000);  /* Set all pins low */
    
    /* Example: Read entire port */
    uint32_t portValue = GPIO_ReadPort();
    (void)portValue;
    
    /* Example: Set multiple pins as output at once */
    GPIO_SetPinsOutput((1UL << LED1_PIN) | (1UL << LED2_PIN) | 
                      (1UL << LED3_PIN) | (1UL << LED4_PIN));
}

/*============================================================================
 * HELPER FUNCTIONS
 *============================================================================*/

/**
 * @brief Simple delay function
 * @param count Delay count value
 * @note This is a simple busy-wait delay. For accurate timing, use a timer.
 */
void Delay(uint32_t count)
{
    volatile uint32_t i;
    for (i = 0; i < count; i++)
    {
        __asm("nop");  /* No-operation instruction */
    }
}

/*============================================================================
 * INTERRUPT HANDLER EXAMPLE
 *============================================================================*/

/**
 * @brief GPIO Interrupt Handler Example
 * @description This function should be called from the actual hardware 
 *              interrupt handler (e.g., GPIOTE_IRQHandler)
 * 
 * Usage in your main code:
 * 
 * void GPIOTE_IRQHandler(void)
 * {
 *     GPIO_ProcessEvents();
 * }
 */

/*============================================================================
 * ADDITIONAL USAGE EXAMPLES
 *============================================================================*/

/**
 * @brief Example: Multiple interrupt channels
 * @description This shows how to use multiple GPIOTE channels for different pins
 */
void Example_MultipleInterrupts(void)
{
    /* Initialize GPIO interrupts first */
    GPIO_Interrupt_Init();
    
    /* Configure Button 1 interrupt on channel 0 */
    GPIO_ConfigPin(BUTTON1_PIN, GPIO_DIR_INPUT, GPIO_PULL_PULLUP);
    GPIO_Interrupt_Config(GPIO_INT_PIN_0, BUTTON1_PIN, GPIO_INT_TRIGGER_FALLING);
    GPIO_Interrupt_Callback(GPIO_INT_PIN_0, Button1_Callback);
    GPIO_Interrupt_Enable(GPIO_INT_PIN_0);
    
    /* Configure Button 2 interrupt on channel 1 */
    GPIO_ConfigPin(BUTTON2_PIN, GPIO_DIR_INPUT, GPIO_PULL_PULLUP);
    GPIO_Interrupt_Config(GPIO_INT_PIN_1, BUTTON2_PIN, GPIO_INT_TRIGGER_FALLING);
    GPIO_Interrupt_Callback(GPIO_INT_PIN_1, Button2_Callback);
    GPIO_Interrupt_Enable(GPIO_INT_PIN_1);
}

/**
 * @brief Callback for Button 2
 */
static void Button2_Callback(GPIO_Pin_t pin)
{
    (void)pin;
    /* Handle Button 2 press - toggle LED4 */
    GPIO_TogglePin(LED4_PIN);
}

/*============================================================================
 * END OF EXAMPLE FILE
 *============================================================================*/
