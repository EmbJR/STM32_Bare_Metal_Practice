/**
 * TMR0 Generated Driver File
 * 
 * @file tmr0.c
 * 
 * @ingroup tmr08bit
 * 
 * @ingroup tmr0
 * 
 * @brief  Driver implementation for the TMR0 driver
 *
 * @version TMR0 Driver Version 2.0.0
 *
 * @version Package Version 2.0.0
*/
/*
© [2026] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include <xc.h>
#include "../tmr0.h"
#include "../../system/pins.h"

static volatile uint8_t tmr0PeriodCount;
static void (*TMR0_OverflowCallback)(void);
static void TMR0_DefaultOverflowCallback(void);

/**
  Section: TMR0 APIs
*/ 

void TMR0_Initialize(void)
{
    TMR0L = 0xC2;        // Period 992us; Frequency 62500Hz; Count 194
    tmr0PeriodCount = 194U;
    TMR0_OverflowCallbackRegister(TMR0_DefaultOverflowCallback);
    
    INTCONbits.TMR0IF = 0;	   
    INTCONbits.TMR0IE = 1;	

    T0CON = (7 << _T0CON_T0PS_POSN)   // T0PS 1:256
        | (0 << _T0CON_PSA_POSN)   // PSA assigned
        | (1 << _T0CON_T0SE_POSN)   // T0SE Increment_hi_lo
        | (0 << _T0CON_T0CS_POSN)   // T0CS FOSC/4
        | (1 << _T0CON_T08BIT_POSN)   // T08BIT 8-bit
        | (1 << _T0CON_TMR0ON_POSN);  // TMR0ON enabled
}

void TMR0_Deinitialize(void)
{
    T0CONbits.TMR0ON = 0;    
    INTCONbits.TMR0IF = 0;	   
    INTCONbits.TMR0IE = 0;   
    T0CON = 0xFF;
    TMR0H = 0x0;
    TMR0L =0x0;
}

void TMR0_Start(void)
{
    T0CONbits.TMR0ON = 1;
}

void TMR0_Stop(void)
{
    T0CONbits.TMR0ON = 0;
}


uint8_t TMR0_CounterGet(void)
{
    uint8_t counterValue;

    counterValue = TMR0L;

    return counterValue;
}

void TMR0_CounterSet(uint8_t counterValue)
{
    TMR0L = counterValue;
}

void TMR0_PeriodSet(uint8_t periodValue)
{
   tmr0PeriodCount = periodValue;
   TMR0L = periodValue;
}

uint8_t TMR0_PeriodGet(void)
{
    return tmr0PeriodCount;
}

uint8_t TMR0_MaxCountGet(void)
{
    return TMR0_MAX_COUNT;
}

void TMR0_TMRInterruptEnable(void)
{
    INTCONbits.TMR0IE = 1;
}

void TMR0_TMRInterruptDisable(void)
{
    INTCONbits.TMR0IE = 0;
}

void TMR0_OverflowISR(void)
{
    static volatile uint16_t countCallBack = 0;
    TMR0L = tmr0PeriodCount;
    if (++countCallBack >= TMR0_INTERRUPT_TICKER_FACTOR)
    {
        if(NULL != TMR0_OverflowCallback)
        {
            TMR0_OverflowCallback();
        }
        countCallBack = 0;
    }
    INTCONbits.TMR0IF = 0;
}

void TMR0_OverflowCallbackRegister(void (* callbackHandler)(void))
{
    TMR0_OverflowCallback = callbackHandler;
}

static void TMR0_DefaultOverflowCallback(void)
{
    // Default interrupt handler
    LED1_Toggle();
}

