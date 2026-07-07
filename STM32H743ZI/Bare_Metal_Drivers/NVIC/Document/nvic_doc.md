# NVIC Driver Documentation (STM32H743 Cortex-M7)

## Overview

The NVIC (Nested Vectored Interrupt Controller) driver provides a complete bare-metal API for managing all 150 maskable interrupts on the STM32H743ZIT6 microcontroller. The Cortex-M7 NVIC enables low-latency interrupt handling with 16 programmable priority levels, priority grouping, tail-chaining, and late-arriving preemption.

**References:**
- PM0253: STM32F7/H7 Cortex-M7 Programming Manual
- RM0433 Rev 7/8: STM32H742/H743/H753/H750 Reference Manual (Vector Table, §12.3)

## File Structure

```
NVIC/
  h7nvic.h               Header with register map, IRQ enum, API prototypes
  h7nvic.c               Implementation + weak default ISR handlers
  Example/
    GPIOExtiExample.c          GPIO EXTI interrupt with callback
    TimerIntExampleCC1.c       Timer capture/compare interrupt
    UartIntExample.c           UART RX interrupt (echo)
    SpiIntExample.c            SPI RX interrupt
    I2cIntExample.c            I2C event + error interrupts
    AdcIntExample.c            ADC end-of-conversion interrupt
    DmaIntExample.c            DMA transfer complete interrupt
    PriorityGroupingExample.c  Priority grouping + preemption demo
  Document/
    nvic_doc.md                This documentation
```

## NVIC Register Map

| Register   | Base Address | Description |
|------------|-------------|-------------|
| **NVIC**   | `0xE000E100` | NVIC base (Private Peripheral Bus) |
| ISER0..7   | `0xE000E100` | Interrupt Set-Enable (write 1 = enable) |
| ICER0..7   | `0xE000E180` | Interrupt Clear-Enable (write 1 = disable) |
| ISPR0..7   | `0xE000E200` | Interrupt Set-Pending (write 1 = pend) |
| ICPR0..7   | `0xE000E280` | Interrupt Clear-Pending (write 1 = unpend) |
| IABR0..7   | `0xE000E300` | Interrupt Active Bit (read-only) |
| IPR0..149  | `0xE000E400` | Interrupt Priority (8-bit byte per IRQ) |
| STIR       | `0xE000EF00` | Software Trigger Interrupt |
| **SCB**    | `0xE000ED00` | System Control Block |
| AIRCR      | `0xE000ED0C` | Priority grouping (PRIGROUP bits [10:8]) |

## Priority Configuration

- **4 priority bits** implemented (`NVIC_PRIO_BITS = 4`)
- Valid priority values: `0x00`, `0x10`, `0x20`, ..., `0xF0`
- Lower value = higher priority
- Priority grouping via `SCB_AIRCR.PRIGROUP` splits bits into group + subpriority

### Priority Grouping Options

| Constant                  | PRIGROUP | Group Bits | Subpriority Bits |
|---------------------------|----------|------------|------------------|
| `NVIC_PRIORITYGROUP_0`   | 7        | 0          | 4                |
| `NVIC_PRIORITYGROUP_1`   | 6        | 1          | 3                |
| `NVIC_PRIORITYGROUP_2`   | 5        | 2          | 2                |
| `NVIC_PRIORITYGROUP_3`   | 4        | 3          | 1                |
| `NVIC_PRIORITYGROUP_4`   | 3        | 4          | 0                |

## API Reference

### Initialization

```c
void NVIC_Init(void);
void NVIC_DeInit(void);
```

- `NVIC_Init()` clears all callbacks, sets priority grouping to default (no split).
- `NVIC_DeInit()` disables all interrupts, clears pending, resets callbacks.

### Interrupt Enable / Disable

```c
void     NVIC_EnableIRQ      (NVIC_IRQn_t irq);
void     NVIC_DisableIRQ     (NVIC_IRQn_t irq);
uint32_t NVIC_GetEnableState (NVIC_IRQn_t irq);
```

- Use ISER/ICER registers (write 1 semantics, write 0 has no effect).
- `NVIC_GetEnableState()` reads ISER and returns 1 if enabled.

### Pending Interrupt Management

```c
void     NVIC_SetPendingIRQ  (NVIC_IRQn_t irq);
void     NVIC_ClearPendingIRQ(NVIC_IRQn_t irq);
uint32_t NVIC_GetPendingIRQ  (NVIC_IRQn_t irq);
```

- Software can set/clear pending to force an ISR invocation for testing.

### Active Interrupt Status

```c
uint32_t NVIC_GetActiveIRQ   (NVIC_IRQn_t irq);
```

- Returns 1 if the interrupt handler is currently executing or preempted.

### Priority Management

```c
void     NVIC_SetPriority        (NVIC_IRQn_t irq, uint32_t priority);
uint32_t NVIC_GetPriority        (NVIC_IRQn_t irq);
void     NVIC_SetPriorityGrouping(uint32_t priorityGroup);
```

- Priority must be `0x00`, `0x10`, `0x20`, ... `0xF0`.
- Priority grouping changes require the VECTKEY write sequence.

### Software Trigger

```c
void NVIC_SoftwareTrigger(NVIC_IRQn_t irq);
```

- Sets the pending bit for the specified IRQ (ISR executes if enabled).

### Callback Registration

```c
void NVIC_RegisterCallback  (NVIC_IRQn_t irq, NVIC_Callback_t callback);
void NVIC_UnregisterCallback(NVIC_IRQn_t irq);
```

- Each IRQ supports one user callback (`void (*)(void)`).
- Callbacks are invoked from weak default ISR handlers.
- Override by defining a function with the ISR name in your application.

### Sleep Mode

```c
void NVIC_EnterSleep(void);
void NVIC_EnterSleepOnExit(uint8_t enable);
```

- Executes `WFI` instruction.
- `NVIC_EnterSleepOnExit()` configures SLEEPONEXIT mode (auto re-enter sleep after ISR).

## ISR Handler Names (Weak Defaults)

All 150 handlers are provided as weak functions in `h7nvic.c`. Users can override them. The default handler calls the registered callback. EXTI handlers (EXTI0..4, EXTI9_5, EXTI15_10) also clear the EXTI pending flag automatically.

Key handler names (override with your own):

| IRQ | Handler |
|-----|---------|
| 6  | `EXTI0_IRQHandler` |
| 7  | `EXTI1_IRQHandler` |
| 8  | `EXTI2_IRQHandler` |
| 9  | `EXTI3_IRQHandler` |
| 10 | `EXTI4_IRQHandler` |
| 23 | `EXTI9_5_IRQHandler` |
| 40 | `EXTI15_10_IRQHandler` |
| 28 | `TIM2_IRQHandler` |
| 31 | `I2C1_EV_IRQHandler` |
| 35 | `SPI1_IRQHandler` |
| 37 | `USART1_IRQHandler` |
| 18 | `ADC_IRQHandler` |
| 56 | `DMA2_Stream0_IRQHandler` |

## Interrupt Vector Table (All 150 IRQs)

| IRQn | Peripheral | IRQn | Peripheral | IRQn | Peripheral |
|------|-----------|------|-----------|------|-----------|
| 0 | WWDG | 50 | TIM5 | 100 | OTG_FS_WKUP |
| 1 | PVD_AVD | 51 | SPI3 | 101 | OTG_FS |
| 2 | TAMP_STAMP | 52 | UART4 | 102 | DMAMUX1_OVR |
| 3 | RTC_WKUP | 53 | UART5 | 103 | HRTIM1_Master |
| 4 | FLASH | 54 | TIM6_DAC | 104 | HRTIM1_TIMA |
| 5 | RCC | 55 | TIM7 | 105 | HRTIM1_TIMB |
| 6 | EXTI0 | 56 | DMA2_Stream0 | 106 | HRTIM1_TIMC |
| 7 | EXTI1 | 57 | DMA2_Stream1 | 107 | HRTIM1_TIMD |
| 8 | EXTI2 | 58 | DMA2_Stream2 | 108 | HRTIM1_TIME |
| 9 | EXTI3 | 59 | DMA2_Stream3 | 109 | HRTIM1_FLT |
| 10 | EXTI4 | 60 | DMA2_Stream4 | 110 | DFSDM1_FLT0 |
| 11 | DMA1_Stream0 | 61 | ETH | 111 | DFSDM1_FLT1 |
| 12 | DMA1_Stream1 | 62 | ETH_WKUP | 112 | DFSDM1_FLT2 |
| 13 | DMA1_Stream2 | 63 | FDCAN_CAL | 113 | DFSDM1_FLT3 |
| 14 | DMA1_Stream3 | 68 | DMA2_Stream5 | 114 | SAI3 |
| 15 | DMA1_Stream4 | 69 | DMA2_Stream6 | 115 | SWPMI1 |
| 16 | DMA1_Stream5 | 70 | DMA2_Stream7 | 116 | TIM15 |
| 17 | DMA1_Stream6 | 71 | USART6 | 117 | TIM16 |
| 18 | ADC | 72 | I2C3_EV | 118 | TIM17 |
| 19 | FDCAN1_IT0 | 73 | I2C3_ER | 119 | MDIOS_WKUP |
| 20 | FDCAN2_IT0 | 74 | OTG_HS_EP1_OUT | 120 | MDIOS |
| 21 | FDCAN1_IT1 | 75 | OTG_HS_EP1_IN | 121 | JPEG |
| 22 | FDCAN2_IT1 | 76 | OTG_HS_WKUP | 122 | MDMA |
| 23 | EXTI9_5 | 77 | OTG_HS | 124 | SDMMC2 |
| 24 | TIM1_BRK | 78 | DCMI | 125 | HSEM1 |
| 25 | TIM1_UP | 80 | RNG | 127 | ADC3 |
| 26 | TIM1_TRG_COM | 81 | FPU | 128 | DMAMUX2_OVR |
| 27 | TIM1_CC | 82 | UART7 | 129 | BDMA_Channel0 |
| 28 | TIM2 | 83 | UART8 | 130 | BDMA_Channel1 |
| 29 | TIM3 | 84 | SPI4 | 131 | BDMA_Channel2 |
| 30 | TIM4 | 85 | SPI5 | 132 | BDMA_Channel3 |
| 31 | I2C1_EV | 86 | SPI6 | 133 | BDMA_Channel4 |
| 32 | I2C1_ER | 87 | SAI1 | 134 | BDMA_Channel5 |
| 33 | I2C2_EV | 88 | LTDC | 135 | BDMA_Channel6 |
| 34 | I2C2_ER | 89 | LTDC_ER | 136 | BDMA_Channel7 |
| 35 | SPI1 | 90 | DMA2D | 137 | COMP |
| 36 | SPI2 | 91 | SAI2 | 138 | LPTIM2 |
| 37 | USART1 | 92 | QUADSPI | 139 | LPTIM3 |
| 38 | USART2 | 93 | LPTIM1 | 140 | LPTIM4 |
| 39 | USART3 | 94 | CEC | 141 | LPTIM5 |
| 40 | EXTI15_10 | 95 | I2C4_EV | 142 | LPUART1 |
| 41 | RTC_Alarm | 96 | I2C4_ER | 144 | CRS |
| 43 | TIM8_BRK_TIM12 | 97 | SPDIF_RX | 145 | ECC |
| 44 | TIM8_UP_TIM13 | 98 | OTG_FS_EP1_OUT | 146 | SAI4 |
| 45 | TIM8_TRG_COM_TIM14 | 99 | OTG_FS_EP1_IN | 149 | WAKEUP_PIN |
| 46 | TIM8_CC | | | | |
| 47 | DMA1_Stream7 | | | | |
| 48 | FMC | | | | |
| 49 | SDMMC1 | | | | |

> Reserved slots (42, 64-67, 79, 123, 126, 143, 147-148) have handler stubs with `RESERVEDnn_IRQHandler` names.

## Configuration Flow

1. **Initialize NVIC:** `NVIC_Init()`
2. **Set priority grouping:** `NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4)`
3. **Configure peripheral and its interrupt source:** e.g., enable UART RX interrupt in USART_CR1
4. **Set interrupt priority:** `NVIC_SetPriority(NVIC_IRQ_USART1, 0x10U)`
5. **Register callback:** `NVIC_RegisterCallback(NVIC_IRQ_USART1, MyCallback)`
6. **Enable interrupt in NVIC:** `NVIC_EnableIRQ(NVIC_IRQ_USART1)`
7. **In ISR or callback:** handle the event, clear peripheral flags

## Typical Usage Scenarios

### Basic Interrupt (with callback)
```c
NVIC_Init();
NVIC_RegisterCallback(NVIC_IRQ_TIM2, MyTim2Handler);
NVIC_SetPriority(NVIC_IRQ_TIM2, 0x30U);
NVIC_EnableIRQ(NVIC_IRQ_TIM2);
// TIM2_IRQHandler (weak) calls MyTim2Handler when interrupt fires
```

### Overriding ISR Directly
```c
void TIM2_IRQHandler(void) {
    // Clear flags manually, do application work
    // This overrides the weak default in h7nvic.c
}
NVIC_EnableIRQ(NVIC_IRQ_TIM2);
```

### Priority Preemption
```c
NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_3);
NVIC_SetPriority(NVIC_IRQ_EXTI0, 0x00U);  // Highest
NVIC_SetPriority(NVIC_IRQ_TIM2,  0x80U);  // Lower
// EXTI0 will preempt TIM2 ISR
```

### Software Trigger (Testing)
```c
NVIC_EnableIRQ(NVIC_IRQ_TIM2);
NVIC_SoftwareTrigger(NVIC_IRQ_TIM2);  // Fires the ISR immediately
```

## Known Limitations

1. **No hardware error handling in default ISRs:** Default handlers assume the interrupt is genuine. For robust systems, override handlers with proper flag-checking, especially for shared vectors (EXTI9_5, EXTI15_10).

2. **Single callback per IRQ:** Only one callback function per IRQ. If multiple modules need notification, implement a dispatcher in your callback.

3. **No interrupt nesting control:** The NVIC hardware handles tail-chaining and late-arriving preemption automatically. But BASEPRI/FAULTMASK control is not exposed in this driver.

4. **System exceptions not covered:** Only maskable interrupts (IRQ 0..149). System exceptions (HardFault, MemManage, BusFault, UsageFault, SVCall, PendSV, SysTick) require separate handling.

5. **Shared interrupt vectors:** `TIM8_BRK_TIM12`, `TIM8_UP_TIM13`, `TIM8_TRG_COM_TIM14`, and `TIM6_DAC` share vectors. The user callback must determine which peripheral triggered the interrupt by checking status registers.
