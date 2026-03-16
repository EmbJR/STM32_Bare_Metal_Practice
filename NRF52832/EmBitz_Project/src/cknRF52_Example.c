/**
 * @file cknRF52_Example.c
 * @brief nRF52832 Clock Driver Example Code
 * @description This file contains example code demonstrating how to use
 *              the nRF52832 Clock driver for various clock operations.
 *
 *              The examples show:
 *              1. Basic HFCLK and LFCLK control
 *              2. Event-driven clock management with callbacks
 *              3. Clock calibration
 *              4. Interrupt-based clock management
 */

/*============================================================================
 * INCLUDES
 *============================================================================*/
#include "cknRF52.h"
#include <stdio.h>

/*============================================================================
 * PRIVATE VARIABLES FOR EXAMPLES
 *============================================================================*/
static volatile bool g_HFCLKStarted = false;
static volatile bool g_LFCLKStarted = false;
static volatile bool g_CalibrationDone = false;

/*============================================================================
 * PRIVATE FUNCTION PROTOTYPES
 *============================================================================*/
static void Example_WaitForHFCLK(void);
static void Example_WaitForLFCLK(void);
static void Example_Delay(volatile uint32_t count);

/*============================================================================
 * PRIVATE VARIABLES
 *============================================================================*/
/* Callback function pointers */
static Clock_Callback_t s_HFCLKStartedCallback = NULL;
static Clock_Callback_t s_LFCLKStartedCallback = NULL;
static Clock_Callback_t s_CalibrationDoneCallback = NULL;
static Clock_Callback_t s_CalibrationTimeoutCallback = NULL;

/*============================================================================
 * CALLBACK FUNCTION IMPLEMENTATIONS
 *============================================================================*/


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
 * @brief Simple delay function
 */
static void Example_Delay(volatile uint32_t count)
{
    while (count--)
    {
        __asm__("nop");  /* No operation - creates delay */
    }
}

/**
 * @brief Wait for HFCLK to start
 */
static void Example_WaitForHFCLK(void)
{
    uint32_t timeout = 100000;
    while (!Clock_HFCLK_IsRunning() && (timeout-- > 0))
    {
        Example_Delay(10);
    }
}

/**
 * @brief Wait for LFCLK to start
 */
static void Example_WaitForLFCLK(void)
{
    uint32_t timeout = 100000;
    while (!Clock_LFCLK_IsRunning() && (timeout-- > 0))
    {
        Example_Delay(10);
    }
}

/*============================================================================
 * EXAMPLE FUNCTIONS
 *============================================================================*/

/**
 * @brief Example 1: Basic Clock Control (Polling Method)
 * @description This example demonstrates basic clock control using polling.
 *              It shows how to start/stop HFCLK and LFCLK and check their status.
 */
void Example_Clock_BasicControl(void)
{
    printf("=== Example 1: Basic Clock Control (Polling) ===\n\n");

    /* Initialize the Clock driver */
    Clock_Init();

    /* Start HFCLK using internal RC oscillator */
    printf("Starting HFCLK (internal RC)...\n");
    Clock_HFCLK_Start();

    /* Wait for HFCLK to start */
    Example_WaitForHFCLK();

    /* Check HFCLK status */
    if (Clock_HFCLK_IsRunning())
    {
        printf("HFCLK is running\n");
        printf("  - Source: %s\n",
               (Clock_HFCLK_GetSource() == CLOCK_HFCLK_SRC_RC) ? "Internal RC" : "External Crystal");
    }
    else
    {
        printf("HFCLK failed to start!\n");
    }

    /* Start LFCLK using external crystal */
    printf("\nStarting LFCLK (external crystal)...\n");
    Clock_LFCLK_Start(CLOCK_LFCLK_SRC_XTAL);

    /* Wait for LFCLK to start */
    Example_WaitForLFCLK();

    /* Check LFCLK status */
    if (Clock_LFCLK_IsRunning())
    {
        printf("LFCLK is running\n");
        printf("  - Source: %s\n",
               (Clock_LFCLK_GetSource() == CLOCK_LFCLK_SRC_XTAL) ? "External Crystal" :
               (Clock_LFCLK_GetSource() == CLOCK_LFCLK_SRC_RC) ? "Internal RC" : "Synthesized");
    }
    else
    {
        printf("LFCLK failed to start!\n");
    }

    /* Stop clocks when done */
    printf("\nStopping clocks...\n");
    Clock_HFCLK_Stop();
    Clock_LFCLK_Stop();

    printf("Basic clock control example complete!\n\n");
}
/*============================================================================
 * MAIN FUNCTION - RUN ALL EXAMPLES
 *============================================================================*/

/**
 * @brief Main function to run all clock examples
 * @return 0 on success
 */
int main(void)
{
    printf("nRF52832 Clock Driver Examples\n");
    printf("==============================\n\n");

    /* Run all examples */
    Example_Clock_BasicControl();

    printf("All examples completed!\n");

    return 0;
}

/*============================================================================
 * END OF FILE
 *============================================================================*/
