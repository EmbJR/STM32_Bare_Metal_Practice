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
static void Example_HFCLKStartedCallback(void);
static void Example_LFCLKStartedCallback(void);
static void Example_CalibrationDoneCallback(void);
static void Example_WaitForHFCLK(void);
static void Example_WaitForLFCLK(void);
static void Example_Delay(volatile uint32_t count);

/*============================================================================
 * CALLBACK FUNCTION IMPLEMENTATIONS
 *============================================================================*/

/**
 * @brief Callback function for HFCLK started event
 */
static void Example_HFCLKStartedCallback(void)
{
    g_HFCLKStarted = true;
    /* HFCLK is now running - can perform operations requiring HFCLK */
}

/**
 * @brief Callback function for LFCLK started event
 */
static void Example_LFCLKStartedCallback(void)
{
    g_LFCLKStarted = true;
    /* LFCLK is now running - can perform operations requiring LFCLK */
}

/**
 * @brief Callback function for calibration done event
 */
static void Example_CalibrationDoneCallback(void)
{
    g_CalibrationDone = true;
    /* Calibration completed successfully */
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

/**
 * @brief Example 2: Clock Control with Callbacks (Event-Driven)
 * @description This example demonstrates event-driven clock management using
 *              callback functions instead of polling.
 */
void Example_Clock_WithCallbacks(void)
{
    printf("=== Example 2: Clock Control with Callbacks ===\n\n");
    
    /* Initialize the Clock driver */
    Clock_Init();
    
    /* Register callbacks for clock events */
    Clock_Callback_HFCLKStarted(Example_HFCLKStartedCallback);
    Clock_Callback_LFCLKStarted(Example_LFCLKStartedCallback);
    
    /* Start HFCLK */
    printf("Starting HFCLK with callback...\n");
    Clock_HFCLK_Start();
    
    /* Wait for callback to be invoked */
    while (!g_HFCLKStarted)
    {
        Clock_ProcessEvents();  /* Process any pending events */
    }
    printf("HFCLK callback received!\n");
    
    /* Start LFCLK */
    printf("\nStarting LFCLK with callback...\n");
    Clock_LFCLK_Start(CLOCK_LFCLK_SRC_RC);
    
    /* Wait for callback to be invoked */
    while (!g_LFCLKStarted)
    {
        Clock_ProcessEvents();  /* Process any pending events */
    }
    printf("LFCLK callback received!\n");
    
    /* Stop clocks */
    Clock_HFCLK_Stop();
    Clock_LFCLK_Stop();
    
    printf("Clock control with callbacks example complete!\n\n");
}

/**
 * @brief Example 3: Clock Configuration
 * @description This example demonstrates various clock configuration options.
 */
void Example_Clock_Configuration(void)
{
    printf("=== Example 3: Clock Configuration ===\n\n");
    
    /* Initialize the Clock driver */
    Clock_Init();
    
    /* Configure LFCLK source before starting */
    printf("Configuring LFCLK source to Internal RC...\n");
    Clock_LFCLK_SetSource(CLOCK_LFCLK_SRC_RC);
    
    /* Start and verify LFCLK */
    Clock_LFCLK_Start(CLOCK_LFCLK_SRC_RC);
    Example_WaitForLFCLK();
    
    printf("LFCLK running with: %s\n",
           (Clock_LFCLK_GetSource() == CLOCK_LFCLK_SRC_RC) ? "Internal RC" :
           (Clock_LFCLK_GetSource() == CLOCK_LFCLK_SRC_XTAL) ? "External Crystal" : "Synthesized");
    
    /* Change LFCLK source (must stop first) */
    printf("\nChanging LFCLK source to External Crystal...\n");
    Clock_LFCLK_Stop();
    
    /* Small delay to ensure clock stops */
    Example_Delay(1000);
    
    Clock_LFCLK_SetSource(CLOCK_LFCLK_SRC_XTAL);
    Clock_LFCLK_Start(CLOCK_LFCLK_SRC_XTAL);
    Example_WaitForLFCLK();
    
    printf("LFCLK now running with: %s\n",
           (Clock_LFCLK_GetSource() == CLOCK_LFCLK_SRC_RC) ? "Internal RC" :
           (Clock_LFCLK_GetSource() == CLOCK_LFCLK_SRC_XTAL) ? "External Crystal" : "Synthesized");
    
    /* Stop clocks */
    Clock_HFCLK_Stop();
    Clock_LFCLK_Stop();
    
    printf("Clock configuration example complete!\n\n");
}

/**
 * @brief Example 4: Clock Calibration
 * @description This example demonstrates clock calibration functionality.
 */
void Example_Clock_Calibration(void)
{
    printf("=== Example 4: Clock Calibration ===\n\n");
    
    /* Initialize the Clock driver */
    Clock_Init();
    
    /* Register calibration done callback */
    Clock_Callback_CalibrationDone(Example_CalibrationDoneCallback);
    
    /* Enable calibration done interrupt */
    Clock_Interrupt_Enable(CLOCK_INT_DONE);
    
    /* Start HFCLK and LFCLK first */
    printf("Starting clocks for calibration...\n");
    Clock_HFCLK_Start();
    Clock_LFCLK_Start(CLOCK_LFCLK_SRC_RC);
    
    Example_WaitForHFCLK();
    Example_WaitForLFCLK();
    
    /* Set calibration interval (in 1/4 seconds) */
    printf("Setting calibration interval to 4 seconds...\n");
    Clock_Calibration_SetInterval(16);  /* 16 * 250ms = 4 seconds */
    
    printf("Calibration interval: %d (1/4 seconds)\n", Clock_Calibration_GetInterval());
    
    /* Start manual calibration */
    printf("\nStarting manual calibration...\n");
    Clock_Calibration_Start();
    
    /* Wait for calibration to complete */
    uint32_t timeout = 1000000;
    while (!g_CalibrationDone && (timeout-- > 0))
    {
        Clock_ProcessEvents();
    }
    
    if (g_CalibrationDone)
    {
        printf("Calibration completed successfully!\n");
    }
    else
    {
        printf("Calibration timed out!\n");
    }
    
    /* Example: Using automatic calibration timer */
    printf("\nStarting automatic calibration timer...\n");
    g_CalibrationDone = false;
    Clock_Calibration_TimerStart();
    
    /* Wait for first calibration */
    timeout = 1000000;
    while (!g_CalibrationDone && (timeout-- > 0))
    {
        Clock_ProcessEvents();
    }
    
    if (g_CalibrationDone)
    {
        printf("Automatic calibration triggered!\n");
    }
    
    /* Stop calibration timer */
    Clock_Calibration_TimerStop();
    
    /* Disable calibration interrupt */
    Clock_Interrupt_Disable(CLOCK_INT_DONE);
    
    /* Stop clocks */
    Clock_HFCLK_Stop();
    Clock_LFCLK_Stop();
    
    printf("Clock calibration example complete!\n\n");
}

/**
 * @brief Example 5: Interrupt-Based Clock Management
 * @description This example demonstrates using interrupts for clock events.
 */
void Example_Clock_Interrupts(void)
{
    printf("=== Example 5: Interrupt-Based Clock Management ===\n\n");
    
    /* Initialize the Clock driver */
    Clock_Init();
    
    /* Register callbacks */
    Clock_Callback_HFCLKStarted(Example_HFCLKStartedCallback);
    Clock_Callback_LFCLKStarted(Example_LFCLKStartedCallback);
    
    /* Enable clock interrupts */
    printf("Enabling HFCLK and LFCLK started interrupts...\n");
    Clock_Interrupt_Enable(CLOCK_INT_HFCLKSTARTED);
    Clock_Interrupt_Enable(CLOCK_INT_LFCLKSTARTED);
    
    /* Verify interrupts are enabled */
    printf("HFCLKSTARTED interrupt enabled: %s\n", 
           Clock_Interrupt_IsEnabled(CLOCK_INT_HFCLKSTARTED) ? "Yes" : "No");
    printf("LFCLKSTARTED interrupt enabled: %s\n", 
           Clock_Interrupt_IsEnabled(CLOCK_INT_LFCLKSTARTED) ? "Yes" : "No");
    
    /* Start clocks */
    printf("\nStarting clocks...\n");
    Clock_HFCLK_Start();
    Clock_LFCLK_Start(CLOCK_LFCLK_SRC_XTAL);
    
    /* Wait for interrupts/callbacks */
    uint32_t timeout = 100000;
    while ((!g_HFCLKStarted || !g_LFCLKStarted) && (timeout-- > 0))
    {
        Clock_ProcessEvents();
    }
    
    if (g_HFCLKStarted && g_LFCLKStarted)
    {
        printf("Both clocks started via interrupts!\n");
    }
    
    /* Disable interrupts when done */
    Clock_Interrupt_Disable(CLOCK_INT_HFCLKSTARTED);
    Clock_Interrupt_Disable(CLOCK_INT_LFCLKSTARTED);
    
    /* Stop clocks */
    Clock_HFCLK_Stop();
    Clock_LFCLK_Stop();
    
    printf("Interrupt-based clock management example complete!\n\n");
}

/**
 * @brief Example 6: Clock Status Monitoring
 * @description This example demonstrates continuous clock status monitoring.
 */
void Example_Clock_StatusMonitoring(void)
{
    printf("=== Example 6: Clock Status Monitoring ===\n\n");
    
    /* Initialize the Clock driver */
    Clock_Init();
    
    /* Monitor clock states before starting */
    printf("Initial state:\n");
    printf("  HFCLK running: %s\n", Clock_HFCLK_IsRunning() ? "Yes" : "No");
    printf("  LFCLK running: %s\n", Clock_LFCLK_IsRunning() ? "Yes" : "No");
    
    /* Start both clocks */
    Clock_HFCLK_Start();
    Clock_LFCLK_Start(CLOCK_LFCLK_SRC_XTAL);
    
    /* Monitor states while starting */
    printf("\nClock states during startup:\n");
    
    Clock_HFCLK_State_t hfState;
    Clock_LFCLK_State_t lfState;
    
    uint32_t timeout = 100000;
    while ((!Clock_HFCLK_IsRunning() || !Clock_LFCLK_IsRunning()) && (timeout-- > 0))
    {
        hfState = Clock_HFCLK_GetState();
        lfState = Clock_LFCLK_GetState();
        
        printf("  HFCLK: %d, LFCLK: %d\r", hfState, lfState);
        Example_Delay(100);
    }
    printf("\n");
    
    /* Print final clock information */
    printf("\nFinal clock status:\n");
    printf("  HFCLK running: %s\n", Clock_HFCLK_IsRunning() ? "Yes" : "No");
    if (Clock_HFCLK_IsRunning())
    {
        printf("    HFCLK Source: %s\n", 
               (Clock_HFCLK_GetSource() == CLOCK_HFCLK_SRC_RC) ? "Internal RC" : "External Crystal");
    }
    
    printf("  LFCLK running: %s\n", Clock_LFCLK_IsRunning() ? "Yes" : "No");
    if (Clock_LFCLK_IsRunning())
    {
        printf("    LFCLK Source: %s\n", 
               (Clock_LFCLK_GetSource() == CLOCK_LFCLK_SRC_RC) ? "Internal RC" :
               (Clock_LFCLK_GetSource() == CLOCK_LFCLK_SRC_XTAL) ? "External Crystal" : "Synthesized");
    }
    
    /* Stop clocks */
    Clock_HFCLK_Stop();
    Clock_LFCLK_Stop();
    
    printf("\nClock status monitoring example complete!\n\n");
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
    Example_Clock_WithCallbacks();
    Example_Clock_Configuration();
    Example_Clock_Calibration();
    Example_Clock_Interrupts();
    Example_Clock_StatusMonitoring();
    
    printf("All examples completed!\n");
    
    return 0;
}

/*============================================================================
 * END OF FILE
 *============================================================================*/
