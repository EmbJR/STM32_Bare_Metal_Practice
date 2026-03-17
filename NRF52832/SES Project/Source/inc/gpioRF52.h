/**
 * @file gpioRF52.h
 * @brief nRF52832 GPIO Driver Header File
 * @description This file contains the register definitions and function 
 *              declarations for the nRF52832 GPIO peripheral driver.
 *              The driver provides simple APIs to control GPIO pins, including
 *              input/output configuration, pull-up/pull-down resistors,
 *              drive strength, and external interrupts.
 */

#ifndef __GPIO_NRF52_H
#define __GPIO_NRF52_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 * INCLUDES
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>

/*============================================================================
 * BASE ADDRESS DEFINITIONS
 *============================================================================*/
/* Base address for GPIO peripheral */
#define GPIO_BASE_ADDR        0x50000000UL

/*============================================================================
 * REGISTER OFFSET DEFINITIONS
 *============================================================================*/
/* Output Register */
#define GPIO_OUT              0x504UL
#define GPIO_OUTSET           0x508UL
#define GPIO_OUTCLR           0x50CUL

/* Input Register */
#define GPIO_IN               0x510UL

/* Direction Register */
#define GPIO_DIR              0x514UL
#define GPIO_DIRSET           0x518UL
#define GPIO_DIRCLR           0x51CUL

/* Pin Configuration Registers (0-31) */
#define GPIO_PIN_CNF(n)       (0x700UL + (n) * 4UL)

/*============================================================================
 * INTERRUPT REGISTER OFFSETS (GPIOTE)
 *============================================================================*/
/* Note: GPIOTE is a separate peripheral at base 0x40006000 */
#define GPIOTE_BASE_ADDR      0x40006000UL

/* GPIOTE Tasks */
#define GPIOTE_TASK_OUT(n)    ((n) * 0x04UL)
#define GPIOTE_TASK_CLR(n)    (0x30UL + (n) * 0x04UL)

/* GPIOTE Events */
#define GPIOTE_EVENTS_IN(n)   ((n) * 0x04UL)
#define GPIOTE_EVENTS_PORT    0x13CUL

/* GPIOTE Registers */
#define GPIOTE_INTENSET       0x304UL
#define GPIOTE_INTENCLR       0x308UL
#define GPIOTE_CONFIG(n)      (0x510UL + (n) * 0x04UL)

/*============================================================================
 * REGISTER BIT DEFINITIONS
 *============================================================================*/
/* PIN_CNF register bits */
#define GPIO_PIN_CNF_DIR_Pos          0
#define GPIO_PIN_CNF_DIR_Input        0
#define GPIO_PIN_CNF_DIR_Output       1

#define GPIO_PIN_CNF_INPUT_Pos        1
#define GPIO_PIN_CNF_INPUT_Connect    0
#define GPIO_PIN_CNF_INPUT_Disconnect 1

#define GPIO_PIN_CNF_PULL_Pos         2
#define GPIO_PIN_CNF_PULL_Disabled    0
#define GPIO_PIN_CNF_PULL_Pulldown    1
#define GPIO_PIN_CNF_PULL_Pullup      3

#define GPIO_PIN_CNF_DRIVE_Pos        8
#define GPIO_PIN_CNF_DRIVE_S0S1       0   /* Standard 0, Standard 1 */
#define GPIO_PIN_CNF_DRIVE_H0S1       1   /* High drive 0, Standard 1 */
#define GPIO_PIN_CNF_DRIVE_S0H1       2   /* Standard 0, High drive 1 */
#define GPIO_PIN_CNF_DRIVE_H0H1       3   /* High drive 0, High drive 1 */
#define GPIO_PIN_CNF_DRIVE_D0S1       4   /* Disconnect 0, Standard 1 */
#define GPIO_PIN_CNF_DRIVE_D0H1       5   /* Disconnect 0, High drive 1 */
#define GPIO_PIN_CNF_DRIVE_S0D1       6   /* Standard 0, Disconnect 1 */
#define GPIO_PIN_CNF_DRIVE_H0D1       7   /* High drive 0, Disconnect 1 */

#define GPIO_PIN_CNF_SENSE_Pos        16
#define GPIO_PIN_CNF_SENSE_Disabled   0
#define GPIO_PIN_CNF_SENSE_High       2
#define GPIO_PIN_CNF_SENSE_Low        3

/* GPIOTE CONFIG register bits */
#define GPIOTE_CONFIG_MODE_Pos        0
#define GPIOTE_CONFIG_MODE_Event      0
#define GPIOTE_CONFIG_MODE_Task       1

#define GPIOTE_CONFIG_PSEL_Pos        8
#define GPIOTE_CONFIG_POLARITY_Pos    16
#define GPIOTE_CONFIG_POLARITY_None    0
#define GPIOTE_CONFIG_POLARITY_LoToHi 1
#define GPIOTE_CONFIG_POLARITY_HiToLo 2
#define GPIOTE_CONFIG_POLARITY_Toggle 3

#define GPIOTE_CONFIG_INIT_Pos        20

/* Interrupt bits */
#define GPIOTE_INT_IN0_Pos     0
#define GPIOTE_INT_IN1_Pos     1
#define GPIOTE_INT_IN2_Pos     2
#define GPIOTE_INT_IN3_Pos     3
#define GPIOTE_INT_IN4_Pos     4
#define GPIOTE_INT_IN5_Pos     5
#define GPIOTE_INT_IN6_Pos     6
#define GPIOTE_INT_IN7_Pos     7
#define GPIOTE_INT_PORT_Pos    31

/*============================================================================
 * TYPE DEFINITIONS
 *============================================================================*/

/* GPIO Pin Number (0-31) */
typedef enum {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1 = 1,
    GPIO_PIN_2 = 2,
    GPIO_PIN_3 = 3,
    GPIO_PIN_4 = 4,
    GPIO_PIN_5 = 5,
    GPIO_PIN_6 = 6,
    GPIO_PIN_7 = 7,
    GPIO_PIN_8 = 8,
    GPIO_PIN_9 = 9,
    GPIO_PIN_10 = 10,
    GPIO_PIN_11 = 11,
    GPIO_PIN_12 = 12,
    GPIO_PIN_13 = 13,
    GPIO_PIN_14 = 14,
    GPIO_PIN_15 = 15,
    GPIO_PIN_16 = 16,
    GPIO_PIN_17 = 17,
    GPIO_PIN_18 = 18,
    GPIO_PIN_19 = 19,
    GPIO_PIN_20 = 20,
    GPIO_PIN_21 = 21,
    GPIO_PIN_22 = 22,
    GPIO_PIN_23 = 23,
    GPIO_PIN_24 = 24,
    GPIO_PIN_25 = 25,
    GPIO_PIN_26 = 26,
    GPIO_PIN_27 = 27,
    GPIO_PIN_28 = 28,
    GPIO_PIN_29 = 29,
    GPIO_PIN_30 = 30,
    GPIO_PIN_31 = 31
} GPIO_Pin_t;

/* GPIO Pin Direction */
typedef enum {
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT = 1
} GPIO_Direction_t;

/* GPIO Pull Configuration */
typedef enum {
    GPIO_PULL_DISABLED = 0,
    GPIO_PULL_PULLDOWN = 1,
    GPIO_PULL_PULLUP = 3
} GPIO_Pull_t;

/* GPIO Drive Strength */
typedef enum {
    GPIO_DRIVE_S0S1 = 0,      /* Standard 0, Standard 1 */
    GPIO_DRIVE_H0S1 = 1,      /* High drive 0, Standard 1 */
    GPIO_DRIVE_S0H1 = 2,      /* Standard 0, High drive 1 */
    GPIO_DRIVE_H0H1 = 3,      /* High drive 0, High drive 1 */
    GPIO_DRIVE_D0S1 = 4,      /* Disconnect 0, Standard 1 */
    GPIO_DRIVE_D0H1 = 5,      /* Disconnect 0, High drive 1 */
    GPIO_DRIVE_S0D1 = 6,      /* Standard 0, Disconnect 1 */
    GPIO_DRIVE_H0D1 = 7       /* High drive 0, Disconnect 1 */
} GPIO_Drive_t;

/* GPIO Sense Level */
typedef enum {
    GPIO_SENSE_DISABLED = 0,
    GPIO_SENSE_HIGH = 2,
    GPIO_SENSE_LOW = 3
} GPIO_Sense_t;

/* GPIO Pin State */
typedef enum {
    GPIO_STATE_LOW = 0,
    GPIO_STATE_HIGH = 1
} GPIO_State_t;

/* GPIO Interrupt Trigger Type */
typedef enum {
    GPIO_INT_TRIGGER_NONE = 0,
    GPIO_INT_TRIGGER_RISING = 1,
    GPIO_INT_TRIGGER_FALLING = 2,
    GPIO_INT_TRIGGER_TOGGLE = 3
} GPIO_IntTrigger_t;

/* GPIO Interrupt Pin (for GPIOTE channels 0-7) */
typedef enum {
    GPIO_INT_PIN_0 = 0,
    GPIO_INT_PIN_1 = 1,
    GPIO_INT_PIN_2 = 2,
    GPIO_INT_PIN_3 = 3,
    GPIO_INT_PIN_4 = 4,
    GPIO_INT_PIN_5 = 5,
    GPIO_INT_PIN_6 = 6,
    GPIO_INT_PIN_7 = 7
} GPIO_IntPin_t;

/* GPIO Interrupt Callback Function Type */
typedef void (*GPIO_Callback_t)(GPIO_Pin_t pin);

/*============================================================================
 * REGISTER ACCESS MACROS
 *============================================================================*/
/* volatile uint32_t pointer cast macro */
#define GPIO_REG(addr)        (*(volatile uint32_t *)(addr))

/* GPIO Output macros */
#define GPIO_WRITE_PIN(port, pin, value)  \
    do { \
        if (value) \
            GPIO_REG(GPIO_BASE_ADDR + GPIO_OUTSET) = (1UL << pin); \
        else \
            GPIO_REG(GPIO_BASE_ADDR + GPIO_OUTCLR) = (1UL << pin); \
    } while(0)

#define GPIO_READ_PIN(port, pin)  \
    ((GPIO_REG(GPIO_BASE_ADDR + GPIO_IN) >> pin) & 0x1UL)

/*============================================================================
 * API FUNCTION DECLARATIONS
 *============================================================================*/

/*============================================================================
 * Basic GPIO Functions
 *============================================================================*/

/**
 * @brief Initialize the GPIO driver
 * @description This function initializes the GPIO peripheral. Currently it does
 *              not require any special initialization, but is provided for 
 *              future use and consistency.
 * @return None
 */
void GPIO_Init(void);

/**
 * @brief Configure a GPIO pin
 * @param pin The pin number (0-31)
 * @param direction The pin direction (input or output)
 * @param pull The pull configuration (disabled, pulldown, or pullup)
 * @return None
 */
void GPIO_ConfigPin(GPIO_Pin_t pin, GPIO_Direction_t direction, GPIO_Pull_t pull);

/**
 * @brief Set GPIO pin direction
 * @param pin The pin number (0-31)
 * @param direction The pin direction (input or output)
 * @return None
 */
void GPIO_SetDirection(GPIO_Pin_t pin, GPIO_Direction_t direction);

/**
 * @brief Get GPIO pin direction
 * @param pin The pin number (0-31)
 * @return The current pin direction
 */
GPIO_Direction_t GPIO_GetDirection(GPIO_Pin_t pin);

/**
 * @brief Configure pull-up/pull-down for a pin
 * @param pin The pin number (0-31)
 * @param pull The pull configuration
 * @return None
 */
void GPIO_SetPull(GPIO_Pin_t pin, GPIO_Pull_t pull);

/**
 * @brief Configure drive strength for a pin
 * @param pin The pin number (0-31)
 * @param drive The drive strength configuration
 * @return None
 */
void GPIO_SetDrive(GPIO_Pin_t pin, GPIO_Drive_t drive);

/**
 * @brief Configure sense level for a pin (for wake-up)
 * @param pin The pin number (0-31)
 * @param sense The sense configuration
 * @return None
 */
void GPIO_SetSense(GPIO_Pin_t pin, GPIO_Sense_t sense);

/**
 * @brief Write to a GPIO pin (for output pins)
 * @param pin The pin number (0-31)
 * @param state The pin state (high or low)
 * @return None
 */
void GPIO_WritePin(GPIO_Pin_t pin, GPIO_State_t state);

/**
 * @brief Toggle a GPIO pin (for output pins)
 * @param pin The pin number (0-31)
 * @return None
 */
void GPIO_TogglePin(GPIO_Pin_t pin);

/**
 * @brief Read a GPIO pin value
 * @param pin The pin number (0-31)
 * @return The current pin state (high or low)
 */
GPIO_State_t GPIO_ReadPin(GPIO_Pin_t pin);

/**
 * @brief Write to all GPIO pins in a port
 * @param value The value to write to all 32 pins
 * @return None
 */
void GPIO_WritePort(uint32_t value);

/**
 * @brief Read all GPIO pins in a port
 * @return The current value of all 32 pins
 */
uint32_t GPIO_ReadPort(void);

/**
 * @brief Set multiple pins as output
 * @param pinMask Bitmask of pins to set as output (bit 0 = pin 0, etc.)
 * @return None
 */
void GPIO_SetPinsOutput(uint32_t pinMask);

/**
 * @brief Set multiple pins as input
 * @param pinMask Bitmask of pins to set as input
 * @return None
 */
void GPIO_SetPinsInput(uint32_t pinMask);

/*============================================================================
 * GPIO Interrupt Functions (GPIOTE)
 *============================================================================*/

/**
 * @brief Initialize GPIO interrupts (GPIOTE)
 * @description This function initializes the GPIOTE peripheral for interrupts.
 * @return None
 */
void GPIO_Interrupt_Init(void);

/**
 * @brief Configure a GPIO pin interrupt
 * @param channel The GPIOTE channel (0-7)
 * @param pin The GPIO pin to trigger interrupt
 * @param trigger The trigger type (rising, falling, or toggle)
 * @return None
 */
void GPIO_Interrupt_Config(GPIO_IntPin_t channel, GPIO_Pin_t pin, GPIO_IntTrigger_t trigger);

/**
 * @brief Enable interrupt for a GPIOTE channel
 * @param channel The GPIOTE channel (0-7)
 * @return None
 */
void GPIO_Interrupt_Enable(GPIO_IntPin_t channel);

/**
 * @brief Disable interrupt for a GPIOTE channel
 * @param channel The GPIOTE channel (0-7)
 * @return None
 */
void GPIO_Interrupt_Disable(GPIO_IntPin_t channel);

/**
 * @brief Enable port interrupt (all pins with SENSE configured)
 * @return None
 */
void GPIO_Interrupt_EnablePort(void);

/**
 * @brief Disable port interrupt
 * @return None
 */
void GPIO_Interrupt_DisablePort(void);

/**
 * @brief Register callback for GPIO interrupt
 * @param channel The GPIOTE channel (0-7)
 * @param callback Function pointer to call when interrupt occurs
 * @return None
 */
void GPIO_Interrupt_Callback(GPIO_IntPin_t channel, GPIO_Callback_t callback);

/**
 * @brief Register callback for port interrupt
 * @param callback Function pointer to call when port interrupt occurs
 * @return None
 */
void GPIO_Interrupt_CallbackPort(GPIO_Callback_t callback);

/**
 * @brief Process GPIO interrupt events
 * @description This function checks for GPIO interrupt events and calls the
 *              registered callbacks. Should be called in the interrupt handler
 *              or main loop.
 * @return None
 */
void GPIO_ProcessEvents(void);

/**
 * @brief Clear a specific GPIOTE event
 * @param channel The GPIOTE channel (0-7)
 * @return None
 */
void GPIO_Interrupt_ClearEvent(GPIO_IntPin_t channel);

/**
 * @brief Clear port interrupt event
 * @return None
 */
void GPIO_Interrupt_ClearPortEvent(void);

/**
 * @brief Check if a GPIOTE channel event is pending
 * @param channel The GPIOTE channel (0-7)
 * @return true if event is pending, false otherwise
 */
bool GPIO_Interrupt_IsPending(GPIO_IntPin_t channel);

/**
 * @brief Check if port event is pending
 * @return true if port event is pending, false otherwise
 */
bool GPIO_Interrupt_IsPortPending(void);

/*============================================================================
 * END OF HEADER FILE
 *============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_NRF52_H */
