# AGENTS.md

This file provides guidance to agents when working with code in this repository.


## Documents required to understand befor code generation.
- nRF52832 Product Specification (Documentation/nRF52832_PS_v1.9.pdf)
- nRF52832 Errata (Documentation/nRF52832_Rev_3_Errata_v1.3.pdf)

## Project Overview
- Bare-metal C driver development for Nordic Semiconductor nRF52832 microcontroller
- No external library dependencies (no CMSIS, HAL, or SDK)
- Code must be compiled with ARM toolchain (ARM GCC, Keil, or IAR) and flashed to hardware

## Critical Non-Obvious Patterns

### Event Processing Required
- Callback functions require `Clock_ProcessEvents()` to be called periodically in main loop or interrupt handler
- Without this, registered callbacks will never execute even when events occur

### Clock Source Configuration
- LFCLK source (`Clock_LFCLK_SetSource()`) can only be changed when LFCLK is stopped
- Attempting to change source while running has no effect (silently fails)

### Event Flag Clearing
- Events must be cleared after reading in `Clock_ProcessEvents()` - hardware requires explicit clear
- Not clearing events will prevent subsequent event detection

### Register Access
- All register accesses use volatile pointer casting via `CLOCK_REG()` macro
- Never cache register values that can change hardware state

## File Structure
- `A1_Generated_Codes_nRF/Clock/code/` - Clock driver (cknRF52.h/.c)
- `A1_Generated_Codes_nRF/GPIO/Code/` - GPIO driver (in development)
- `Documentation/` - nRF52832 Product Specification PDF

## Code Conventions
- Prefix all functions with driver name (e.g., `Clock_`, `GPIO_`)
- Use enum types for configuration options
- Header guards use `__CK_NRF52_H` format
- Callback functions registered via dedicated registration APIs