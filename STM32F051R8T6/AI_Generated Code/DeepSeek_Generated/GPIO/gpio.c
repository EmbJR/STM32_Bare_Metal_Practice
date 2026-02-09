#include "gpio.h"

// Enable GPIO clock
void GPIO_EnableClock(GPIO_TypeDef *GPIOx) {
    uint32_t reg_value = *(volatile uint32_t *)0x40021020UL; // RCC_AHBENR

    if (GPIOx == GPIOA) {
        reg_value |= (1U << 17); // GPIOAEN
    } else if (GPIOx == GPIOB) {
        reg_value |= (1U << 18); // GPIOBEN
    } else if (GPIOx == GPIOC) {
        reg_value |= (1U << 19); // GPIOCEN
    } else if (GPIOx == GPIOD) {
        reg_value |= (1U << 20); // GPIODEN
    } else if (GPIOx == GPIOE) {
        reg_value |= (1U << 21); // GPIOEEN
    } else if (GPIOx == GPIOF) {
        reg_value |= (1U << 22); // GPIOFEN
    }

    *(volatile uint32_t *)0x40021014UL = reg_value;
}

// Disable GPIO clock
void GPIO_DisableClock(GPIO_TypeDef *GPIOx) {
    uint32_t reg_value = *(volatile uint32_t *)0x40021020UL; // RCC_AHBENR

    if (GPIOx == GPIOA) {
        reg_value &= ~(1U << 17); // GPIOAEN
    } else if (GPIOx == GPIOB) {
        reg_value &= ~(1U << 18); // GPIOBEN
    } else if (GPIOx == GPIOC) {
        reg_value &= ~(1U << 19); // GPIOCEN
    } else if (GPIOx == GPIOD) {
        reg_value &= ~(1U << 20); // GPIODEN
    } else if (GPIOx == GPIOE) {
        reg_value &= ~(1U << 21); // GPIOEEN
    } else if (GPIOx == GPIOF) {
        reg_value &= ~(1U << 22); // GPIOFEN
    }

    *(volatile uint32_t *)0x40021014UL = reg_value;
}

// Initialize GPIO pin
void GPIO_Init(GPIO_TypeDef *GPIOx, uint32_t pin, const GPIO_Config *config) {
    // Enable GPIO clock
    GPIO_EnableClock(GPIOx);

    // Clear and set mode bits (2 bits per pin)
    GPIOx->MODER &= ~(3U << (2 * pin));
    GPIOx->MODER |= (config->mode << (2 * pin));

    // Set output type (1 bit per pin)
    if (config->mode == GPIO_MODE_OUTPUT || config->mode == GPIO_MODE_ALTERNATE) {
        GPIOx->OTYPER &= ~(1U << pin);
        GPIOx->OTYPER |= (config->output_type << pin);
    }

    // Set output speed (2 bits per pin)
    if (config->mode == GPIO_MODE_OUTPUT || config->mode == GPIO_MODE_ALTERNATE) {
        GPIOx->OSPEEDR &= ~(3U << (2 * pin));
        GPIOx->OSPEEDR |= (config->speed << (2 * pin));
    }

    // Set pull-up/pull-down (2 bits per pin)
    GPIOx->PUPDR &= ~(3U << (2 * pin));
    GPIOx->PUPDR |= (config->pull << (2 * pin));

    // Set alternate function if needed
    if (config->mode == GPIO_MODE_ALTERNATE) {
        GPIO_SetAlternateFunction(GPIOx, pin, config->alternate);
    }
}

// Set alternate function for pin
void GPIO_SetAlternateFunction(GPIO_TypeDef *GPIOx, uint32_t pin, GPIO_AlternateFunction af) {
    if (pin < 8) {
        // Pins 0-7 use AFRL
        GPIOx->AFRL &= ~(0xFU << (4 * pin));
        GPIOx->AFRL |= (af << (4 * pin));
    } else {
        // Pins 8-15 use AFRH
        pin -= 8;
        GPIOx->AFRH &= ~(0xFU << (4 * pin));
        GPIOx->AFRH |= (af << (4 * pin));
    }
}

// Write pin state
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint32_t pin, bool state) {
    if (state) {
        GPIO_SetPin(GPIOx, pin);
    } else {
        GPIO_ResetPin(GPIOx, pin);
    }
}

// Toggle pin state
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t pin) {
    GPIOx->ODR ^= (1U << pin);
}

// Read pin state
bool GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint32_t pin) {
    return (GPIOx->IDR & (1U << pin)) ? true : false;
}

// Set pin (atomic operation)
void GPIO_SetPin(GPIO_TypeDef *GPIOx, uint32_t pin) {
    GPIOx->BSRR = (1U << pin);
}

// Reset pin (atomic operation)
void GPIO_ResetPin(GPIO_TypeDef *GPIOx, uint32_t pin) {
    GPIOx->BRR = (1U << pin);
}

// Lock pin configuration
void GPIO_LockPin(GPIO_TypeDef *GPIOx, uint32_t pin) {
    uint32_t temp = GPIOx->LCKR;

    // Write 1 to LCKK bit and 1 to desired pin
    temp = (1U << 16) | (1U << pin);

    // Write sequence to lock the pin
    GPIOx->LCKR = temp;           // Write 1
    GPIOx->LCKR = (1U << pin);    // Write 0
    GPIOx->LCKR = temp;           // Write 1
    (void)GPIOx->LCKR;           // Read back

    // Check if lock is active
    if (GPIOx->LCKR & (1U << 16)) {
        // Lock successful
    }
}
