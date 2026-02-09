#include "gpio.h"

// Example configurations
void Configure_LED_Pin(void) {
    GPIO_Config led_config = {
        .mode = GPIO_MODE_OUTPUT,
        .output_type = GPIO_OUTPUT_PUSH_PULL,
        .speed = GPIO_SPEED_LOW,
        .pull = GPIO_NO_PULL,
        .alternate = GPIO_AF0
    };
    
    // Configure PB0 as LED output
    GPIO_Init(GPIOB, GPIO_PIN_0, &led_config);
}

void Configure_Button_Pin(void) {
    GPIO_Config button_config = {
        .mode = GPIO_MODE_INPUT,
        .output_type = GPIO_OUTPUT_PUSH_PULL, // Not used in input mode
        .speed = GPIO_SPEED_LOW,
        .pull = GPIO_PULL_UP,
        .alternate = GPIO_AF0
    };
    
    // Configure PA0 as button input with pull-up
    GPIO_Init(GPIOA, GPIO_PIN_0, &button_config);
}

void Configure_USART_TX_Pin(void) {
    GPIO_Config usart_config = {
        .mode = GPIO_MODE_ALTERNATE,
        .output_type = GPIO_OUTPUT_PUSH_PULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_NO_PULL,
        .alternate = GPIO_AF1  // USART2_TX on PA2
    };
    
    // Configure PA2 as USART TX
    GPIO_Init(GPIOA, GPIO_PIN_2, &usart_config);
}

void Configure_ADC_Pin(void) {
    GPIO_Config adc_config = {
        .mode = GPIO_MODE_ANALOG,
        .output_type = GPIO_OUTPUT_PUSH_PULL, // Not used in analog mode
        .speed = GPIO_SPEED_LOW,
        .pull = GPIO_NO_PULL,
        .alternate = GPIO_AF0
    };
    
    // Configure PA1 as ADC input
    GPIO_Init(GPIOA, GPIO_PIN_1, &adc_config);
}

int main(void) {
    // Configure pins
    Configure_LED_Pin();
    Configure_Button_Pin();
    Configure_USART_TX_Pin();
    Configure_ADC_Pin();
    
    // LED blink example
    while (1) {
        // Turn LED on
        GPIO_WritePin(GPIOB, GPIO_PIN_0, true);
        
        // Simple delay
        for (volatile uint32_t i = 0; i < 500000; i++);
        
        // Turn LED off
        GPIO_WritePin(GPIOB, GPIO_PIN_0, false);
        
        // Simple delay
        for (volatile uint32_t i = 0; i < 500000; i++);
        
        // Alternative methods using macros
        // GPIO_SET_PIN(GPIOB, GPIO_PIN_0);     // Set pin
        // GPIO_RESET_PIN(GPIOB, GPIO_PIN_0);   // Reset pin
        // GPIO_TOGGLE_PIN(GPIOB, GPIO_PIN_0);  // Toggle pin
        
        // Read button state
        bool button_pressed = !GPIO_ReadPin(GPIOA, GPIO_PIN_0); // Active low with pull-up
        // Alternative: bool button_pressed = GPIO_READ_PIN(GPIOA, GPIO_PIN_0);
        
        if (button_pressed) {
            // Button is pressed
            GPIO_SetPin(GPIOB, GPIO_PIN_0);  // Turn LED on
        }
    }
    
    return 0;
}