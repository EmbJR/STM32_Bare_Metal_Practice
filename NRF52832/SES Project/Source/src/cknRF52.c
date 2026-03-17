/**
 * @file cknRF52.c
 * @brief nRF52832 Clock Driver Source File
 * @description This file contains the implementation of the nRF52832 Clock
 *              peripheral driver. It provides simple APIs to control the
 *              High-Frequency Clock (HFCLK) and Low-Frequency Clock (LFCLK).
 */

/*============================================================================
 * INCLUDES
 *============================================================================*/
#include "cknRF52.h"
#include "Common.h"

/*============================================================================
 * PRIVATE VARIABLES
 *============================================================================*/
/* Callback function pointers */
static Clock_Callback_t s_HFCLKStartedCallback = NULL;
static Clock_Callback_t s_LFCLKStartedCallback = NULL;
static Clock_Callback_t s_CalibrationDoneCallback = NULL;
static Clock_Callback_t s_CalibrationTimeoutCallback = NULL;

/*============================================================================
 * PRIVATE FUNCTION PROTOTYPES
 *============================================================================*/
static void Clock_ClearAllEvents(void);

/*============================================================================
 * API FUNCTION IMPLEMENTATIONS
 *============================================================================*/

/**
 * @brief Initialize the Clock driver
 */
void Clock_Init(void)
{
    /* Clear all events */
    Clock_ClearAllEvents();

    /* Initialize callback pointers to NULL */
    s_HFCLKStartedCallback = NULL;
    s_LFCLKStartedCallback = NULL;
    s_CalibrationDoneCallback = NULL;
    s_CalibrationTimeoutCallback = NULL;
}

/**
 * @brief Start the High-Frequency Clock (HFCLK)
 */
void Clock_HFCLK_Start(void)
{
    CLOCK_TASK_TRIGGER(CLOCK_TASK_HFCLKSTART);
}

/**
 * @brief Stop the High-Frequency Clock (HFCLK)
 */
void Clock_HFCLK_Stop(void)
{
    CLOCK_TASK_TRIGGER(CLOCK_TASK_HFCLKSTOP);
}

/**
 * @brief Get the current HFCLK state
 */
Clock_HFCLK_State_t Clock_HFCLK_GetState(void)
{
    uint32_t status = CLOCK_REG(CLOCK_BASE_ADDR + CLOCK_HFCLKSTAT);
    uint32_t state = (status & CLOCK_HFCLKSTAT_STATE_Msk) >> CLOCK_HFCLKSTAT_STATE_Pos;
    return (Clock_HFCLK_State_t)state;
}

/**
 * @brief Get the current HFCLK source
 */
Clock_HFCLK_Source_t Clock_HFCLK_GetSource(void)
{
    uint32_t status = CLOCK_REG(CLOCK_BASE_ADDR + CLOCK_HFCLKSTAT);
    uint32_t src = (status & CLOCK_HFCLKSTAT_SRC_Msk) >> CLOCK_HFCLKSTAT_SRC_Pos;
    return (Clock_HFCLK_Source_t)src;
}

/**
 * @brief Check if HFCLK is running
 */
bool Clock_HFCLK_IsRunning(void)
{
    return (Clock_HFCLK_GetState() == CLOCK_HFCLK_STATE_RUNNING);
}

/**
 * @brief Start the Low-Frequency Clock (LFCLK)
 */
void Clock_LFCLK_Start(Clock_LFCLK_Source_t source)
{
    /* Set the LFCLK source before starting */
    Clock_LFCLK_SetSource(source);

    /* Start the LFCLK */
    CLOCK_TASK_TRIGGER(CLOCK_TASK_LFCLKSTART);
}

/**
 * @brief Stop the Low-Frequency Clock (LFCLK)
 */
void Clock_LFCLK_Stop(void)
{
    CLOCK_TASK_TRIGGER(CLOCK_TASK_LFCLKSTOP);
}

/**
 * @brief Get the current LFCLK state
 */
Clock_LFCLK_State_t Clock_LFCLK_GetState(void)
{
    uint32_t status = CLOCK_REG(CLOCK_BASE_ADDR + CLOCK_LFCLKSTAT);
    uint32_t state = (status & CLOCK_LFCLKSTAT_STATE_Msk) >> CLOCK_LFCLKSTAT_STATE_Pos;
    return (Clock_LFCLK_State_t)state;
}

/**
 * @brief Get the current LFCLK source
 */
Clock_LFCLK_Source_t Clock_LFCLK_GetSource(void)
{
    uint32_t src = CLOCK_REG(CLOCK_BASE_ADDR + CLOCK_LFCLKSRC) & CLOCK_LFCLKSRC_Msk;
    return (Clock_LFCLK_Source_t)src;
}

/**
 * @brief Check if LFCLK is running
 */
bool Clock_LFCLK_IsRunning(void)
{
    return (Clock_LFCLK_GetState() == CLOCK_LFCLK_STATE_RUNNING);
}

/**
 * @brief Configure LFCLK source
 */
void Clock_LFCLK_SetSource(Clock_LFCLK_Source_t source)
{
    /* Only configure when LFCLK is not running */
    if (!Clock_LFCLK_IsRunning())
    {
        uint32_t reg = CLOCK_REG(CLOCK_BASE_ADDR + CLOCK_LFCLKSRC);
        reg = (reg & ~CLOCK_LFCLKSRC_Msk) | (source & CLOCK_LFCLKSRC_Msk);
        CLOCK_REG(CLOCK_BASE_ADDR + CLOCK_LFCLKSRC) = reg;
    }
}

/**
 * @brief Enable a Clock interrupt
 */
void Clock_Interrupt_Enable(Clock_Int_t interrupt)
{
    CLOCK_REG(CLOCK_BASE_ADDR + CLOCK_INTENSET) = (1UL << interrupt);
}

/**
 * @brief Disable a Clock interrupt
 */
void Clock_Interrupt_Disable(Clock_Int_t interrupt)
{
    CLOCK_REG(CLOCK_BASE_ADDR + CLOCK_INTENCLR) = (1UL << interrupt);
}

/**
 * @brief Check if a Clock interrupt is enabled
 */
bool Clock_Interrupt_IsEnabled(Clock_Int_t interrupt)
{
    uint32_t reg = CLOCK_REG(CLOCK_BASE_ADDR + CLOCK_INTENCLR);
    return ((reg & (1UL << interrupt)) != 0);
}

/**
 * @brief Start clock calibration
 */
void Clock_Calibration_Start(void)
{
    CLOCK_TASK_TRIGGER(CLOCK_TASK_CAL);
}

/**
 * @brief Set calibration timer interval
 */
void Clock_Calibration_SetInterval(uint8_t interval)
{
    CLOCK_REG(CLOCK_BASE_ADDR + CLOCK_CTIV) = interval;
}

/**
 * @brief Start the calibration timer
 */
void Clock_Calibration_TimerStart(void)
{
    CLOCK_TASK_TRIGGER(CLOCK_TASK_START);
}

/**
 * @brief Stop the calibration timer
 */
void Clock_Calibration_TimerStop(void)
{
    CLOCK_TASK_TRIGGER(CLOCK_TASK_STOP);
}

/**
 * @brief Get calibration timer interval
 */
uint8_t Clock_Calibration_GetInterval(void)
{
    return (uint8_t)(CLOCK_REG(CLOCK_BASE_ADDR + CLOCK_CTIV) & 0xFF);
}

/**
 * @brief Register callback for HFCLK started event
 */
void Clock_Callback_HFCLKStarted(Clock_Callback_t callback)
{
    s_HFCLKStartedCallback = callback;
}

/**
 * @brief Register callback for LFCLK started event
 */
void Clock_Callback_LFCLKStarted(Clock_Callback_t callback)
{
    s_LFCLKStartedCallback = callback;
}

/**
 * @brief Register callback for calibration done event
 */
void Clock_Callback_CalibrationDone(Clock_Callback_t callback)
{
    s_CalibrationDoneCallback = callback;
}

/**
 * @brief Register callback for calibration timeout event
 */
void Clock_Callback_CalibrationTimeout(Clock_Callback_t callback)
{
    s_CalibrationTimeoutCallback = callback;
}

/**
 * @brief Clear all Clock events
 */
static void Clock_ClearAllEvents(void)
{
    CLOCK_EVENT_CLEAR(CLOCK_EVENT_HFCLKSTARTED);
    CLOCK_EVENT_CLEAR(CLOCK_EVENT_LFCLKSTARTED);
    CLOCK_EVENT_CLEAR(CLOCK_EVENT_DONE);
    CLOCK_EVENT_CLEAR(CLOCK_EVENT_CTTO);
}

/**
 * @brief Process Clock events
 */
void Clock_ProcessEvents(void)
{
    /* Check HFCLK started event */
    if (CLOCK_EVENT_READ(CLOCK_EVENT_HFCLKSTARTED))
    {
        CLOCK_EVENT_CLEAR(CLOCK_EVENT_HFCLKSTARTED);
        if (s_HFCLKStartedCallback != NULL)
        {
            s_HFCLKStartedCallback();
        }
    }

    /* Check LFCLK started event */
    if (CLOCK_EVENT_READ(CLOCK_EVENT_LFCLKSTARTED))
    {
        CLOCK_EVENT_CLEAR(CLOCK_EVENT_LFCLKSTARTED);
        if (s_LFCLKStartedCallback != NULL)
        {
            s_LFCLKStartedCallback();
        }
    }

    /* Check calibration done event */
    if (CLOCK_EVENT_READ(CLOCK_EVENT_DONE))
    {
        CLOCK_EVENT_CLEAR(CLOCK_EVENT_DONE);
        if (s_CalibrationDoneCallback != NULL)
        {
            s_CalibrationDoneCallback();
        }
    }

    /* Check calibration timeout event */
    if (CLOCK_EVENT_READ(CLOCK_EVENT_CTTO))
    {
        CLOCK_EVENT_CLEAR(CLOCK_EVENT_CTTO);
        if (s_CalibrationTimeoutCallback != NULL)
        {
            s_CalibrationTimeoutCallback();
        }
    }
}

/*============================================================================
 * END OF FILE
 *============================================================================*/
