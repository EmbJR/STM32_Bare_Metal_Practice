
#include "F051NVIC.h"


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


//--------------------------- EXT Interrupt ------------------------------


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