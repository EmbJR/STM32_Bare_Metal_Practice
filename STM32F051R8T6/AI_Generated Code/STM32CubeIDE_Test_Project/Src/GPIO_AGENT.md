# GPIO Agent Information for STM32F051R8T6
- Generate GPIO APIs
- Generate the GPIO Interrupt APIs also.
- Generate the Example code also for GPIO in/out and Interrupts.
- Do not use any HAL library.
- Make the code short and optimized.
- Do not use any other library or files in the repository. Write the fresh code.
- Take the reference of the document at "STM32F051R8T6/AI_Generated Code/rm0360-stm32f0x1-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf".
- Take the reference of the document at "STM32F051R8T6/AI_Generated Code/stm32f051r8.pdf".

## Microcontroller Overview

**Part Number:** STM32F051R8T6  
**Family:** STM32F0x1  
**Core:** ARM Cortex-M0  
**Flash Memory:** 64 KB  
**SRAM:** 8 KB  
**Package:** LQFP64  
**Maximum Frequency:** 48 MHz  

## GPIO Ports Available

The STM32F051R8T6 microcontroller provides the following GPIO ports:

| Port | Base Address | Number of Pins | Notes |
|------|--------------|----------------|-------|
| GPIOA | 0x48000000 | 16 | PA0-PA15 |
| GPIOB | 0x48000400 | 16 | PB0-PB15 |
| GPIOC | 0x48000800 | 16 | PC0-PC15 |
| GPIOD | 0x48000C00 | 16 | PD0-PD15 |
| GPIOE | 0x48001000 | 16 | PE0-PE15 |
| GPIOF | 0x48001400 | 16 | PF0-PF15 |

**Note:** Not all pins may be available on the LQFP64 package. Refer to the datasheet for pin availability.

## GPIO Register Structure

Each GPIO port has the following registers:

| Register | Offset | Description | Reset Value |
|----------|--------|-------------|-------------|
| MODER | 0x00 | Mode Register | 0x00000000 |
| OTYPER | 0x04 | Output Type Register | 0x00000000 |
| OSPEEDR | 0x08 | Output Speed Register | 0x00000000 |
| PUPDR | 0x0C | Pull-up/Pull-down Register | 0x00000000 |
| IDR | 0x10 | Input Data Register | 0x00000000 |
| ODR | 0x14 | Output Data Register | 0x00000000 |
| BSRR | 0x18 | Bit Set/Reset Register | 0x00000000 |
| LCKR | 0x1C | Configuration Lock Register | 0x00000000 |
| AFRL | 0x20 | Alternate Function Low Register | 0x00000000 |
| AFRH | 0x24 | Alternate Function High Register | 0x00000000 |
| BRR | 0x28 | Bit Reset Register | 0x00000000 |

## GPIO Modes

Each pin can be configured in one of four modes:

| Mode | Value | Description |
|------|-------|-------------|
| INPUT | 0b00 | General purpose input |
| OUTPUT | 0b01 | General purpose output |
| ALTERNATE | 0b10 | Alternate function mode |
| ANALOG | 0b11 | Analog mode |

## GPIO Output Types

| Type | Value | Description |
|------|-------|-------------|
| PUSH_PULL | 0 | Push-pull output |
| OPEN_DRAIN | 1 | Open-drain output |

## GPIO Output Speeds

| Speed | Value | Frequency | Description |
|-------|-------|-----------|-------------|
| LOW | 0b00 | 2 MHz | Low speed |
| MEDIUM | 0b01 | 10 MHz | Medium speed |
| HIGH | 0b11 | 50 MHz | High speed |

**Note:** Value 0b10 is reserved.

## GPIO Pull-up/Pull-down Configuration

| Configuration | Value | Description |
|--------------|-------|-------------|
| NO_PULL | 0b00 | No pull-up/pull-down |
| PULL_UP | 0b01 | Pull-up resistor enabled |
| PULL_DOWN | 0b10 | Pull-down resistor enabled |

**Note:** Value 0b11 is reserved.

## Alternate Functions

The STM32F051R8T6 supports up to 8 alternate functions (AF0-AF7) per pin. The specific alternate function mapping depends on the pin and peripheral. Common alternate functions include:

- AF0: System functions (MCO, SWD, etc.)
- AF1: TIM1, TIM2
- AF2: TIM3, TIM16, TIM17
- AF3: USART1, USART2
- AF4: SPI1, SPI2
- AF5: I2C1, I2C2
- AF6: SPI1, SPI2
- AF7: USART1, USART2

**Note:** Refer to the datasheet for complete alternate function mapping.

## Clock Configuration

GPIO ports are clocked by the AHB bus. To use a GPIO port, you must enable its clock in the RCC (Reset and Clock Control) register.

**RCC AHB Enable Register (RCC_AHBENR) - Address: 0x40021014**

| Bit | Peripheral | Description |
|-----|------------|-------------|
| 17 | IOPAEN | GPIOA clock enable |
| 18 | IOPBEN | GPIOB clock enable |
| 19 | IOPCEN | GPIOC clock enable |
| 20 | IOPDEN | GPIOD clock enable |
| 21 | IOPEEN | GPIOE clock enable |
| 22 | IOPFEN | GPIOF clock enable |

## GPIO Configuration Steps

### Step 1: Enable GPIO Clock
```c
// Enable clock for GPIOC
RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
```

### Step 2: Configure GPIO Mode
```c
// Set PC8 as output
GPIOC->MODER &= ~(GPIO_MODER_MODER8_Msk);  // Clear mode bits
GPIOC->MODER |= (0x01 << GPIO_MODER_MODER8_Pos);  // Set to output mode
```

### Step 3: Configure Output Type (if output)
```c
// Set PC8 as push-pull output
GPIOC->OTYPER &= ~(GPIO_OTYPER_OT8_Msk);  // Push-pull
```

### Step 4: Configure Output Speed (if output)
```c
// Set PC8 to medium speed
GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR8_Msk);
GPIOC->OSPEEDR |= (0x01 << GPIO_OSPEEDR_OSPEEDR8_Pos);
```

### Step 5: Configure Pull-up/Pull-down
```c
// No pull-up/pull-down for PC8
GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR8_Msk);
```

### Step 6: Configure Alternate Function (if needed)
```c
// Set alternate function for PC8 (if pin < 8, use AFRL; else use AFRH)
GPIOC->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk);  // Clear AF bits
GPIOC->AFR[1] |= (0x00 << GPIO_AFRH_AFSEL8_Pos);  // Set AF0
```

## GPIO Operations

### Reading a Pin
```c
// Read PC8
uint8_t pin_state = (GPIOC->IDR & GPIO_IDR_ID8_Msk) >> GPIO_IDR_ID8_Pos;
```

### Writing a Pin
```c
// Set PC8 high
GPIOC->BSRR = GPIO_BSRR_BS8;

// Reset PC8 low
GPIOC->BRR = GPIO_BRR_BR8;
```

### Toggling a Pin
```c
// Toggle PC8
GPIOC->ODR ^= GPIO_ODR_OD8_Msk;
```

### Atomic Set/Reset
```c
// Set PC8 (atomic operation)
GPIOC->BSRR = GPIO_BSRR_BS8;

// Reset PC8 (atomic operation)
GPIOC->BSRR = GPIO_BSRR_BR8;
```

## GPIO Locking

To prevent accidental modification of GPIO configuration, you can lock the pin configuration:

```c
// Lock PC8 configuration
GPIOC->LCKR = GPIO_LCKR_LCKK | GPIO_LCKR_LCK8;  // Write 1
GPIOC->LCKR = GPIO_LCKR_LCK8;                   // Write 0
GPIOC->LCKR = GPIO_LCKR_LCKK | GPIO_LCKR_LCK8;  // Write 1
volatile uint32_t temp = GPIOC->LCKR;           // Read back

// Check if lock is successful
if (GPIOC->LCKR & GPIO_LCKR_LCKK) {
    // Lock successful
}
```

## Common Use Cases

### 1. LED Blinking (Output)
```c
// Configure PC8 as output push-pull
RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
GPIOC->MODER = (GPIOC->MODER & ~GPIO_MODER_MODER8_Msk) | (0x01 << GPIO_MODER_MODER8_Pos);
GPIOC->OTYPER &= ~GPIO_OTYPER_OT8_Msk;
GPIOC->OSPEEDR = (GPIOC->OSPEEDR & ~GPIO_OSPEEDR_OSPEEDR8_Msk) | (0x01 << GPIO_OSPEEDR_OSPEEDR8_Pos);
GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR8_Msk;

// Blink LED
while (1) {
    GPIOC->BSRR = GPIO_BSRR_BS8;    // LED ON
    HAL_Delay(500);
    GPIOC->BRR = GPIO_BRR_BR8;      // LED OFF
    HAL_Delay(500);
}
```

### 2. Button Reading (Input)
```c
// Configure PA0 as input with pull-up
RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
GPIOA->MODER &= ~GPIO_MODER_MODER0_Msk;  // Input mode
GPIOA->PUPDR = (GPIOA->PUPDR & ~GPIO_PUPDR_PUPDR0_Msk) | (0x01 << GPIO_PUPDR_PUPDR0_Pos);  // Pull-up

// Read button
if ((GPIOA->IDR & GPIO_IDR_ID0_Msk) == 0) {
    // Button pressed (active low)
}
```

### 3. UART TX Pin (Alternate Function)
```c
// Configure PA9 as USART1_TX (AF7)
RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODER9_Msk) | (0x02 << GPIO_MODER_MODER9_Pos);  // Alternate function
GPIOA->OTYPER &= ~GPIO_OTYPER_OT9_Msk;  // Push-pull
GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~GPIO_OSPEEDR_OSPEEDR9_Msk) | (0x03 << GPIO_OSPEEDR_OSPEEDR9_Pos);  // High speed
GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR9_Msk;  // No pull-up/pull-down
GPIOA->AFR[1] = (GPIOA->AFR[1] & ~GPIO_AFRH_AFSEL9_Msk) | (0x07 << GPIO_AFRH_AFSEL9_Pos);  // AF7
```

## Best Practices

1. **Always enable the GPIO clock** before configuring any GPIO registers.
2. **Use atomic operations** (BSRR/BRR) for setting/resetting pins to avoid read-modify-write issues.
3. **Configure unused pins** as analog inputs to reduce power consumption.
4. **Use appropriate output speed** based on your application requirements to minimize EMI.
5. **Enable pull-up/pull-down resistors** for input pins to prevent floating inputs.
6. **Lock critical GPIO configurations** to prevent accidental modification.
7. **Consider using open-drain outputs** for I2C and other multi-master bus applications.
8. **Check pin availability** in the datasheet before using a pin for a specific function.

## Important Notes

- The STM32F051R8T6 operates at a maximum frequency of 48 MHz.
- All GPIO pins are 5V tolerant (except for some specific pins - check datasheet).
- The microcontroller supports up to 8 mA sink/source current per pin (with some exceptions).
- Total current consumption per port should not exceed 25 mA.
- The microcontroller includes built-in ESD protection on all GPIO pins.
- Some pins have special functions (reset, SWD, etc.) that may require special handling.

## Reference Documents

- **Datasheet:** STM32F051R8 - Reference Manual (RM0091)
- **Datasheet:** STM32F051R8T6 - Datasheet
- **Programming Manual:** PM0215 - STM32F0xxx Cortex-M0 programming manual

## Example Code Structure

```c
// GPIO Configuration Example
void GPIO_Config_Example(void) {
    // Enable GPIO clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    
    // Configure PC8 as output
    GPIOC->MODER &= ~GPIO_MODER_MODER8_Msk;
    GPIOC->MODER |= (0x01 << GPIO_MODER_MODER8_Pos);
    
    // Configure as push-pull
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT8_Msk;
    
    // Set output speed to medium
    GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR8_Msk;
    GPIOC->OSPEEDR |= (0x01 << GPIO_OSPEEDR_OSPEEDR8_Pos);
    
    // No pull-up/pull-down
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR8_Msk;
}

// Main application
int main(void) {
    // Initialize system clock
    SystemClock_Config();
    
    // Configure GPIO
    GPIO_Config_Example();
    
    // Main loop
    while (1) {
        GPIOC->BSRR = GPIO_BSRR_BS8;    // Set pin high
        HAL_Delay(500);
        GPIOC->BRR = GPIO_BRR_BR8;      // Set pin low
        HAL_Delay(500);
    }
}
```

## Troubleshooting

### Issue: GPIO pin not toggling
- Check if GPIO clock is enabled
- Verify pin mode is configured correctly
- Ensure pin is not locked
- Check if alternate function is interfering

### Issue: Input pin always reads high/low
- Verify pull-up/pull-down configuration
- Check external circuit connections
- Ensure pin is not configured as output

### Issue: Unexpected behavior after reset
- Check if GPIO configuration is properly initialized
- Verify clock configuration
- Ensure proper startup code execution

---

**Last Updated:** 2026-02-09  
**Microcontroller:** STM32F051R8T6  
**Core:** ARM Cortex-M0  
**Max Frequency:** 48 MHz
