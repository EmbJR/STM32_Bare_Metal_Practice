/**
 * @file gpioRF52.c
 * @brief nRF52832 GPIO Driver Source File
 * @description This file contains the implementation of the nRF52832 GPIO
 *              peripheral driver. It provides simple APIs to control GPIO pins,
 *              including input/output configuration, pull-up/pull-down resistors,
 *              drive strength, and external interrupts via GPIOTE.
 */

/*============================================================================
 * INCLUDES
 *============================================================================*/
#include "gpioRF52.h"
#include "Common.h"

/*============================================================================
 * PRIVATE VARIABLES
 *============================================================================*/
/* Callback function pointers for GPIO interrupts (channels 0-7) */
static GPIO_Callback_t s_InterruptCallbacks[8] = {NULL};

/* Callback function pointer for port interrupt */
static GPIO_Callback_t s_PortInterruptCallback = NULL;

/* Store configured pins for interrupt handling */
static GPIO_Pin_t s_InterruptPins[8] = {GPIO_PIN_0};

/*============================================================================
 * PRIVATE FUNCTION PROTOTYPES
 *============================================================================*/
static void GPIO_ClearAllInterruptEvents(void);

/*============================================================================
 * API FUNCTION IMPLEMENTATIONS
 *============================================================================*/

/*============================================================================
 * Basic GPIO Functions
 *============================================================================*/

/**
 * @brief Initialize the GPIO driver
 */
void GPIO_Init(void)
{
    /* Clear all output pins (set to 0) */
    GPIO_REG(GPIO_BASE_ADDR + GPIO_OUT) = 0;

    /* Set all pins as input by default */
    GPIO_REG(GPIO_BASE_ADDR + GPIO_DIR) = 0;

    /* Clear all interrupt events */
    GPIO_ClearAllInterruptEvents();
}

/**
 * @brief Configure a GPIO pin
 */
void GPIO_ConfigPin(GPIO_Pin_t pin, GPIO_Direction_t direction, GPIO_Pull_t pull)
{
    uint32_t cnf;

    /* Configure pin configuration register */
    cnf = (direction << GPIO_PIN_CNF_DIR_Pos) |
          (GPIO_PIN_CNF_INPUT_Connect << 1) |
          (pull << GPIO_PIN_CNF_PULL_Pos) |
          (GPIO_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
          (GPIO_SENSE_DISABLED << GPIO_PIN_CNF_SENSE_Pos);

    GPIO_REG(GPIO_BASE_ADDR + GPIO_PIN_CNF(pin)) = cnf;
}

/**
 * @brief Set GPIO pin direction
 */
void GPIO_SetDirection(GPIO_Pin_t pin, GPIO_Direction_t direction)
{
    if (direction == GPIO_DIR_OUTPUT)
    {
        GPIO_REG(GPIO_BASE_ADDR + GPIO_DIRSET) = (1UL << pin);
    }
    else
    {
        GPIO_REG(GPIO_BASE_ADDR + GPIO_DIRCLR) = (1UL << pin);
    }
}

/**
 * @brief Get GPIO pin direction
 */
GPIO_Direction_t GPIO_GetDirection(GPIO_Pin_t pin)
{
    uint32_t dir = GPIO_REG(GPIO_BASE_ADDR + GPIO_DIR);
    return (GPIO_Direction_t)((dir >> pin) & 0x1UL);
}

/**
 * @brief Configure pull-up/pull-down for a pin
 */
void GPIO_SetPull(GPIO_Pin_t pin, GPIO_Pull_t pull)
{
    uint32_t cnf = GPIO_REG(GPIO_BASE_ADDR + GPIO_PIN_CNF(pin));
    cnf = (cnf & ~(0x3UL << GPIO_PIN_CNF_PULL_Pos)) | (pull << GPIO_PIN_CNF_PULL_Pos);
    GPIO_REG(GPIO_BASE_ADDR + GPIO_PIN_CNF(pin)) = cnf;
}

/**
 * @brief Configure drive strength for a pin
 */
void GPIO_SetDrive(GPIO_Pin_t pin, GPIO_Drive_t drive)
{
    uint32_t cnf = GPIO_REG(GPIO_BASE_ADDR + GPIO_PIN_CNF(pin));
    cnf = (cnf & ~(0x7UL << GPIO_PIN_CNF_DRIVE_Pos)) | (drive << GPIO_PIN_CNF_DRIVE_Pos);
    GPIO_REG(GPIO_BASE_ADDR + GPIO_PIN_CNF(pin)) = cnf;
}

/**
 * @brief Configure sense level for a pin (for wake-up)
 */
void GPIO_SetSense(GPIO_Pin_t pin, GPIO_Sense_t sense)
{
    uint32_t cnf = GPIO_REG(GPIO_BASE_ADDR + GPIO_PIN_CNF(pin));
    cnf = (cnf & ~(0x3UL << GPIO_PIN_CNF_SENSE_Pos)) | (sense << GPIO_PIN_CNF_SENSE_Pos);
    GPIO_REG(GPIO_BASE_ADDR + GPIO_PIN_CNF(pin)) = cnf;
}

/**
 * @brief Write to a GPIO pin (for output pins)
 */
void GPIO_WritePin(GPIO_Pin_t pin, GPIO_State_t state)
{
    if (state == GPIO_STATE_HIGH)
    {
        GPIO_REG(GPIO_BASE_ADDR + GPIO_OUTSET) = (1UL << pin);
    }
    else
    {
        GPIO_REG(GPIO_BASE_ADDR + GPIO_OUTCLR) = (1UL << pin);
    }
}

/**
 * @brief Toggle a GPIO pin (for output pins)
 */
void GPIO_TogglePin(GPIO_Pin_t pin)
{
    uint32_t out = GPIO_REG(GPIO_BASE_ADDR + GPIO_OUT);
    if (out & (1UL << pin))
    {
        GPIO_REG(GPIO_BASE_ADDR + GPIO_OUTCLR) = (1UL << pin);
    }
    else
    {
        GPIO_REG(GPIO_BASE_ADDR + GPIO_OUTSET) = (1UL << pin);
    }
}

/**
 * @brief Read a GPIO pin value
 */
GPIO_State_t GPIO_ReadPin(GPIO_Pin_t pin)
{
    uint32_t in = GPIO_REG(GPIO_BASE_ADDR + GPIO_IN);
    return (GPIO_State_t)((in >> pin) & 0x1UL);
}

/**
 * @brief Write to all GPIO pins in a port
 */
void GPIO_WritePort(uint32_t value)
{
    GPIO_REG(GPIO_BASE_ADDR + GPIO_OUT) = value;
}

/**
 * @brief Read all GPIO pins in a port
 */
uint32_t GPIO_ReadPort(void)
{
    return GPIO_REG(GPIO_BASE_ADDR + GPIO_IN);
}

/**
 * @brief Set multiple pins as output
 */
void GPIO_SetPinsOutput(uint32_t pinMask)
{
    GPIO_REG(GPIO_BASE_ADDR + GPIO_DIRSET) = pinMask;
}

/**
 * @brief Set multiple pins as input
 */
void GPIO_SetPinsInput(uint32_t pinMask)
{
    GPIO_REG(GPIO_BASE_ADDR + GPIO_DIRCLR) = pinMask;
}

/*============================================================================
 * GPIO Interrupt Functions (GPIOTE)
 *============================================================================*/

/**
 * @brief Initialize GPIO interrupts (GPIOTE)
 */
void GPIO_Interrupt_Init(void)
{
    /* Clear all GPIOTE events */
    GPIO_ClearAllInterruptEvents();

    /* Initialize callback pointers to NULL */
    for (int i = 0; i < 8; i++)
    {
        s_InterruptCallbacks[i] = NULL;
        s_InterruptPins[i] = GPIO_PIN_0;
    }
    s_PortInterruptCallback = NULL;
}

/**
 * @brief Configure a GPIO pin interrupt
 */
void GPIO_Interrupt_Config(GPIO_IntPin_t channel, GPIO_Pin_t pin, GPIO_IntTrigger_t trigger)
{
    uint32_t config;

    /* Store the pin number for callback context */
    s_InterruptPins[channel] = pin;

    /* Configure the GPIOTE channel:
     * - Mode: Event mode
     * - PSEL: Pin select
     * - POLARITY: Trigger polarity
     * - INIT: Initial value (not used for events)
     */
    config = (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
             (pin << GPIOTE_CONFIG_PSEL_Pos) |
             (trigger << GPIOTE_CONFIG_POLARITY_Pos);

    GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_CONFIG(channel)) = config;
}

/**
 * @brief Enable interrupt for a GPIOTE channel
 */
void GPIO_Interrupt_Enable(GPIO_IntPin_t channel)
{
    GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_INTENSET) = (1UL << channel);
}

/**
 * @brief Disable interrupt for a GPIOTE channel
 */
void GPIO_Interrupt_Disable(GPIO_IntPin_t channel)
{
    GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_INTENCLR) = (1UL << channel);
}

/**
 * @brief Enable port interrupt (all pins with SENSE configured)
 */
void GPIO_Interrupt_EnablePort(void)
{
    GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_INTENSET) = (1UL << GPIOTE_INT_PORT_Pos);
}

/**
 * @brief Disable port interrupt
 */
void GPIO_Interrupt_DisablePort(void)
{
    GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_INTENCLR) = (1UL << GPIOTE_INT_PORT_Pos);
}

/**
 * @brief Register callback for GPIO interrupt
 */
void GPIO_Interrupt_Callback(GPIO_IntPin_t channel, GPIO_Callback_t callback)
{
    if (channel < 8)
    {
        s_InterruptCallbacks[channel] = callback;
    }
}

/**
 * @brief Register callback for port interrupt
 */
void GPIO_Interrupt_CallbackPort(GPIO_Callback_t callback)
{
    s_PortInterruptCallback = callback;
}

/**
 * @brief Clear all GPIO interrupt events
 */
static void GPIO_ClearAllInterruptEvents(void)
{
    /* Clear all 8 GPIOTE input events */
    for (int i = 0; i < 8; i++)
    {
        GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_EVENTS_IN(i)) = 0;
    }

    /* Clear port event */
    GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_EVENTS_PORT) = 0;
}

/**
 * @brief Clear a specific GPIOTE event
 */
void GPIO_Interrupt_ClearEvent(GPIO_IntPin_t channel)
{
    GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_EVENTS_IN(channel)) = 0;
}

/**
 * @brief Clear port interrupt event
 */
void GPIO_Interrupt_ClearPortEvent(void)
{
    GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_EVENTS_PORT) = 0;
}

/**
 * @brief Check if a GPIOTE channel event is pending
 */
bool GPIO_Interrupt_IsPending(GPIO_IntPin_t channel)
{
    return (GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_EVENTS_IN(channel)) != 0);
}

/**
 * @brief Check if port event is pending
 */
bool GPIO_Interrupt_IsPortPending(void)
{
    return (GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_EVENTS_PORT) != 0);
}

/**
 * @brief Process GPIO interrupt events
 * @description This function checks for GPIO interrupt events and calls the
 *              registered callbacks. Should be called in the interrupt handler
 *              or main loop.
 */
void GPIO_ProcessEvents(void)
{
    /* Check each GPIOTE channel for events */
    for (int i = 0; i < 8; i++)
    {
        if (GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_EVENTS_IN(i)))
        {
            /* Clear the event */
            GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_EVENTS_IN(i)) = 0;

            /* Call the registered callback if any */
            if (s_InterruptCallbacks[i] != NULL)
            {
                s_InterruptCallbacks[i](s_InterruptPins[i]);
            }
        }
    }

    /* Check for port event (multiple pins with SENSE configured) */
    if (GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_EVENTS_PORT))
    {
        /* Clear the event */
        GPIO_REG(GPIOTE_BASE_ADDR + GPIOTE_EVENTS_PORT) = 0;

        /* Call the registered port callback if any */
        if (s_PortInterruptCallback != NULL)
        {
            s_PortInterruptCallback(GPIO_PIN_0);  /* Pin 0 is placeholder for port event */
        }
    }
}

/*============================================================================
 * END OF FILE
 *============================================================================*/
