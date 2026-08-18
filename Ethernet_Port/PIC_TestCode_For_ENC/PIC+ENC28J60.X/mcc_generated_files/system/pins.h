/**
 * Generated Pins header File
 * 
 * @file pins.h
 * 
 * @defgroup  pinsdriver Pins Driver
 * 
 * @brief This is generated driver header for pins. 
 *        This header file provides APIs for all pins selected in the GUI.
 *
 * @version Driver Version  3.1.1
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

#ifndef PINS_H
#define PINS_H

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set RC2 aliases
#define ETH_CS_TRIS                 TRISCbits.TRISC2
#define ETH_CS_LAT                  LATCbits.LATC2
#define ETH_CS_PORT                 PORTCbits.RC2
#define ETH_CS_WPU                  WPUCbits.
#define ETH_CS_OD                   ODCONCbits.
#define ETH_CS_ANS                  ANSELCbits.
#define ETH_CS_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define ETH_CS_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define ETH_CS_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define ETH_CS_GetValue()           PORTCbits.RC2
#define ETH_CS_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define ETH_CS_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define ETH_CS_SetPullup()          do { WPUCbits. = 1; } while(0)
#define ETH_CS_ResetPullup()        do { WPUCbits. = 0; } while(0)
#define ETH_CS_SetPushPull()        do { ODCONCbits. = 0; } while(0)
#define ETH_CS_SetOpenDrain()       do { ODCONCbits. = 1; } while(0)
#define ETH_CS_SetAnalogMode()      do { ANSELCbits. = 1; } while(0)
#define ETH_CS_SetDigitalMode()     do { ANSELCbits. = 0; } while(0)

// get/set RC3 aliases
#define IO_RC3_TRIS                 TRISCbits.TRISC3
#define IO_RC3_LAT                  LATCbits.LATC3
#define IO_RC3_PORT                 PORTCbits.RC3
#define IO_RC3_WPU                  WPUCbits.
#define IO_RC3_OD                   ODCONCbits.
#define IO_RC3_ANS                  ANSELCbits.
#define IO_RC3_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define IO_RC3_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define IO_RC3_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define IO_RC3_GetValue()           PORTCbits.RC3
#define IO_RC3_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define IO_RC3_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define IO_RC3_SetPullup()          do { WPUCbits. = 1; } while(0)
#define IO_RC3_ResetPullup()        do { WPUCbits. = 0; } while(0)
#define IO_RC3_SetPushPull()        do { ODCONCbits. = 0; } while(0)
#define IO_RC3_SetOpenDrain()       do { ODCONCbits. = 1; } while(0)
#define IO_RC3_SetAnalogMode()      do { ANSELCbits. = 1; } while(0)
#define IO_RC3_SetDigitalMode()     do { ANSELCbits. = 0; } while(0)

// get/set RC4 aliases
#define IO_RC4_TRIS                 TRISCbits.TRISC4
#define IO_RC4_LAT                  LATCbits.LATC4
#define IO_RC4_PORT                 PORTCbits.RC4
#define IO_RC4_WPU                  WPUCbits.
#define IO_RC4_OD                   ODCONCbits.
#define IO_RC4_ANS                  ANSELCbits.
#define IO_RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define IO_RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define IO_RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define IO_RC4_GetValue()           PORTCbits.RC4
#define IO_RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define IO_RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define IO_RC4_SetPullup()          do { WPUCbits. = 1; } while(0)
#define IO_RC4_ResetPullup()        do { WPUCbits. = 0; } while(0)
#define IO_RC4_SetPushPull()        do { ODCONCbits. = 0; } while(0)
#define IO_RC4_SetOpenDrain()       do { ODCONCbits. = 1; } while(0)
#define IO_RC4_SetAnalogMode()      do { ANSELCbits. = 1; } while(0)
#define IO_RC4_SetDigitalMode()     do { ANSELCbits. = 0; } while(0)

// get/set RC5 aliases
#define IO_RC5_TRIS                 TRISCbits.TRISC5
#define IO_RC5_LAT                  LATCbits.LATC5
#define IO_RC5_PORT                 PORTCbits.RC5
#define IO_RC5_WPU                  WPUCbits.
#define IO_RC5_OD                   ODCONCbits.
#define IO_RC5_ANS                  ANSELCbits.
#define IO_RC5_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define IO_RC5_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define IO_RC5_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define IO_RC5_GetValue()           PORTCbits.RC5
#define IO_RC5_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define IO_RC5_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define IO_RC5_SetPullup()          do { WPUCbits. = 1; } while(0)
#define IO_RC5_ResetPullup()        do { WPUCbits. = 0; } while(0)
#define IO_RC5_SetPushPull()        do { ODCONCbits. = 0; } while(0)
#define IO_RC5_SetOpenDrain()       do { ODCONCbits. = 1; } while(0)
#define IO_RC5_SetAnalogMode()      do { ANSELCbits. = 1; } while(0)
#define IO_RC5_SetDigitalMode()     do { ANSELCbits. = 0; } while(0)

// get/set RC6 aliases
#define IO_RC6_TRIS                 TRISCbits.TRISC6
#define IO_RC6_LAT                  LATCbits.LATC6
#define IO_RC6_PORT                 PORTCbits.RC6
#define IO_RC6_WPU                  WPUCbits.
#define IO_RC6_OD                   ODCONCbits.
#define IO_RC6_ANS                  ANSELCbits.
#define IO_RC6_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define IO_RC6_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define IO_RC6_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define IO_RC6_GetValue()           PORTCbits.RC6
#define IO_RC6_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define IO_RC6_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define IO_RC6_SetPullup()          do { WPUCbits. = 1; } while(0)
#define IO_RC6_ResetPullup()        do { WPUCbits. = 0; } while(0)
#define IO_RC6_SetPushPull()        do { ODCONCbits. = 0; } while(0)
#define IO_RC6_SetOpenDrain()       do { ODCONCbits. = 1; } while(0)
#define IO_RC6_SetAnalogMode()      do { ANSELCbits. = 1; } while(0)
#define IO_RC6_SetDigitalMode()     do { ANSELCbits. = 0; } while(0)

// get/set RC7 aliases
#define IO_RC7_TRIS                 TRISCbits.TRISC7
#define IO_RC7_LAT                  LATCbits.LATC7
#define IO_RC7_PORT                 PORTCbits.RC7
#define IO_RC7_WPU                  WPUCbits.
#define IO_RC7_OD                   ODCONCbits.
#define IO_RC7_ANS                  ANSELCbits.
#define IO_RC7_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define IO_RC7_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define IO_RC7_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define IO_RC7_GetValue()           PORTCbits.RC7
#define IO_RC7_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define IO_RC7_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define IO_RC7_SetPullup()          do { WPUCbits. = 1; } while(0)
#define IO_RC7_ResetPullup()        do { WPUCbits. = 0; } while(0)
#define IO_RC7_SetPushPull()        do { ODCONCbits. = 0; } while(0)
#define IO_RC7_SetOpenDrain()       do { ODCONCbits. = 1; } while(0)
#define IO_RC7_SetAnalogMode()      do { ANSELCbits. = 1; } while(0)
#define IO_RC7_SetDigitalMode()     do { ANSELCbits. = 0; } while(0)

// get/set RD0 aliases
#define Link_TRIS                 TRISDbits.TRISD0
#define Link_LAT                  LATDbits.LATD0
#define Link_PORT                 PORTDbits.RD0
#define Link_WPU                  WPUDbits.
#define Link_OD                   ODCONDbits.
#define Link_ANS                  ANSELDbits.
#define Link_SetHigh()            do { LATDbits.LATD0 = 1; } while(0)
#define Link_SetLow()             do { LATDbits.LATD0 = 0; } while(0)
#define Link_Toggle()             do { LATDbits.LATD0 = ~LATDbits.LATD0; } while(0)
#define Link_GetValue()           PORTDbits.RD0
#define Link_SetDigitalInput()    do { TRISDbits.TRISD0 = 1; } while(0)
#define Link_SetDigitalOutput()   do { TRISDbits.TRISD0 = 0; } while(0)
#define Link_SetPullup()          do { WPUDbits. = 1; } while(0)
#define Link_ResetPullup()        do { WPUDbits. = 0; } while(0)
#define Link_SetPushPull()        do { ODCONDbits. = 0; } while(0)
#define Link_SetOpenDrain()       do { ODCONDbits. = 1; } while(0)
#define Link_SetAnalogMode()      do { ANSELDbits. = 1; } while(0)
#define Link_SetDigitalMode()     do { ANSELDbits. = 0; } while(0)

// get/set RD1 aliases
#define LED1_TRIS                 TRISDbits.TRISD1
#define LED1_LAT                  LATDbits.LATD1
#define LED1_PORT                 PORTDbits.RD1
#define LED1_WPU                  WPUDbits.
#define LED1_OD                   ODCONDbits.
#define LED1_ANS                  ANSELDbits.
#define LED1_SetHigh()            do { LATDbits.LATD1 = 1; } while(0)
#define LED1_SetLow()             do { LATDbits.LATD1 = 0; } while(0)
#define LED1_Toggle()             do { LATDbits.LATD1 = ~LATDbits.LATD1; } while(0)
#define LED1_GetValue()           PORTDbits.RD1
#define LED1_SetDigitalInput()    do { TRISDbits.TRISD1 = 1; } while(0)
#define LED1_SetDigitalOutput()   do { TRISDbits.TRISD1 = 0; } while(0)
#define LED1_SetPullup()          do { WPUDbits. = 1; } while(0)
#define LED1_ResetPullup()        do { WPUDbits. = 0; } while(0)
#define LED1_SetPushPull()        do { ODCONDbits. = 0; } while(0)
#define LED1_SetOpenDrain()       do { ODCONDbits. = 1; } while(0)
#define LED1_SetAnalogMode()      do { ANSELDbits. = 1; } while(0)
#define LED1_SetDigitalMode()     do { ANSELDbits. = 0; } while(0)

// get/set RD2 aliases
#define LED2_TRIS                 TRISDbits.TRISD2
#define LED2_LAT                  LATDbits.LATD2
#define LED2_PORT                 PORTDbits.RD2
#define LED2_WPU                  WPUDbits.
#define LED2_OD                   ODCONDbits.
#define LED2_ANS                  ANSELDbits.
#define LED2_SetHigh()            do { LATDbits.LATD2 = 1; } while(0)
#define LED2_SetLow()             do { LATDbits.LATD2 = 0; } while(0)
#define LED2_Toggle()             do { LATDbits.LATD2 = ~LATDbits.LATD2; } while(0)
#define LED2_GetValue()           PORTDbits.RD2
#define LED2_SetDigitalInput()    do { TRISDbits.TRISD2 = 1; } while(0)
#define LED2_SetDigitalOutput()   do { TRISDbits.TRISD2 = 0; } while(0)
#define LED2_SetPullup()          do { WPUDbits. = 1; } while(0)
#define LED2_ResetPullup()        do { WPUDbits. = 0; } while(0)
#define LED2_SetPushPull()        do { ODCONDbits. = 0; } while(0)
#define LED2_SetOpenDrain()       do { ODCONDbits. = 1; } while(0)
#define LED2_SetAnalogMode()      do { ANSELDbits. = 1; } while(0)
#define LED2_SetDigitalMode()     do { ANSELDbits. = 0; } while(0)

/**
 * @ingroup  pinsdriver
 * @brief GPIO and peripheral I/O initialization
 * @param none
 * @return none
 */
void PIN_MANAGER_Initialize (void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt on Change Handling routine
 * @param none
 * @return none
 */
void PIN_MANAGER_IOC(void);


#endif // PINS_H
/**
 End of File
*/