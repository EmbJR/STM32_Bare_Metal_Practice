#include "GPIOH743ZI.h"

/* =====================================================================
 *  STM32H743ZIT6 - Bare Metal GPIO Driver implementation
 * ===================================================================== */

/* ---------------------------------------------------------------------
 *  Helper: return the AHB4ENR enable bit position for a given port.
 *  GPIOA = bit0, GPIOB = bit1, ... derived from the 0x400 spacing.
 * ------------------------------------------------------------------- */
static uint32_t GPIO_GetPortIndex(GPIO_TypeDef *port) {
    return ((uint32_t)port - GPIOA_BASE) / 0x400UL;
}

/* ---------------------------------------------------------------------
 *  Clock control
 * ------------------------------------------------------------------- */
void GPIO_EnableClock(GPIO_TypeDef *port) {
    RCC_AHB4ENR_REG |= (1U << GPIO_GetPortIndex(port));
}

void GPIO_DisableClock(GPIO_TypeDef *port) {
    RCC_AHB4ENR_REG &= ~(1U << GPIO_GetPortIndex(port));
}

/* ---------------------------------------------------------------------
 *  Per-attribute configuration (2 bits per pin unless noted)
 * ------------------------------------------------------------------- */
void GPIO_SetMode(GPIO_TypeDef *port, uint8_t pin, GPIO_Mode mode) {
    port->MODER &= ~(0x3U << (pin * 2));
    port->MODER |=  ((uint32_t)mode << (pin * 2));
}

void GPIO_SetOutputType(GPIO_TypeDef *port, uint8_t pin, GPIO_OutputType otype) {
    /* OTYPER uses 1 bit per pin */
    port->OTYPER &= ~(0x1U << pin);
    port->OTYPER |=  ((uint32_t)otype << pin);
}

void GPIO_SetSpeed(GPIO_TypeDef *port, uint8_t pin, GPIO_Speed speed) {
    port->OSPEEDR &= ~(0x3U << (pin * 2));
    port->OSPEEDR |=  ((uint32_t)speed << (pin * 2));
}

void GPIO_SetPull(GPIO_TypeDef *port, uint8_t pin, GPIO_Pull pull) {
    port->PUPDR &= ~(0x3U << (pin * 2));
    port->PUPDR |=  ((uint32_t)pull << (pin * 2));
}

void GPIO_SetAlternate(GPIO_TypeDef *port, uint8_t pin, uint8_t af) {
    /* AFR[0] handles pins 0..7, AFR[1] handles pins 8..15 (4 bits each) */
    uint32_t idx   = (pin >> 3) & 0x1U;          /* 0 for low, 1 for high */
    uint32_t shift = (pin & 0x7U) * 4U;
    port->AFR[idx] &= ~(0xFU << shift);
    port->AFR[idx] |=  ((uint32_t)(af & 0xFU) << shift);
}

/* ---------------------------------------------------------------------
 *  Full pin initialisation from a descriptor
 * ------------------------------------------------------------------- */
void GPIO_Init(GPIO_TypeDef *port, const GPIO_PinConfig *config) {
    GPIO_EnableClock(port);

    GPIO_SetMode(port, config->pin, config->mode);
    GPIO_SetPull(port, config->pin, config->pull);

    if (config->mode == GPIO_MODE_OUTPUT || config->mode == GPIO_MODE_ALTERNATE) {
        GPIO_SetOutputType(port, config->pin, config->otype);
        GPIO_SetSpeed(port, config->pin, config->speed);
    }

    if (config->mode == GPIO_MODE_ALTERNATE) {
        GPIO_SetAlternate(port, config->pin, config->alternate);
    }
}

/* ---------------------------------------------------------------------
 *  Data access
 *  BSRR: lower 16 bits set a pin, upper 16 bits reset a pin (atomic).
 * ------------------------------------------------------------------- */
void GPIO_WritePin(GPIO_TypeDef *port, uint8_t pin, GPIO_PinState state) {
    if (state == GPIO_PIN_HIGH) {
        port->BSRR = (1U << pin);
    } else {
        port->BSRR = (1U << (pin + 16));
    }
}

void GPIO_SetPin(GPIO_TypeDef *port, uint8_t pin) {
    port->BSRR = (1U << pin);
}

void GPIO_ResetPin(GPIO_TypeDef *port, uint8_t pin) {
    port->BSRR = (1U << (pin + 16));
}

void GPIO_TogglePin(GPIO_TypeDef *port, uint8_t pin) {
    if (port->ODR & (1U << pin)) {
        port->BSRR = (1U << (pin + 16));   /* currently high -> reset */
    } else {
        port->BSRR = (1U << pin);          /* currently low  -> set   */
    }
}

GPIO_PinState GPIO_ReadPin(GPIO_TypeDef *port, uint8_t pin) {
    return (port->IDR & (1U << pin)) ? GPIO_PIN_HIGH : GPIO_PIN_LOW;
}

uint16_t GPIO_ReadPort(GPIO_TypeDef *port) {
    return (uint16_t)(port->IDR & 0xFFFFU);
}

void GPIO_WritePort(GPIO_TypeDef *port, uint16_t value) {
    port->ODR = value;
}

/* ---------------------------------------------------------------------
 *  Interrupt (EXTI) handling
 *
 *  Steps to wire a GPIO pin to an EXTI line:
 *   1. Enable SYSCFG clock.
 *   2. Select the source port for the EXTI line in SYSCFG_EXTICR.
 *   3. Choose the trigger edge (rising / falling) in EXTI_RTSR/FTSR.
 *   4. Unmask the line in EXTI_CPUIMR.
 *   5. Enable the matching IRQ in the NVIC.
 *
 *  EXTI line number == pin number (e.g. PG0 -> EXTI0).
 * ------------------------------------------------------------------- */
void GPIO_ConfigInterrupt(GPIO_TypeDef *port, uint8_t pin, GPIO_IntEdge edge) {
    /* 1. Enable SYSCFG clock */
    RCC_APB4ENR_REG |= RCC_APB4ENR_SYSCFGEN;

    /* 2. Map the EXTI line to this port.
     *    Each EXTICR register holds 4 lines, 4 bits per line. */
    uint32_t port_index = GPIO_GetPortIndex(port);
    uint32_t reg        = pin >> 2;          /* which EXTICR[0..3]   */
    uint32_t shift      = (pin & 0x3U) * 4U; /* bit offset in reg    */

    SYSCFG->EXTICR[reg] &= ~(0xFU << shift);
    SYSCFG->EXTICR[reg] |=  ((port_index & 0xFU) << shift);

    /* 3. Configure trigger edge */
    if (edge & GPIO_IT_RISING) {
        EXTI_RTSR1 |= (1U << pin);
    } else {
        EXTI_RTSR1 &= ~(1U << pin);
    }

    if (edge & GPIO_IT_FALLING) {
        EXTI_FTSR1 |= (1U << pin);
    } else {
        EXTI_FTSR1 &= ~(1U << pin);
    }

    /* 4. Unmask the interrupt request for this line */
    EXTI_CPUIMR1 |= (1U << pin);
}

/* Return the NVIC IRQ number that services a given EXTI line/pin. */
static uint8_t GPIO_GetIRQn(uint8_t pin) {
    if (pin <= 4) {
        return EXTI0_IRQn + pin;   /* EXTI0..EXTI4 have dedicated vectors */
    } else if (pin <= 9) {
        return EXTI9_5_IRQn;       /* EXTI5..EXTI9 share one vector       */
    } else {
        return EXTI15_10_IRQn;     /* EXTI10..EXTI15 share one vector     */
    }
}

void GPIO_EnableInterrupt(uint8_t pin) {
    uint8_t irq = GPIO_GetIRQn(pin);
    EXTI_CPUIMR1 |= (1U << pin);
    NVIC_ISER(irq >> 5) = (1U << (irq & 0x1FU));
}

void GPIO_DisableInterrupt(uint8_t pin) {
    uint8_t irq = GPIO_GetIRQn(pin);
    EXTI_CPUIMR1 &= ~(1U << pin);
    NVIC_ICER(irq >> 5) = (1U << (irq & 0x1FU));
}

uint32_t GPIO_GetPendingInterrupt(uint8_t pin) {
    return (EXTI_CPUPR1 & (1U << pin)) ? 1U : 0U;
}

void GPIO_ClearPendingInterrupt(uint8_t pin) {
    /* Pending bit is cleared by writing 1 to it */
    EXTI_CPUPR1 = (1U << pin);
}
