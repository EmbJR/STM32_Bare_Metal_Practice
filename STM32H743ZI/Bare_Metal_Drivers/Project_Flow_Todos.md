Project Overview
- You are to develop bare‑metal peripheral drivers for the STM32H743ZIT6 microcontroller, without using CMSIS, HAL, or any third‑party libraries. All code must be written from scratch, using only the document given below as a basis. 
1> Document `Bare_Metal_Drivers/Reference_Manual_stm32h742-stm32h743-Mem_Addresses.pdf`
2> Document inside folder in each driver folders for which the driver will be developed(Ex:- for RCC go to `Bare_Metal_Drivers/RCC/Document` and for SPI go to `Bare_Metal_Drivers/SPI/Document`).

The final deliverable for each peripheral driver is a complete, modular, well‑documented set of files, following the exact folder structure below.

📁 Folder Structure (Mandatory)
text
Main_folder/
│
├── Bare_Metal_Drivers/
│   │
│   ├── RCC/
│   │   ├── h7rcc.c
│   │   ├── h7rcc.h
│   │   ├── Example/
│   │   │   ├── rccExample18MHz.c
│   │   │   ├── rccExample232MHz.c
│   │   │   └── ... (other frequency examples)
│   │   └── Document/
│   │       └── h7rcc_doc.pdf (or .md) 
│   │
│   ├── PWR/          (Power Control)
│   │   ├── h7pwr.c
│   │   ├── h7pwr.h
│   │   ├── Example/
│   │   │   ├── pwrExample1.c
│   │   │   ├── pwrExample2.c
│   │   │   └── ...
│   │   └── Document/
│   │       └── h7pwr_doc.pdf
│   │
│   ├── GPIO/
│   │   ├── h7gpio.c
│   │   ├── h7gpio.h
│   │   ├── Example/
│   │   │   ├── gpioExample1.c
│   │   │   ├── gpioExample2.c
│   │   │   └── ...
│   │   └── Document/
│   │       └── h7gpio_doc.pdf
│   │
│   ├── TIM/          (Timer / PWM)
│   │   ├── h7tim.c
│   │   ├── h7tim.h
│   │   ├── Example/...
│   │   └── Document/...
│   │
│   ├── UART/
│   │   ├── h7uart.c
│   │   ├── h7uart.h
│   │   ├── Example/...
│   │   └── Document/...
│   │
│   ├── SPI/
│   │   ├── h7spi.c
│   │   ├── h7spi.h
│   │   ├── Example/...
│   │   └── Document/...
│   │
│   ├── I2C/
│   │   ├── h7i2c.c
│   │   ├── h7i2c.h
│   │   ├── Example/...
│   │   └── Document/...
│   │
│   ├── ADC/
│   │   ├── h7adc.c
│   │   ├── h7adc.h
│   │   ├── Example/...
│   │   └── Document/...
│   │
│   ├── DAC/
│   │   ├── h7dac.c
│   │   ├── h7dac.h
│   │   ├── Example/...
│   │   └── Document/...
│   │
│   ├── DMA/
│   │   ├── h7dma.c
│   │   ├── h7dma.h
│   │   ├── Example/...
│   │   └── Document/...
│   │
│   └── I2S/
│       ├── h7i2s.c
│       ├── h7i2s.h
│       ├── Example/...
│       └── Document/...
│
└── (other project files)
# 🧩 General Rules for Every Driver
No Dependencies – All code must be standalone. No inclusion of stm32h7xx.h, cmsis_*, or any HAL/LL headers. Use only your own register definitions (derived from the reference manual).

## Header File (.h) must contain:
- Base addresses for the peripheral (using #define).
- Register structure definitions (bit‑fields, unions, or plain #define masks).
- All necessary bit‑mask constants.
- Function prototypes for all public APIs.
- Inline static helper functions if appropriate (e.g., for reading/writing registers).

# Source File (.c) must implement:

- Initialisation/de‑initialisation functions.
- Configuration functions (clock, mode, baud rate, etc.).
- Enable/disable functions (peripheral clock gating, interrupts, etc.).
- Interrupt service routines (if interrupt support is provided, they should be weak or user‑defined).
- Low‑power mode handling (e.g., sleep, stop, standby when relevant).
- Comprehensive in‑line comments explaining register settings, timing, and rationale.
- Example Files – At least minimum two distinct usage examples per driver, demonstrating different configurations or use cases.
- Documentation – A PDF (or Markdown that can be converted) containing:

# Overview of the peripheral.
- List of all functions with descriptions, parameters, return values.
- Configuration flow (initialisation steps).
- Register map summary (if helpful).
- Typical usage scenarios.
- Any known limitations or hardware errata.
- Coding Style – Clean, modular, easy to read. Use consistent naming (e.g., H7_RCC_Init(), H7_GPIO_SetMode(), etc.). 
- Avoid global variables; use context structures where appropriate.

# 📌 Development Priority Order (Stages)
You will be asked to implement each stage one at a time. The following list of the stages to be finished when asked:

- RCC – Reset and Clock Control (system clock setup, peripheral clock enable/disable, clock security system).
- PWR – Power Control (voltage scaling, low‑power modes, wake‑up sources).
- GPIO – General‑Purpose I/O (modes, speed, pull‑up/down, alternate functions, interrupts).
- TIM – Timers (basic, general‑purpose, advanced; PWM generation, input capture, output compare, encoder mode).
- UART – Universal Asynchronous Receiver/Transmitter (baud rate, word length, parity, flow control, interrupts, DMA).
- SPI – Serial Peripheral Interface (master/slave, clock polarity/phase, data size, interrupts, DMA).
- I2C – Inter‑Integrated Circuit (master/slave, standard/fast/fast+ modes, interrupts, DMA).
- ADC – Analog‑to‑Digital Converter (channels, resolution, sampling time, continuous/scan/injected modes, interrupts, DMA).
- DAC – Digital‑to‑Analog Converter (output channels, trigger, buffer, waveform generation).
- DMA – Direct Memory Access (streams/channels, transfer types, circular mode, interrupts, double‑buffer).
- I2S – Inter‑IC Sound (audio protocols, master/slave, data format, interrupts, DMA).

# 🔍 Detailed Stage Descriptions
When I ask you to work on a specific stage, follow the corresponding detailed instructions below.
# ----------------------------------------------------------------------------------------------------------------------------------------------------
## Stage 1 – RCC (Reset and Clock Control)
----------------------------------------------------------------------------------------------------------------------------------------------------
Target peripheral: RCC (Reset and Clock Control) of STM32H743.

### Must support:
- All clock sources: HSI (64 MHz), HSE (external crystal, e.g., 8–48 MHz), PLL1, PLL2, PLL3 with programmable multiplication/division.
- System clock source selection (HSI, HSE, or PLL1).
- AHB and APB bus prescalers (AHB1, AHB2, AHB3, AHB4; APB1, APB2, APB3, APB4).
- Peripheral clock enable/disable for all buses.
- Clock security system (CSS) for HSE.
- Low‑speed clocks: LSI (32 kHz) and LSE (32.768 kHz) for RTC and independent watchdog.
- Reset cause detection (power‑on, external, watchdog, etc.).

### Execution flow.
1. **Gather Documentation:** Read the documentation resides in path `Bare_Metal_Drivers/RCC/Document` and identify RCC base addresses and register layouts (e.g. `RCC_CR`, `RCC_PLLCFGR`, etc.) and study the clock tree diagrams.
2. **Define Registers in Header:** In `Bare_Metal_Drivers/RCC/h7rcc.h`, `#define` each RCC register address and bit mask (HSI ready, PLL ready, prescalers, AHB/APB enable bits, etc.). Follow the document in the `Bare_Metal_Drivers/RCC/Document` folder for the bitfield names for clarity.
3. **Implement API in Source:** In `h7rcc.c`, write functions to: reset RCC to defaults; enable/disable clocks for peripherals (write to `RCC_AHBxENR`, `RCC_APB1/2ENR`); configure the system clock. Below is the sample list of functions to be implement.
c
void H7_RCC_Init(void);                      // Default system clock (e.g., 400 MHz from HSE+PLL)
void H7_RCC_DeInit(void);                   // Reset to default (HSI)
void H7_RCC_ConfigSystemClock(uint32_t src, uint32_t pll_m, uint32_t pll_n, uint32_t pll_p, uint32_t pll_q, uint32_t pll_r);
void H7_RCC_ConfigAHBPrescaler(uint32_t prescaler);
void H7_RCC_ConfigAPBPrescaler(uint32_t apb1, uint32_t apb2, uint32_t apb3, uint32_t apb4);
void H7_RCC_PeriphClockEnable(uint32_t periph);
void H7_RCC_PeriphClockDisable(uint32_t periph);
void H7_RCC_ResetPeriph(uint32_t periph);
uint32_t H7_RCC_GetSysClockFreq(void);
uint32_t H7_RCC_GetHCLKFreq(void);
uint32_t H7_RCC_GetPCLK1Freq(void);
// ... plus functions for LSI/LSE, CSS, reset flags, etc.
4. **Examples:** In `Bare_Metal_Drivers/RCC/Example/`, create C examples demonstrating common clock setups.
- Examples:
rccExample18MHz.c – configure system clock to 18 MHz using HSE (if external 8 MHz with PLL).
rccExample232MHz.c – configure system clock to 232 MHz using HSI + PLL (or HSE).
5. **Documentation:** In the `Document` folder, include a PDF or markdown that explains the RCC configuration. Describe the clock tree (PLL equations, prescalers), safe startup steps, and how each register is configured. For example, cite that the PLL can multiply up to 480 MHz on H7. Include comment headers in `h7rcc.c` explaining each register write.
6. **Validation:** Develop test cases or simple hardware tests to verify the RCC driver. For example, confirm that after configuration the CPU clock is as expected (measure GPIO toggle rate). Ensure peripheral clocks (GPIO, USART, etc.) enable bits work correctly.

# ----------------------------------------------------------------------------------------------------------------------------------------------------
## Stage 2 – PWR (Power Control)
----------------------------------------------------------------------------------------------------------------------------------------------------
Target: Power control (PWR) of STM32H743.

### Must support:
- Voltage scaling (scale 0, 1, 2, 3) for different performance/power trade‑offs.
- Low‑power modes: Sleep, Stop (0, 1, 2), Standby, VBAT.
- Wake‑up sources (from RTC, external pins, etc.).
- Power domain control (D1, D2, D3) and independent domain power down.
- Peripheral voltage regulator configuration (main regulator, low‑power regulator).
- Backup domain access control.

### Execution flow.
1. **Gather Documentation:** Read the documentation resides in path `Bare_Metal_Drivers/PWR/Document` and study the PWR chapter in document to understand power domains and modes (Run, Sleep, Stop, Standby). Note how to switch voltage scaling and enable wakeup sources. Also identify PWR registers (e.g. `PWR_CR1`, `PWR_CR3`, `PWR_CPUCR` etc.) and voltage regulator options.
2. **Define PWR Registers:** In `Bare_Metal_Drivers/Power/h7pwr.h`, `#define` base addresses and bit masks for PWR registers as per the manual.
3. **Implement API:** In `h7pwr.c`, write functions to configure power: 
e.g.     
    c
    void H7_PWR_Init(void);
    void H7_PWR_DeInit(void);
    void H7_PWR_SetVoltageScale(uint32_t scale);
    void H7_PWR_EnterSleepMode(void);
    void H7_PWR_EnterStopMode(uint32_t stop_level); // 0,1,2
    void H7_PWR_EnterStandbyMode(void);
    void H7_PWR_EnableWakeUpPin(uint32_t pin);
    void H7_PWR_DisableWakeUpPin(uint32_t pin);
    void H7_PWR_EnableBackupAccess(void);
    void H7_PWR_DisableBackupAccess(void);
    // ...
Include functions to clear standby flags and resume. Handle unlocking if needed (some PWR bits require key sequence).
4. **Examples:** In `Bare_Metal_Drivers/PWR/Example/`,  

Examples:
pwrExample1.c – enter Stop mode and wake on external interrupt.
pwrExample2.c – use Standby mode with RTC wake‑up.
pwrExample3.c – System full run mode with power to the basic functional peripherals are enabled.
pwrExample4.c - write a demo like entering Stop mode with a timer to wake up. Show the CPU going to low power and resuming.
5. **Documentation:** In `Bare_Metal_Drivers/PWR/Document`, explain power modes and regulator levels. For instance, note that LSI (32.768 kHz) is used for RTC in Standby. Describe how you programmed the registers to achieve a mode (e.g. setting `PWR_CR3_VOS` bits, etc.).


## Stage 3: GPIO Driver Development


## Stage 4 – TIM (Timer / PWM)
Target: General‑purpose timers (TIM2–TIM5, TIM12–TIM17) and advanced timers (TIM1, TIM8) – but at least one representative timer.

### Must support:
- Time base configuration (prescaler, auto‑reload, counter mode up/down/center‑aligned).
- Output compare/PWM generation (multiple channels).
- Input capture (frequency/period measurement).
- Encoder mode.
- One‑pulse mode.
- Interrupts (update, capture/compare, trigger, etc.).
- DMA burst transfers.

### Execution flow.
1. **Identify Timers:** Read the documentation resides in path `Bare_Metal_Drivers/Timer/Document` and study all Timer registers. Note which timers are present and on which bus. (E.g. TIM1 and TIM8 on APB2; TIM2–7 on APB1).
2. **Header File:** In `Bare_Metal_Drivers/Timer/h7timer.h`, define timer base addresses and bitfields for prescaler, counters, and control bits.
3. **Source File:** In `h7timer.c`, implement functions like `Timer_Init(timer_base, prescaler, period)`, `Timer_Start()`, and interrupts (e.g. enable update interrupt). Configure CCMR registers for PWM modes (edge-aligned, center-aligned).
Functions:
c
void H7_TIM_Init(TIM_TypeDef *tim, uint32_t prescaler, uint32_t period, uint32_t counterMode);
void H7_TIM_DeInit(TIM_TypeDef *tim);
void H7_TIM_PWM_ConfigChannel(TIM_TypeDef *tim, uint32_t channel, uint32_t compare, uint32_t polarity, uint32_t outputMode);
void H7_TIM_Start(TIM_TypeDef *tim);
void H7_TIM_Stop(TIM_TypeDef *tim);
void H7_TIM_EnableInterrupt(TIM_TypeDef *tim, uint32_t src);
void H7_TIM_DisableInterrupt(TIM_TypeDef *tim, uint32_t src);
// ... and for input capture, encoder, etc.
4. **Examples:** In `Timer/Example/`, write code to generate a 1 kHz square wave or a PWM with adjustable duty on a GPIO pin using timer output. For instance, blink an LED using an update interrupt or vary LED brightness via PWM.  
5. **Documentation:** In `Timer/Document`, explain the timer clock (e.g. timer clock = APB1/2 freq * multiplier if APB prescaler >1), prescaler and auto-reload formula, and how you set CCRx for PWM duty. Reference that GPIO pins must be set to the timer’s alternate function.


## Stage 5 – NVIC controller
Target: NVIC based supporting functions development.

### Must support:
- Support all interrupts for peripherals including External interrupts.
- Interrupt enable/disable api for all peripherals in STM32H743ZI controller.
- Interrupt priority set/Get APIs.
- Sleep mode support if needed.
- User callback register for all Interrupt vectors. (User function can be set as the Interrupt handler).

### Execution flow.
1. **Identify Timers:** Read the below documents and study the NVIC memory addresses and also the important information about NVIC. Also study the bit fields of the NVIC related registers for programming.
    - All documents in the `Bare_Metal_Drivers/NVIC/Document` folder.
    - Programming menual for STM32H7 at the path        "Documents_H7\Reference_and_Programming_Manual_stm32h742-stm32h743-753-and-stm32h750\pm0253-stm32f7-series-and-stm32h7-series-cortexm7-processor-programming-manual-stmicroelectronics.pdf.

2. **Header File:** In `Bare_Metal_Drivers/NVIC/h7nvic.h`, define NVIC base addresses and bitfields for different settings (like Enable/Disable the peripheral interrupts, Set priority of interrupts etc).

3. **Source File:** In `h7nvic.c`, Inplement the functions for below requirements.
    - Enable/Disable the individual interrupts (Ex:- Timers, SPI, UART,...etc). 
    - Set priority of interrupts.
    - User function set API for interrupt callback for each and every peripherals.
    - Interrupt handlers for each and every peripherals with interrupt flags clear if needed and calls the user functions.
4. **Examples:** Examples for all peripherals interrupt configurations and interrupt handler user function register. For example generate file GPIOExtiExample.c for GPIO external interrupts and TimerIntExampleCC1.c for Timer1 Capture Compare interrupt. Do it for all peripherals like I2C, SPI, UART, I2S, ADC, Comparator, DAC, USB, CAN, etc..
5. **Documentation:** Generate the detailed documentation for interrupt configurations and interrupt handling functions. Also describe about each registers and its bitfields for each interrupts.