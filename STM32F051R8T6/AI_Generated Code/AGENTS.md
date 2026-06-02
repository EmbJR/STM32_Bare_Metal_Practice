# AI Agent Instructions for STM32F051R8T6 Repository

## Purpose
This repository contains bare-metal STM32F051R8T6 driver development and example code. AI agents should focus on the existing `AI_Generated Code` folders for generated drivers and the `STM32CubeIDE_Test_Project/Src` folder for the active test application.

## Key conventions
- Target MCU: **STM32F051R8T6** (Cortex-M0).
- Generated drivers must be:
  - independent of HAL/CMSIS and other external STM32 abstractions,
  - simple, modular, and API-centric,
  - implemented in `AI_Generated Code/<module>` directories.
- Many prompts explicitly require using existing local drivers from `AI_Generated Code` as dependencies.
- Some tasks are constrained to a specific folder or file set; follow the prompt exactly.

## Important directories
- `Applications/AT45DB/` — AT45DB DataFlash application driver and example code.
- `CRC/` — CRC hardware driver and example.
- `SPI/` — SPI driver and example code.
- `UART/` — UART driver and example code.
- `GPIO_AI/` — GPIO driver implementation used by other modules.
- `DeepSeek_Generated/RCC/` — RCC driver requested by some AT45DB tasks.
- `STM32CubeIDE_Test_Project/Src/` — active STM32CubeIDE project source files and debugging tests.

## Build and tooling
- Build commands appear to use `make -j4 all` under `STM32CubeIDE_Test_Project`.
- Compiler toolchain: `arm-none-eabi-gcc` for Cortex-M0.
- Example compile options in logs:
  - `-mcpu=cortex-m0`
  - `-std=gnu11`
  - `-O0`
  - `-ffunction-sections`
  - `-fdata-sections`
  - `-Wall`
  - `-fstack-usage`
  - `--specs=nano.specs`
  - `-mfloat-abi=soft`
  - `-mthumb`

## Common prompt patterns
- When a prompt requests driver code, prefer creating or updating `*.c` and `*.h` under the corresponding `AI_Generated Code/<module>` folder.
- When a prompt requests examples, place them under the module’s `Code/Examples` folder.
- When a prompt identifies a specific folder such as `STM32CubeIDE_Test_Project/Src`, do not modify files outside that folder.
- Read the local prompt text and linked documentation before making changes.

## Troubleshooting and pitfalls
- There are repeated issues in this workspace with:
  - conflicting symbol declarations between headers and source files,
  - incorrect `static` vs non-`static` function declarations,
  - wrong interrupt registration/clear logic in SPI and UART examples,
  - bad buffer usage in AT45DB page write/read implementation,
  - wrong type matches in API signatures (e.g. `uint8_t *` vs `uint16_t *`).
- If a prompt mentions using existing APIs like `SPI_SendBuffer` or `UART_EnableInterrupt`, keep those APIs consistent across the driver and example code.

## Documentation references
- `Applications/AT45DB/Document/AT45DB_Programming_KeyPoints.md`
- `SPI/Docs/SPI_Help.txt`
- `UART/Docs/UART_Help.txt` (if present)
- Local prompt files under `Applications/AT45DB/Prompt.txt`, `SPI/Prompts.txt`, `UART/Prompts.txt`, `CRC/Prompts.txt`, and `STM32CubeIDE_Test_Project/Prompts_for_STM32_Project.txt`
- STM32F051R8T6 Programming manual `STM32F051R8T6_Document/pm0215-stm32f0-series-cortexm0-programming-manual-stmicroelectronics.pdf`
- STM32F051R8T6 Reference manual `STM32F051R8T6_Document/rm0091-stm32f0x1stm32f0x2stm32f0x8-advanced-armbased-32bit-mcus-stmicroelectronics.pdf`
- STM32F051R8T6 Datasheet `STM32F051R8T6_Document/stm32f051r8.pdf`

## Agent behavior
- Use this file for repo-specific guidance, but do not duplicate large existing documentation.
- When asked to generate or fix code, preserve existing API and folder structure unless the prompt explicitly permits refactoring.
- Prefer minimal, safe changes that satisfy the prompt and avoid introducing new dependencies.
