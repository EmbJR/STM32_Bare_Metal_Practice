/* nRF52832 Linker Script */
/* Memory Definitions for nRF52832 */

/* Flash Memory: 512KB (0x00000000 - 0x00080000) */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 512K
    
    /* RAM Memory: 64KB (0x20000000 - 0x20010000) */
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 64K
}

/* Stack and Heap Sizes */
_stack_size = 8K;
_heap_size = 2K;

ENTRY(Reset_Handler)

/* Section Definitions */
SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;
    } >FLASH

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT>FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        end = .;
        _end = .;
    } >RAM

    .heap :
    {
        . = ALIGN(4);
        _heap_start = .;
        . = . + _heap_size;
        . = ALIGN(4);
        _heap_end = .;
    } >RAM

    .stack :
    {
        . = ALIGN(4);
        _stack_end = .;
        . = . + _stack_size;
        . = ALIGN(4);
        _stack_top = .;
    } >RAM
}

_prog_start = ORIGIN(FLASH);
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
