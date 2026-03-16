/**
 * @file cknRF52.h
 * @brief nRF52832 Clock Driver Header File
 * @description This file contains the register definitions and function 
 *              declarations for the nRF52832 Clock peripheral driver.
 *              The driver provides simple APIs to control the High-Frequency
 *              Clock (HFCLK) and Low-Frequency Clock (LFCLK).
 */

#ifndef __CK_NRF52_H
#define __CK_NRF52_H

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
/* Base address for Clock peripheral */
#define CLOCK_BASE_ADDR        0x40000000UL

/*============================================================================
 * REGISTER OFFSET DEFINITIONS
 *============================================================================*/
/* Task Registers */
#define CLOCK_TASK_HFCLKSTART  0x000UL
#define CLOCK_TASK_HFCLKSTOP   0x004UL
#define CLOCK_TASK_LFCLKSTART  0x008UL
#define CLOCK_TASK_LFCLKSTOP  0x00CUL
#define CLOCK_TASK_CAL         0x010UL
#define CLOCK_TASK_START       0x014UL
#define CLOCK_TASK_STOP        0x018UL

/* Event Registers */
#define CLOCK_EVENT_HFCLKSTARTED   0x100UL
#define CLOCK_EVENT_LFCLKSTARTED   0x104UL
#define CLOCK_EVENT_DONE            0x10CUL
#define CLOCK_EVENT_CTTO            0x110UL

/* Interrupt Registers */
#define CLOCK_INTENSET     0x304UL
#define CLOCK_INTENCLR     0x308UL

/* Status Registers */
#define CLOCK_HFCLKRUN     0x408UL
#define CLOCK_HFCLKSTAT    0x40CUL
#define CLOCK_LFCLKRUN     0x414UL
#define CLOCK_LFCLKSTAT    0x418UL
#define CLOCK_LFCLKSRCCOPY 0x41CUL

/* Configuration Registers */
#define CLOCK_LFCLKSRC     0x518UL
#define CLOCK_CTIV         0x520UL
#define CLOCK_CTCTL        0x524UL

/*============================================================================
 * REGISTER BIT DEFINITIONS
 *============================================================================*/
/* HFCLKSTAT register bits */
#define CLOCK_HFCLKSTAT_STATE_Pos       0
#define CLOCK_HFCLKSTAT_STATE_Msk       (0xFUL << CLOCK_HFCLKSTAT_STATE_Pos)
#define CLOCK_HFCLKSTAT_STATE_NotRunning 0
#define CLOCK_HFCLKSTAT_STATE_Running   1
#define CLOCK_HFCLKSTAT_STATE_Starting  2

#define CLOCK_HFCLKSTAT_SRC_Pos          4
#define CLOCK_HFCLKSTAT_SRC_Msk          (0xFUL << CLOCK_HFCLKSTAT_SRC_Pos)
#define CLOCK_HFCLKSTAT_SRC_RC           0
#define CLOCK_HFCLKSTAT_SRC_Xtal         1

/* LFCLKSTAT register bits */
#define CLOCK_LFCLKSTAT_STATE_Pos        0
#define CLOCK_LFCLKSTAT_STATE_Msk        (0xFUL << CLOCK_LFCLKSTAT_STATE_Pos)
#define CLOCK_LFCLKSTAT_STATE_NotRunning 0
#define CLOCK_LFCLKSTAT_STATE_Running    1
#define CLOCK_LFCLKSTAT_STATE_Starting   2

#define CLOCK_LFCLKSTAT_SRC_Pos           4
#define CLOCK_LFCLKSTAT_SRC_Msk           (0x3UL << CLOCK_LFCLKSTAT_SRC_Pos)
#define CLOCK_LFCLKSTAT_SRC_RC            0
#define CLOCK_LFCLKSTAT_SRC_Xtal          1
#define CLOCK_LFCLKSTAT_SRC_Synth         2

/* LFCLKSRC register bits */
#define CLOCK_LFCLKSRC_Pos                0
#define CLOCK_LFCLKSRC_Msk                0x3UL
#define CLOCK_LFCLKSRC_RC                 0
#define CLOCK_LFCLKSRC_Xtal               1
#define CLOCK_LFCLKSRC_Synth              2

/* CLOCK_INTENCLR/INTENSET register bits */
#define CLOCK_INT_HFCLKSTARTED_Pos        0
#define CLOCK_INT_HFCLKSTARTED_Msk        (1UL << CLOCK_INT_HFCLKSTARTED_Pos)
#define CLOCK_INT_LFCLKSTARTED_Pos        1
#define CLOCK_INT_LFCLKSTARTED_Msk        (1UL << CLOCK_INT_LFCLKSTARTED_Pos)
#define CLOCK_INT_DONE_Pos                3
#define CLOCK_INT_DONE_Msk                (1UL << CLOCK_INT_DONE_Pos)
#define CLOCK_INT_CTTO_Pos                4
#define CLOCK_INT_CTTO_Msk                (1UL << CLOCK_INT_CTTO_Pos)

/*============================================================================
 * TYPE DEFINITIONS
 *============================================================================*/
/* Clock LFCLK Source Selection */
typedef enum {
    CLOCK_LFCLK_SRC_RC = 0,      /**< Internal 32.768kHz RC oscillator */
    CLOCK_LFCLK_SRC_XTAL = 1,    /**< External 32.768kHz crystal oscillator */
    CLOCK_LFCLK_SRC_SYNTH = 2    /**< Synthesized from HFCLK */
} Clock_LFCLK_Source_t;

/* Clock LFCLK State */
typedef enum {
    CLOCK_LFCLK_STATE_NOT_RUNNING = 0,
    CLOCK_LFCLK_STATE_RUNNING = 1,
    CLOCK_LFCLK_STATE_STARTING = 2
} Clock_LFCLK_State_t;

/* Clock HFCLK State */
typedef enum {
    CLOCK_HFCLK_STATE_NOT_RUNNING = 0,
    CLOCK_HFCLK_STATE_RUNNING = 1,
    CLOCK_HFCLK_STATE_STARTING = 2
} Clock_HFCLK_State_t;

/* Clock HFCLK Source */
typedef enum {
    CLOCK_HFCLK_SRC_RC = 0,      /**< Internal 64MHz RC oscillator */
    CLOCK_HFCLK_SRC_XTAL = 1     /**< External 64MHz crystal oscillator */
} Clock_HFCLK_Source_t;

/* Clock Interrupt Types */
typedef enum {
    CLOCK_INT_HFCLKSTARTED = 0,
    CLOCK_INT_LFCLKSTARTED = 1,
    CLOCK_INT_DONE = 3,
    CLOCK_INT_CTTO = 4
} Clock_Int_t;

/* Clock Callback Function Type */
typedef void (*Clock_Callback_t)(void);

/*============================================================================
 * REGISTER ACCESS MACROS
 *============================================================================*/
/* volatile uint32_t pointer cast macro */
#define CLOCK_REG(addr)        (*(volatile uint32_t *)(addr))

/* Task triggering macros */
#define CLOCK_TASK_TRIGGER(task_addr)    do { CLOCK_REG(CLOCK_BASE_ADDR + task_addr) = 1; } while(0)

/* Event clearing macros */
#define CLOCK_EVENT_CLEAR(event_addr)    do { CLOCK_REG(CLOCK_BASE_ADDR + event_addr) = 0; } while(0)

/* Event reading macros */
#define CLOCK_EVENT_READ(event_addr)     (CLOCK_REG(CLOCK_BASE_ADDR + event_addr))

/*============================================================================
 * API FUNCTION DECLARATIONS
 *============================================================================*/

/**
 * @brief Initialize the Clock driver
 * @description This function initializes the Clock driver. Currently it does not
 *              require any special initialization, but is provided for future use
 *              and consistency.
 * @return None
 */
void Clock_Init(void);

/**
 * @brief Start the High-Frequency Clock (HFCLK)
 * @description This function starts the HFCLK oscillator. The HFCLK can run from
 *              either the internal 64MHz RC oscillator or an external 64MHz crystal.
 *              By default, it starts the internal RC oscillator.
 * @return None
 */
void Clock_HFCLK_Start(void);

/**
 * @brief Stop the High-Frequency Clock (HFCLK)
 * @description This function stops the HFCLK oscillator.
 * @return None
 */
void Clock_HFCLK_Stop(void);

/**
 * @brief Get the current HFCLK state
 * @return Current HFCLK state (NotRunning, Running, or Starting)
 */
Clock_HFCLK_State_t Clock_HFCLK_GetState(void);

/**
 * @brief Get the current HFCLK source
 * @return Current HFCLK source (RC or XTAL)
 */
Clock_HFCLK_Source_t Clock_HFCLK_GetSource(void);

/**
 * @brief Check if HFCLK is running
 * @return true if HFCLK is running, false otherwise
 */
bool Clock_HFCLK_IsRunning(void);

/**
 * @brief Start the Low-Frequency Clock (LFCLK)
 * @description This function starts the LFCLK oscillator. The LFCLK can run from
 *              one of three sources: internal RC oscillator, external crystal, or
 *              synthesized from HFCLK.
 * @param source The LFCLK source to use
 * @return None
 */
void Clock_LFCLK_Start(Clock_LFCLK_Source_t source);

/**
 * @brief Stop the Low-Frequency Clock (LFCLK)
 * @description This function stops the LFCLK oscillator.
 * @return None
 */
void Clock_LFCLK_Stop(void);

/**
 * @brief Get the current LFCLK state
 * @return Current LFCLK state (NotRunning, Running, or Starting)
 */
Clock_LFCLK_State_t Clock_LFCLK_GetState(void);

/**
 * @brief Get the current LFCLK source
 * @return Current LFCLK source
 */
Clock_LFCLK_Source_t Clock_LFCLK_GetSource(void);

/**
 * @brief Check if LFCLK is running
 * @return true if LFCLK is running, false otherwise
 */
bool Clock_LFCLK_IsRunning(void);

/**
 * @brief Configure LFCLK source
 * @description This function configures the LFCLK source. The source is only
 *              changed when the LFCLK is stopped.
 * @param source The desired LFCLK source
 * @return None
 */
void Clock_LFCLK_SetSource(Clock_LFCLK_Source_t source);

/**
 * @brief Enable a Clock interrupt
 * @param interrupt The interrupt to enable
 * @return None
 */
void Clock_Interrupt_Enable(Clock_Int_t interrupt);

/**
 * @brief Disable a Clock interrupt
 * @param interrupt The interrupt to disable
 * @return None
 */
void Clock_Interrupt_Disable(Clock_Int_t interrupt);

/**
 * @brief Check if a Clock interrupt is enabled
 * @param interrupt The interrupt to check
 * @return true if interrupt is enabled, false otherwise
 */
bool Clock_Interrupt_IsEnabled(Clock_Int_t interrupt);

/**
 * @brief Start clock calibration
 * @description This function starts the calibration of the low-frequency
 *              clock source.
 * @return None
 */
void Clock_Calibration_Start(void);

/**
 * @brief Set calibration timer interval
 * @description This function sets the calibration timer interval in units
 *              of 1/4 second (250ms).
 * @param interval Calibration interval (0-255, where 0 = disabled)
 * @return None
 */
void Clock_Calibration_SetInterval(uint8_t interval);

/**
 * @brief Start the calibration timer
 * @description This function starts the calibration timer which periodically
 *              triggers calibration.
 * @return None
 */
void Clock_Calibration_TimerStart(void);

/**
 * @brief Stop the calibration timer
 * @description This function stops the calibration timer.
 * @return None
 */
void Clock_Calibration_TimerStop(void);

/**
 * @brief Get calibration timer interval
 * @return Current calibration interval
 */
uint8_t Clock_Calibration_GetInterval(void);

/**
 * @brief Register callback for HFCLK started event
 * @param callback Function pointer to call when HFCLK started event occurs
 * @return None
 */
void Clock_Callback_HFCLKStarted(Clock_Callback_t callback);

/**
 * @brief Register callback for LFCLK started event
 * @param callback Function pointer to call when LFCLK started event occurs
 * @return None
 */
void Clock_Callback_LFCLKStarted(Clock_Callback_t callback);

/**
 * @brief Register callback for calibration done event
 * @param callback Function pointer to call when calibration done event occurs
 * @return None
 */
void Clock_Callback_CalibrationDone(Clock_Callback_t callback);

/**
 * @brief Register callback for calibration timeout event
 * @param callback Function pointer to call when calibration timeout occurs
 * @return None
 */
void Clock_Callback_CalibrationTimeout(Clock_Callback_t callback);

/**
 * @brief Process Clock events (call this in main loop or interrupt)
 * @description This function checks for Clock events and calls the
 *              registered callbacks. Should be called periodically.
 * @return None
 */
void Clock_ProcessEvents(void);

/*============================================================================
 * DEPRECATED APIs (for backward compatibility)
 *============================================================================*/
#define Clock_StartHFCLK()         Clock_HFCLK_Start()
#define Clock_StopHFCLK()          Clock_HFCLK_Stop()
#define Clock_StartLFCLK(src)      Clock_LFCLK_Start(src)
#define Clock_StopLFCLK()          Clock_LFCLK_Stop()

#ifdef __cplusplus
}
#endif

#endif /* __CK_NRF52_H */
