# nRF52832 emBitz Project Configuration Guide

## Project Files Created

This document describes all files needed to compile bare-metal code for nRF52832 using emBitz IDE.

### 1. Project File
- **File**: `EmBitz_Project/nRF52832_Project.ebp`
- **Description**: Main emBitz project file with complete compiler, assembler, and linker settings
- **Device**: nRF52832_XXAA (512KB Flash, 64KB RAM)

### 2. Linker Script
- **File**: `EmBitz_Project/nrf52832.ld`
- **Description**: Memory layout for nRF52832
- **Flash**: 512KB at 0x00000000
- **RAM**: 64KB at 0x20000000
- **Stack**: 8KB
- **Heap**: 2KB

### 3. Startup File
- **File**: `EmBitz_Project/startup_nrf52832.s`
- **Description**: ARM Cortex-M4F vector table and startup code
- **Features**:
  - Vector table with all 26 interrupt handlers
  - Data section initialization (copy from flash to RAM)
  - BSS section zero initialization
  - FPU enable
  - SystemInit() call

### 4. System Configuration
- **Files**: `EmBitz_Project/system_nrf52832.h` and `system_nrf52832.c`
- **Description**: System initialization and clock configuration
- **Features**:
  - HFCLK startup (64MHz)
  - LFCLK startup (32.768kHz)
  - Peripheral base address definitions
  - IRQ number definitions

## Project Settings Summary

### Compiler Settings (ARM GCC)
| Setting | Debug | Release |
|---------|-------|---------|
| Optimization | -O0 | -O2 |
| Debug | -g2 | -g2 |
| CPU | cortex-m4 | cortex-m4 |
| Thumb Mode | -mthumb | -mthumb |
| FPU | fpv4-sp-d16 | fpv4-sp-d16 |
| Float ABI | hard | hard |
| Standard | c11 | c11 |

### Linker Settings
- **Linker Script**: `nrf52832.ld`
- **GC Sections**: Enabled (--gc-sections)
- **Memory Map**: Generated
- **Specs**: nosys.specs, nano.specs

### Define Symbols
```
NRF52832_XXAA
__CORTEX_M=4
ARM_MATH_CM4
```

## Include Paths
1. `..\EmBitz_Project` - System files
2. `..\A1_Generated_Codes_nRF\Clock\code` - Clock driver
3. `..\A1_Generated_Codes_nRF\GPIO\Code` - GPIO driver
4. `..` - Root directory

## Source Files
The project includes:
- `startup_nrf52832.s` - Startup assembly
- `system_nrf52832.c` - System initialization
- `cknRF52.c` - Clock driver (from A1_Generated_Codes_nRF)
- `gpioRF52.c` - GPIO driver (from A1_Generated_Codes_nRF)

## Building the Project

### In emBitz IDE:
1. Open `EmBitz_Project/nRF52832_Project.ebp`
2. Select Build target (Debug or Release)
3. Press F9 to build or click Build menu

### Project Structure:
```
NRF52832/
├── EmBitz_Project/
│   ├── nRF52832_Project.ebp    (Project file)
│   ├── nrf52832.ld            (Linker script)
│   ├── startup_nrf52832.s     (Startup code)
│   ├── system_nrf52832.h      (System header)
│   └── system_nrf52832.c      (System source)
├── A1_Generated_Codes_nRF/
│   ├── Clock/code/
│   │   ├── cknRF52.h
│   │   ├── cknRF52.c
│   │   └── cknRF52_Example.c
│   └── GPIO/Code/
│       ├── gpioRF52.h
│       ├── gpioRF52.c
│       └── gpioRF52_Example.c
└── Documentation/
```

## Toolchain Requirements

### ARM GCC Embedded Toolchain
- **Path**: `C:\Program Files\GNU Arm Embedded Toolchain\10 2021.10\bin`
- **Version**: 10.3.1 (or compatible)
- **Download**: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm

### Required Executables:
- `arm-none-eabi-gcc` - Compiler
- `arm-none-eabi-as` - Assembler
- `arm-none-eabi-ld` - Linker
- `arm-none-eabi-objcopy` - Object file utility

## Debugging

### J-Link Settings (in project file):
- **Device**: nRF52832_AA
- **Speed**: 4000 kHz
- **Flash Size**: 524288 bytes (512KB)
- **RAM Size**: 65536 bytes (64KB)
- **VCC**: 3.3V

## Additional Notes

1. **Bare-metal**: No external libraries (no CMSIS, HAL, or Nordic SDK)
2. **Clock Driver**: Call `Clock_ProcessEvents()` periodically in main loop
3. **GPIO**: Configure pins before use
4. **Memory**: Flash starts at 0x00000000, RAM at 0x20000000

## Troubleshooting

### Common Issues:
1. **Toolchain not found**: Update `external_toolchain_path` in .ebp file
2. **Linker errors**: Verify nrf52832.ld is in correct location
3. **Startup errors**: Ensure startup_nrf52832.s is in assembly source list
4. **Flash size**: Verify device matches nRF52832_XXAA

### Build Output Locations:
- Debug: `bin\Debug\nRF52832_BareMetal.elf`
- Release: `bin\Release\nRF52832_BareMetal.elf`
- Map files: `bin\Debug\nRF52832_BareMetal.map` / `bin\Release\nRF52832_BareMetal.map`
