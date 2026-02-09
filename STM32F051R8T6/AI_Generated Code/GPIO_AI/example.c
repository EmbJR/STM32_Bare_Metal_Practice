/**
 * @file    example.c
 * @brief   GPIO Driver Example Code for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-02-09
 */

#include "gpio.h"

/*============================================================================
 * System Core Clock (assumes HSI 8MHz, no PLL for simplicity)
 *============================================================================*/
#define HSI_FREQUENCY    8000000UL

/*============================================================================
 * Simple Delay Function (using active wait)
 *============================================================================*/
void Delay_ms(uint32_t ms) {
    uint32_t i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 8000; j++) {
            __NOP();
        }
    }
}

/*============================================================================
 * Example 1: LED Blink (GPIO Output)
 *============================================================================*/
void Example_LED_Blink(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Enable GPIOC clock for LED on PC8 */
    GPIO_EnableClock(GPIOC);
    
    /* Configure PC8 as output */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* Main loop - toggle LED */
    while (1) {
        GPIO_TogglePin(GPIOC, GPIO_PIN_8);
        Delay_ms(500);
    }
}

/*============================================================================
 * Example 2: Button Read (GPIO Input)
 *============================================================================*/
volatile uint8_t Button_Pressed = 0;

void Example_Button_Read(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Enable GPIOA clock for button on PA0 */
    GPIO_EnableClock(GPIOA);
    
    /* Configure PA0 as input with pull-up */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Pull = GPIO_PULL_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* Enable GPIOC for LED */
    GPIO_EnableClock(GPIOC);
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* Main loop - check button and control LED */
    while (1) {
        if (GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0) {
            /* Button pressed (active low) */
            GPIO_SetPin(GPIOC, GPIO_PIN_8);  /* LED on */
        } else {
            /* Button released */
            GPIO_ResetPin(GPIOC, GPIO_PIN_8); /* LED off */
        }
    }
}

/*============================================================================
 * Example 3: External Interrupt (EXTI)
 *============================================================================*/
volatile uint8_t Interrupt_Count = 0;

void Example_EXTI_Interrupt(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Enable GPIOC for LED */
    GPIO_EnableClock(GPIOC);
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* Enable GPIOA for button */
    GPIO_EnableClock(GPIOA);
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Pull = GPIO_PULL_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* Configure EXTI Line 0 for PA0 */
    SYSCFG_CR &= ~(0x0F);  /* Clear SYSCFG_EXTI0 bits */
    SYSCFG_CR |= (0x00);   /* Select GPIOA for EXTI0 */
    EXTI_IMR |= EXTI_LINE_0;       /* Enable EXTI Line 0 */
    EXTI_RTSR |= EXTI_LINE_0;     /* Enable rising edge trigger */
    EXTI_FTSR &= ~EXTI_LINE_0;    /* Disable falling edge trigger */
    EXTI_PR = EXTI_LINE_0;        /* Clear pending bit */
    
    /* Configure NVIC for EXTI0_1 IRQ */
    NVIC_EnableIRQ(EXTI0_1_IRQn);
    NVIC_SetPriority(EXTI0_1_IRQn, 3);  /* Lowest priority */
    
    /* Main loop */
    while (1) {
        /* LED reflects interrupt count (mod 2) */
        if (Interrupt_Count % 2) {
            GPIO_SetPin(GPIOC, GPIO_PIN_8);
        } else {
            GPIO_ResetPin(GPIOC, GPIO_PIN_8);
        }
    }
}

/*============================================================================
 * Example 4: Multiple Pins Configuration
 *============================================================================*/
void Example_Multiple_Pins(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Enable GPIOC clock */
    GPIO_EnableClock(GPIOC);
    
    /* Configure PC8, PC9 as outputs */
    GPIO_InitStruct.Pin = (GPIO_PIN_8 | GPIO_PIN_9);
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* Configure PC6, PC7 as inputs */
    GPIO_InitStruct.Pin = (GPIO_PIN_6 | GPIO_PIN_7);
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Pull = GPIO_PULL_DOWN;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* Main loop - read inputs and control outputs */
    while (1) {
        if (GPIO_ReadPin(GPIOC, GPIO_PIN_6)) {
            GPIO_SetPin(GPIOC, GPIO_PIN_8);
        } else {
            GPIO_ResetPin(GPIOC, GPIO_PIN_8);
        }
        
        if (GPIO_ReadPin(GPIOC, GPIO_PIN_7)) {
            GPIO_SetPin(GPIOC, GPIO_PIN_9);
        } else {
            GPIO_ResetPin(GPIOC, GPIO_PIN_9);
        }
    }
}

/*============================================================================
 * Example 5: Alternate Function (UART TX on PA9)
 *============================================================================*/
void Example_Alternate_Function(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Enable GPIOA clock */
    GPIO_EnableClock(GPIOA);
    
    /* Configure PA9 as USART1_TX (AF7) */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_InitStruct.AF = GPIO_AF7;  /* USART1_TX */
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* Configure PA10 as USART1_RX (AF7) */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULL_UP;
    GPIO_InitStruct.AF = GPIO_AF7;  /* USART1_RX */
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* Main loop */
    while (1) {
        /* UART is now configured - can be used with USART driver */
    }
}

/*============================================================================
 * Example 6: Open-Drain Output (I2C-like)
 *============================================================================*/
void Example_Open_Drain(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Enable GPIOB clock for I2C pins (PB6=SCL, PB7=SDA) */
    GPIO_EnableClock(GPIOB);
    
    /* Configure PB6 as open-drain output */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Ot = GPIO_OTYPE_OD;  /* Open-drain */
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* Configure PB7 as open-drain output */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* Main loop */
    while (1) {
        /* Open-drain outputs - need external pull-up resistors */
        /* Drive low by setting pin, release high by resetting pin */
    }
}

/*============================================================================
 * Example 7: Port Read/Write Operations
 *============================================================================*/
void Example_Port_Operations(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Enable GPIOC for LED port */
    GPIO_EnableClock(GPIOC);
    
    /* Configure entire port PC as output */
    GPIO_InitStruct.Pin = GPIO_PIN_ALL;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Ot = GPIO_OTYPE_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Pull = GPIO_PULL_NO;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* Main loop - write patterns to port */
    while (1) {
        /* Write 0xFF to turn all LEDs on (active high) */
        GPIO_WritePort(GPIOC, 0xFF);
        Delay_ms(500);
        
        /* Write 0x00 to turn all LEDs off */
        GPIO_WritePort(GPIOC, 0x00);
        Delay_ms(500);
        
        /* Knight rider pattern */
        GPIO_WritePort(GPIOC, 0x01);
        Delay_ms(100);
        GPIO_WritePort(GPIOC, 0x02);
        Delay_ms(100);
        GPIO_WritePort(GPIOC, 0x04);
        Delay_ms(100);
        GPIO_WritePort(GPIOC, 0x08);
        Delay_ms(100);
        GPIO_WritePort(GPIOC, 0x10);
        Delay_ms(100);
        GPIO_WritePort(GPIOC, 0x20);
        Delay_ms(100);
        GPIO_WritePort(GPIOC, 0x40);
        Delay_ms(100);
        GPIO_WritePort(GPIOC, 0x80);
        Delay_ms(100);
    }
}

/*============================================================================
 * EXTI0_1 Interrupt Handler
 *============================================================================*/
void EXTI0_1_IRQHandler(void) {
    if (EXTI_GetFlag(EXTI_LINE_0)) {
        /* Clear interrupt flag */
        EXTI_ClearFlag(EXTI_LINE_0);
        
        /* Increment counter */
        Interrupt_Count++;
    }
}

/*============================================================================
 * EXTI2_3 Interrupt Handler
 *============================================================================*/
void EXTI2_3_IRQHandler(void) {
    if (EXTI_GetFlag(EXTI_LINE_2)) {
        EXTI_ClearFlag(EXTI_LINE_2);
        /* Handle EXTI2 interrupt */
    }
    
    if (EXTI_GetFlag(EXTI_LINE_3)) {
        EXTI_ClearFlag(EXTI_LINE_3);
        /* Handle EXTI3 interrupt */
    }
}

/*============================================================================
 * EXTI4_15 Interrupt Handler
 *============================================================================*/
void EXTI4_15_IRQHandler(void) {
    uint32_t pending_lines = (EXTI_PR >> 4) & 0xFFF;
    uint32_t i;
    
    for (i = 4; i <= 15; i++) {
        if (pending_lines & (1UL << i)) {
            EXTI_ClearFlag(1UL << i);
            /* Handle EXTIi interrupt */
        }
    }
}

/*============================================================================
 * Main Function - Run Selected Example
 *============================================================================*/
int main(void) {
    /* Select example to run by uncommenting one of the following: */
    
    // Example_LED_Blink();
    // Example_Button_Read();
    // Example_EXTI_Interrupt();
    // Example_Multiple_Pins();
    // Example_Alternate_Function();
    // Example_Open_Drain();
    // Example_Port_Operations();
    
    /* Default: LED blink */
    Example_LED_Blink();
    
    return 0;
}
