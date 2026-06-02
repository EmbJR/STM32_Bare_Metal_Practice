/**
 * @file    gpio.c
 * @brief   GPIO Driver Implementation for STM32F051R8T6
 * @author  AI Generated
 * @date    2026-02-09
 */

#include "gpio.h"

/*============================================================================
 * GPIO Initialization and Configuration
 *============================================================================*/

/**
 * @brief  Deinitialize GPIO registers to their default reset values.
 * @param  GPIOx: pointer to GPIO peripheral (GPIOA, GPIOB, etc.)
 */
void GPIO_DeInit(GPIO_TypeDef *GPIOx) {
    GPIOx->MODER = 0x00000000;
    GPIOx->OTYPER = 0x00000000;
    GPIOx->OSPEEDR = 0x00000000;
    GPIOx->PUPDR = 0x00000000;
    GPIOx->ODR = 0x00000000;
    GPIOx->BSRR = 0x00000000;
    GPIOx->LCKR = 0x00000000;
    GPIOx->AFR[0] = 0x00000000;
    GPIOx->AFR[1] = 0x00000000;
    GPIOx->BRR = 0x00000000;
}

/**
 * @brief  Initialize GPIO peripheral according to specified parameters.
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  GPIO_Init: pointer to GPIO_InitTypeDef structure
 */
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init) {
    uint32_t pinpos = 0;
    uint32_t currentpin = 0;

    for (pinpos = 0; pinpos < 16; pinpos++) {
        currentpin = (GPIO_Init->Pin >> pinpos) & 0x01;
        if (currentpin == 0x01) {
            /* Configure Mode */
            GPIOx->MODER &= ~(0x03 << (pinpos * 2));
            GPIOx->MODER |= (GPIO_Init->Mode << (pinpos * 2));

            /* Configure Output Type */
            if (GPIO_Init->Mode == GPIO_MODE_OUTPUT || GPIO_Init->Mode == GPIO_MODE_ALTERNATE) {
                GPIOx->OTYPER &= ~(0x01 << pinpos);
                GPIOx->OTYPER |= (GPIO_Init->Ot << pinpos);
            }

            /* Configure Speed */
            GPIOx->OSPEEDR &= ~(0x03 << (pinpos * 2));
            GPIOx->OSPEEDR |= (GPIO_Init->Speed << (pinpos * 2));

            /* Configure Pull-up/Pull-down */
            GPIOx->PUPDR &= ~(0x03 << (pinpos * 2));
            GPIOx->PUPDR |= (GPIO_Init->Pull << (pinpos * 2));

            /* Configure Alternate Function */
            if (GPIO_Init->Mode == GPIO_MODE_ALTERNATE) {
                if (pinpos < 8) {
                    GPIOx->AFR[0] &= ~(0x0F << (pinpos * 4));
                    GPIOx->AFR[0] |= (GPIO_Init->AF << (pinpos * 4));
                } else {
                    GPIOx->AFR[1] &= ~(0x0F << ((pinpos - 8) * 4));
                    GPIOx->AFR[1] |= (GPIO_Init->AF << ((pinpos - 8) * 4));
                }
            }
        }
    }
}

/**
 * @brief  Fill GPIO_InitStruct with default values.
 * @param  GPIO_InitStruct: pointer to GPIO_InitTypeDef structure
 */
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_Init) {
    GPIO_Init->Pin = GPIO_PIN_ALL;
    GPIO_Init->Mode = GPIO_MODE_INPUT;
    GPIO_Init->Ot = GPIO_OTYPE_PP;
    GPIO_Init->Speed = GPIO_SPEED_LOW;
    GPIO_Init->Pull = GPIO_PULL_NO;
    GPIO_Init->AF = GPIO_AF0;
}

/*============================================================================
 * Pin Operations
 *============================================================================*/

/**
 * @brief  Set specified GPIO pin(s).
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  Pin: pin(s) to set (use GPIO_PIN_x defines)
 */
void GPIO_SetPin(GPIO_TypeDef *GPIOx, uint32_t Pin) {
    GPIOx->BSRR = Pin;
}

/**
 * @brief  Reset specified GPIO pin(s).
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  Pin: pin(s) to reset (use GPIO_PIN_x defines)
 */
void GPIO_ResetPin(GPIO_TypeDef *GPIOx, uint32_t Pin) {
    GPIOx->BRR = Pin;
}

/**
 * @brief  Toggle specified GPIO pin(s).
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  Pin: pin(s) to toggle (use GPIO_PIN_x defines)
 */
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t Pin) {
    GPIOx->ODR ^= Pin;
}

/**
 * @brief  Read specified GPIO pin.
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  Pin: pin to read (use GPIO_PIN_x defines)
 * @return: Pin state (0 or 1)
 */
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint32_t Pin) {
    return (GPIOx->IDR & Pin) ? 1 : 0;
}

/**
 * @brief  Read entire GPIO port.
 * @param  GPIOx: pointer to GPIO peripheral
 * @return: Port value (16-bit)
 */
uint16_t GPIO_ReadPort(GPIO_TypeDef *GPIOx) {
    return (uint16_t)(GPIOx->IDR);
}

/**
 * @brief  Write value to entire GPIO port.
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  PortVal: value to write (16-bit)
 */
void GPIO_WritePort(GPIO_TypeDef *GPIOx, uint16_t PortVal) {
    GPIOx->ODR = PortVal;
}

/*============================================================================
 * Clock Control
 *============================================================================*/

/**
 * @brief  Enable GPIO clock.
 * @param  GPIOx: pointer to GPIO peripheral
 */
void GPIO_EnableClock(GPIO_TypeDef *GPIOx) {
    if (GPIOx == GPIOA) {
        RCC_AHBENR |= RCC_AHBENR_IOPAEN;
    } else if (GPIOx == GPIOB) {
        RCC_AHBENR |= RCC_AHBENR_IOPBEN;
    } else if (GPIOx == GPIOC) {
        RCC_AHBENR |= RCC_AHBENR_IOPCEN;
    } else if (GPIOx == GPIOD) {
        RCC_AHBENR |= RCC_AHBENR_IOPDEN;
    } else if (GPIOx == GPIOE) {
        RCC_AHBENR |= RCC_AHBENR_IOPEEN;
    } else if (GPIOx == GPIOF) {
        RCC_AHBENR |= RCC_AHBENR_IOPFEN;
    }
}

/**
 * @brief  Disable GPIO clock.
 * @param  GPIOx: pointer to GPIO peripheral
 */
void GPIO_DisableClock(GPIO_TypeDef *GPIOx) {
    if (GPIOx == GPIOA) {
        RCC_AHBENR &= ~RCC_AHBENR_IOPAEN;
    } else if (GPIOx == GPIOB) {
        RCC_AHBENR &= ~RCC_AHBENR_IOPBEN;
    } else if (GPIOx == GPIOC) {
        RCC_AHBENR &= ~RCC_AHBENR_IOPCEN;
    } else if (GPIOx == GPIOD) {
        RCC_AHBENR &= ~RCC_AHBENR_IOPDEN;
    } else if (GPIOx == GPIOE) {
        RCC_AHBENR &= ~RCC_AHBENR_IOPEEN;
    } else if (GPIOx == GPIOF) {
        RCC_AHBENR &= ~RCC_AHBENR_IOPFEN;
    }
}

/*============================================================================
 * Locking
 *============================================================================*/

/**
 * @brief  Lock GPIO pin configuration.
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  Pin: pin to lock (use GPIO_PIN_x defines)
 */
void GPIO_LockPin(GPIO_TypeDef *GPIOx, uint32_t Pin) {
    GPIOx->LCKR = (GPIO_LCKR_LCKK | Pin);
    GPIOx->LCKR = Pin;
    GPIOx->LCKR = (GPIO_LCKR_LCKK | Pin);
    (void)GPIOx->LCKR;  /* Read to complete lock sequence */
}

/**
 * @brief  Check if specified pin is locked.
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  Pin: pin to check (use GPIO_PIN_x defines)
 * @return: Lock status (0 = not locked, 1 = locked)
 */
uint8_t GPIO_IsPinLocked(GPIO_TypeDef *GPIOx, uint32_t Pin) {
    return (GPIOx->LCKR & Pin) ? 1 : 0;
}

/**
 * @brief  Check if entire port is locked.
 * @param  GPIOx: pointer to GPIO peripheral
 * @return: Lock status (0 = not locked, 1 = locked)
 */
uint8_t GPIO_IsPortLocked(GPIO_TypeDef *GPIOx) {
    return (GPIOx->LCKR & GPIO_LCKR_LCKK) ? 1 : 0;
}

/*============================================================================
 * EXTI Functions
 *============================================================================*/

/**
 * @brief  Deinitialize EXTI registers.
 */
void EXTI_DeInit(void) {
    EXTI_IMR = 0x00000000;
    EXTI_EMR = 0x00000000;
    EXTI_RTSR = 0x00000000;
    EXTI_FTSR = 0x00000000;
    EXTI_SWIER = 0x00000000;
    EXTI_PR = 0x0000FFFF;
}

/**
 * @brief  Configure EXTI line to use specified GPIO pin.
 * @param  EXTI_Line: EXTI line number (EXTI_LINE_x)
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  Pin: GPIO pin number (0-15)
 */
void EXTI_LineConfig(uint32_t EXTI_Line, GPIO_TypeDef *GPIOx, uint16_t Pin) {
    uint32_t gpio_port_index = GPIO_GetPortIndex(GPIOx);
    uint32_t exticr_index = Pin >> 2;      /* 0, 1, 2, or 3 */
    uint32_t exticr_shift = (Pin & 0x03) * 4;  /* 0, 4, 8, or 12 */
    
    /* Clear EXTIx bits */
    if (exticr_index == 0) {
        SYSCFG_EXTICR1 &= ~(0x0F << exticr_shift);
        SYSCFG_EXTICR1 |= (gpio_port_index << exticr_shift);
    } else if (exticr_index == 1) {
        SYSCFG_EXTICR2 &= ~(0x0F << exticr_shift);
        SYSCFG_EXTICR2 |= (gpio_port_index << exticr_shift);
    } else if (exticr_index == 2) {
        SYSCFG_EXTICR3 &= ~(0x0F << exticr_shift);
        SYSCFG_EXTICR3 |= (gpio_port_index << exticr_shift);
    } else {
        SYSCFG_EXTICR4 &= ~(0x0F << exticr_shift);
        SYSCFG_EXTICR4 |= (gpio_port_index << exticr_shift);
    }
}

/**
 * @brief  Enable or disable EXTI line.
 * @param  EXTI_Line: EXTI line to enable/disable
 * @param  Enable: 1 = enable, 0 = disable
 */
void EXTI_EnableExtiLine(uint32_t EXTI_Line, uint8_t Enable) {
    if (Enable) {
        EXTI_IMR |= EXTI_Line;
    } else {
        EXTI_IMR &= ~EXTI_Line;
    }
}

/**
 * @brief  Configure EXTI line trigger type.
 * @param  EXTI_Line: EXTI line to configure
 * @param  Trigger: trigger type (EXTI_TRIGGER_RISING, FALLING, or BOTH)
 */
void EXTI_SetTrigger(uint32_t EXTI_Line, EXTI_TriggerTypeDef Trigger) {
    EXTI_RTSR &= ~EXTI_Line;
    EXTI_FTSR &= ~EXTI_Line;
    
    if (Trigger & EXTI_TRIGGER_RISING) {
        EXTI_RTSR |= EXTI_Line;
    }
    if (Trigger & EXTI_TRIGGER_FALLING) {
        EXTI_FTSR |= EXTI_Line;
    }
}

/**
 * @brief  Clear EXTI line flag.
 * @param  EXTI_Line: EXTI line to clear
 */
void EXTI_ClearFlag(uint32_t EXTI_Line) {
    EXTI_PR = EXTI_Line;
}

/**
 * @brief  Get EXTI line flag status.
 * @param  EXTI_Line: EXTI line to check
 * @return: Flag status (0 or 1)
 */
uint32_t EXTI_GetFlag(uint32_t EXTI_Line) {
    return (EXTI_PR & EXTI_Line) ? 1 : 0;
}

/**
 * @brief  Get EXTI pending bit.
 * @param  EXTI_Line: EXTI line to check
 * @return: Pending status (0 or 1)
 */
uint32_t EXTI_GetPending(uint32_t EXTI_Line) {
    return (EXTI_PR & EXTI_Line) ? 1 : 0;
}

/**
 * @brief  Clear EXTI pending bit.
 * @param  EXTI_Line: EXTI line to clear
 */
void EXTI_ClearPending(uint32_t EXTI_Line) {
    EXTI_PR = EXTI_Line;
}

/*============================================================================
 * NVIC Functions
 *============================================================================*/

/**
 * @brief  Enable interrupt request.
 * @param  IRQn: interrupt number
 */
void NVIC_EnableIRQ(IRQn_Type IRQn) {
    NVIC_ISER |= (1UL << IRQn);
}

/**
 * @brief  Disable interrupt request.
 * @param  IRQn: interrupt number
 */
void NVIC_DisableIRQ(IRQn_Type IRQn) {
    NVIC_ICER |= (1UL << IRQn);
}

/**
 * @brief  Set interrupt priority.
 * @param  IRQn: interrupt number
 * @param  Priority: priority value (0-3 for STM32F0xx)
 */
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t Priority) {
    uint8_t reg_index = IRQn >> 2;
    uint8_t shift = (IRQn & 0x03) * 8;
    NVIC_IPR[reg_index] &= ~(0xFF << shift);
    NVIC_IPR[reg_index] |= (Priority << (shift + 6));  /* Bits 6-7 for priority */
}

/**
 * @brief  Clear pending interrupt request.
 * @param  IRQn: interrupt number
 */
void NVIC_ClearPendingIRQ(IRQn_Type IRQn) {
    NVIC_ICER |= (1UL << IRQn);
}

/*============================================================================
 * Extended GPIO Functions
 *============================================================================*/

/**
 * @brief  Set GPIO pin speed.
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  Pin: pin to configure
 * @param  Speed: speed value
 */
void GPIO_SetSpeed(GPIO_TypeDef *GPIOx, uint16_t Pin, GPIO_SpeedTypeDef Speed) {
    uint32_t pinpos = 0;
    uint32_t currentpin = 0;
    
    for (pinpos = 0; pinpos < 16; pinpos++) {
        currentpin = (Pin >> pinpos) & 0x01;
        if (currentpin) {
            GPIOx->OSPEEDR &= ~(0x03 << (pinpos * 2));
            GPIOx->OSPEEDR |= (Speed << (pinpos * 2));
        }
    }
}

/**
 * @brief  Set GPIO pin pull-up/pull-down.
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  Pin: pin to configure
 * @param  Pull: pull configuration
 */
void GPIO_SetPull(GPIO_TypeDef *GPIOx, uint16_t Pin, GPIO_PullTypeDef Pull) {
    uint32_t pinpos = 0;
    uint32_t currentpin = 0;
    
    for (pinpos = 0; pinpos < 16; pinpos++) {
        currentpin = (Pin >> pinpos) & 0x01;
        if (currentpin) {
            GPIOx->PUPDR &= ~(0x03 << (pinpos * 2));
            GPIOx->PUPDR |= (Pull << (pinpos * 2));
        }
    }
}

/**
 * @brief  Set GPIO pin mode.
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  Pin: pin to configure
 * @param  Mode: mode value
 */
void GPIO_SetMode(GPIO_TypeDef *GPIOx, uint16_t Pin, GPIO_ModeTypeDef Mode) {
    uint32_t pinpos = 0;
    uint32_t currentpin = 0;
    
    for (pinpos = 0; pinpos < 16; pinpos++) {
        currentpin = (Pin >> pinpos) & 0x01;
        if (currentpin) {
            GPIOx->MODER &= ~(0x03 << (pinpos * 2));
            GPIOx->MODER |= (Mode << (pinpos * 2));
        }
    }
}

/**
 * @brief  Set GPIO pin output type.
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  Pin: pin to configure
 * @param  Ot: output type
 */
void GPIO_SetOutputType(GPIO_TypeDef *GPIOx, uint16_t Pin, GPIO_OTypeTypeDef Ot) {
    uint32_t pinpos = 0;
    uint32_t currentpin = 0;
    
    for (pinpos = 0; pinpos < 16; pinpos++) {
        currentpin = (Pin >> pinpos) & 0x01;
        if (currentpin) {
            GPIOx->OTYPER &= ~(0x01 << pinpos);
            GPIOx->OTYPER |= (Ot << pinpos);
        }
    }
}

/**
 * @brief  Set GPIO pin alternate function.
 * @param  GPIOx: pointer to GPIO peripheral
 * @param  Pin: pin to configure
 * @param  AF: alternate function
 */
void GPIO_SetAF(GPIO_TypeDef *GPIOx, uint16_t Pin, GPIO_AFTypeDef AF) {
    uint32_t pinpos = 0;
    uint32_t currentpin = 0;
    
    for (pinpos = 0; pinpos < 16; pinpos++) {
        currentpin = (Pin >> pinpos) & 0x01;
        if (currentpin) {
            if (pinpos < 8) {
                GPIOx->AFR[0] &= ~(0x0F << (pinpos * 4));
                GPIOx->AFR[0] |= (AF << (pinpos * 4));
            } else {
                GPIOx->AFR[1] &= ~(0x0F << ((pinpos - 8) * 4));
                GPIOx->AFR[1] |= (AF << ((pinpos - 8) * 4));
            }
        }
    }
}
