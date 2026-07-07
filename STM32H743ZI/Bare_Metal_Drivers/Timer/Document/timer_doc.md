# Timer (TIM) Driver Documentation

## Overview
This driver provides basic timer functionality for STM32H743 microcontroller. It supports multiple timer instances and various modes including time base, PWM generation, input capture, and encoder mode.

## Supported Functions

### Initialization
```c
void H7_TIM_Init(TIM_TypeDef *tim, uint32_t prescaler, uint32_t period, uint32_t counterMode);
```
- **tim**: Pointer to the TIM instance (e.g., TIM2, TIM3).
- **prescaler**: Timer clock division factor (timer clock = APB clock / (prescaler + 1)).
- **period**: Auto-reload value determining the timer period.
- **counterMode**: Counting direction (TIM_COUNTERMODE_UP, DOWN, CENTERALIGNED).

### PWM Generation
```c
void H7_TIM_PWM_ConfigChannel(TIM_TypeDef *tim, uint32_t channel, uint32_t compare, uint32_t polarity, uint32_t outputMode);
```
- **channel**: Timer output channel (1-6).
- **compare**: Compare value for duty cycle (duty cycle = compare / period).
- **polarity**: Output polarity (TIM_OCPOLARITY_HIGH or LOW).
- **outputMode**: PWM mode (TIM_OCMODE_PWM1 or PWM2).

### Input Capture
```c
void H7_TIM_IC_ConfigChannel(TIM_TypeDef *tim, uint32_t channel, uint32_t filter);
```
- **channel**: Input capture channel (1-4).
- **filter**: Digital filter value (0-15) for noise filtering.

### Encoder Mode
```c
void H7_TIM_Encoder_Start(TIM_TypeDef *tim);
```
Starts the timer in encoder mode to read quadrature encoder signals.

### Interrupt Handling
```c
void H7_TIM_EnableInterrupt(TIM_TypeDef *tim, uint32_t src);
void H7_TIM_DisableInterrupt(TIM_TypeDef *tim, uint32_t src);
```
Enables/disables timer interrupts (update, capture/compare, break, etc.).

## Clock Configuration

Timer clock is derived from the APB bus:
- TIM1, TIM8, TIM15, TIM16, TIM17: APB2 clock (typically 100 MHz)
- TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14: APB1 clock (typically 50 MHz)

If APB prescaler > 1, the timer clock is multiplied by 2.

## PWM Frequency Calculation

PWM frequency = Timer clock frequency / ((prescaler + 1) * (period + 1))

## Example Usage

### PWM Example
```c
/* TIM2 on PA0 (CH1) */
H7_TIM_Init(TIM2, 79, 999, TIM_COUNTERMODE_UP);  // 1 kHz PWM
H7_TIM_PWM_ConfigChannel(TIM2, 1, 500, TIM_OCPOLARITY_HIGH, TIM_OCMODE_PWM1);
H7_TIM_Start(TIM2);
```

### Input Capture Example
```c
/* TIM3 on PB5 (CH2) */
H7_TIM_Init(TIM3, 0, 0xFFFF, TIM_COUNTERMODE_UP);
H7_TIM_IC_ConfigChannel(TIM3, 2, 0x0F);
H7_TIM_EnableInterrupt(TIM3, TIM_CC2_INTERRUPT);
H7_TIM_Start(TIM3);
```

## Known Limitations

- Does not support advanced timer features like break input, dead-time generation, or DMA burst mode.
- Input capture filter range is limited to 0-15.
- Encoder mode is basic; does not support encoder interfaces with index signals.

## Hardware Connections

Ensure GPIO pins are configured in alternate function mode for timer outputs/inputs:
- TIM2_CH1 → PA0
- TIM3_CH2 → PB5
- Refer to STM32H743 datasheet for other channel pin mappings.

## Register Map Summary

| Register | Offset | Description |
|----------|--------|-------------|
| CR1      | 0x00   | Control register 1 |
| CR2      | 0x04   | Control register 2 |
| SMCR     | 0x08   | Slave mode control |
| DIER     | 0x0C   | DMA/interrupt enable |
| SR       | 0x10   | Status register |
| EGR      | 0x14   | Event generation |
| CCMR1    | 0x18   | Capture/compare mode 1 |
| CCMR2    | 0x1C   | Capture/compare mode 2 |
| CCER     | 0x20   | Capture/compare enable |
| CNT      | 0x24   | Counter |
| PSC      | 0x28   | Prescaler |
| ARR      | 0x2C   | Auto-reload |
| CCR1     | 0x34   | Capture/compare 1 |
| CCR2     | 0x38   | Capture/compare 2 |
| CCR3     | 0x3C   | Capture/compare 3 |
| CCR4     | 0x40   | Capture/compare 4 |
| BDTR     | 0x44   | Break and dead-time |
| DCR      | 0x48   | DMA control |
| DMAR     | 0x4C   | DMA address for burst |

(End of documentation)