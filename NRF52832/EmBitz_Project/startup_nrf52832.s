/* Startup file for nRF52832 ARM Cortex-M4F */
/* Generated for emBitz IDE */

.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16

/* Vector Table */
.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
    .word _stack_top              /* Top of Stack */
    .word Reset_Handler           /* Reset Handler */
    .word NMI_Handler             /* NMI Handler */
    .word HardFault_Handler       /* Hard Fault Handler */
    .word MemManage_Handler       /* MPU Fault Handler */
    .word BusFault_Handler        /* Bus Fault Handler */
    .word UsageFault_Handler      /* Usage Fault Handler */
    .word 0                       /* Reserved */
    .word 0                       /* Reserved */
    .word 0                       /* Reserved */
    .word 0                       /* Reserved */
    .word SVC_Handler             /* SVCall Handler */
    .word DebugMon_Handler        /* Debug Monitor Handler */
    .word 0                       /* Reserved */
    .word PendSV_Handler          /* PendSV Handler */
    .word SysTick_Handler         /* SysTick Handler */

    /* External Interrupts */
    .word POWER_CLOCK_IRQHandler
    .word RADIO_IRQHandler
    .word UARTE0_UART0_IRQHandler
    .word SPIM0_SPIS0_TWIM0_TWIS0_IRQHandler
    .word SPIM1_SPIS1_TWIM1_TWIS1_IRQHandler
    .word NFCT_IRQHandler
    .word GPIOTE_IRQHandler
    .word SAADC_IRQHandler
    .word TIMER0_IRQHandler
    .word TIMER1_IRQHandler
    .word TIMER2_IRQHandler
    .word RTC0_IRQHandler
    .word TEMP_IRQHandler
    .word RNG_IRQHandler
    .word ECB_IRQHandler
    .word CCM_AAR_IRQHandler
    .word WDT_IRQHandler
    .word RTC1_IRQHandler
    .word QDEC_IRQHandler
    .word COMP_LPCOMP_IRQHandler
    .word SWI0_EGU0_IRQHandler
    .word SWI1_EGU1_IRQHandler
    .word SWI2_EGU2_IRQHandler
    .word SWI3_EGU3_IRQHandler
    .word SWI4_EGU4_IRQHandler
    .word SWI5_EGU5_IRQHandler
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0

.size g_pfnVectors, .-g_pfnVectors

/* Reset Handler */
.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
    /* Copy data sections from flash to RAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    movs r3, #0
    b CheckCopyData

CopyDataLoop:
    ldr r4, [r2, r3]
    str r4, [r0, r3]
    adds r3, r3, #4

CheckCopyData:
    adds r4, r0, r3
    cmp r4, r1
    bcc CopyDataLoop
    ldr r2, =_sbss
    ldr r4, =_ebss
    movs r3, #0
    b CheckFillZero

FillZeroLoop:
    str r3, [r2], #4

CheckFillZero:
    cmp r2, r4
    bcc FillZeroLoop

    /* Enable FPU */
    ldr r0, =0xE000ED88
    ldr r1, [r0]
    orr r1, r1, #(0xF << 20)
    str r1, [r0]
    dsb
    isb

    /* Call SystemInit */
    bl SystemInit

    /* Call main */
    bl main

    /* If main returns, loop forever */
InfiniteLoop:
    b InfiniteLoop

.size Reset_Handler, .-Reset_Handler

/* Default Handler */
.section .text.Default_Handler,"ax",%progbits
Default_Handler:
InfiniteLoop_1:
    b InfiniteLoop_1
.size Default_Handler, .-Default_Handler

/* Macro for Exception Handlers */
.macro Default_Handler_Entry name
.weak \name
.set \name, Default_Handler
.endm

/* Exception Handlers */
Default_Handler_Entry NMI_Handler
Default_Handler_Entry HardFault_Handler
Default_Handler_Entry MemManage_Handler
Default_Handler_Entry BusFault_Handler
Default_Handler_Entry UsageFault_Handler
Default_Handler_Entry SVC_Handler
Default_Handler_Entry DebugMon_Handler
Default_Handler_Entry PendSV_Handler
Default_Handler_Entry SysTick_Handler

/* External Interrupts */
Default_Handler_Entry POWER_CLOCK_IRQHandler
Default_Handler_Entry RADIO_IRQHandler
Default_Handler_Entry UARTE0_UART0_IRQHandler
Default_Handler_Entry SPIM0_SPIS0_TWIM0_TWIS0_IRQHandler
Default_Handler_Entry SPIM1_SPIS1_TWIM1_TWIS1_IRQHandler
Default_Handler_Entry NFCT_IRQHandler
Default_Handler_Entry GPIOTE_IRQHandler
Default_Handler_Entry SAADC_IRQHandler
Default_Handler_Entry TIMER0_IRQHandler
Default_Handler_Entry TIMER1_IRQHandler
Default_Handler_Entry TIMER2_IRQHandler
Default_Handler_Entry RTC0_IRQHandler
Default_Handler_Entry TEMP_IRQHandler
Default_Handler_Entry RNG_IRQHandler
Default_Handler_Entry ECB_IRQHandler
Default_Handler_Entry CCM_AAR_IRQHandler
Default_Handler_Entry WDT_IRQHandler
Default_Handler_Entry RTC1_IRQHandler
Default_Handler_Entry QDEC_IRQHandler
Default_Handler_Entry COMP_LPCOMP_IRQHandler
Default_Handler_Entry SWI0_EGU0_IRQHandler
Default_Handler_Entry SWI1_EGU1_IRQHandler
Default_Handler_Entry SWI2_EGU2_IRQHandler
Default_Handler_Entry SWI3_EGU3_IRQHandler
Default_Handler_Entry SWI4_EGU4_IRQHandler
Default_Handler_Entry SWI5_EGU5_IRQHandler

/* SystemInit - System Clock Configuration */
.section .text.SystemInit,"ax",%progbits
.weak SystemInit
.type SystemInit, %function

SystemInit:
    /* Configure HFCLK clock source */
    /* Configure LFCLK clock source */
    /* Enable peripheral clocks */
    bx lr

.size SystemInit, .-SystemInit

/* Data section addresses (from linker script) */
.section .data
.global _sdata
.global _edata
.global _sidata
.global _sbss
.global _ebss

/* These symbols are defined by the linker script */
.extern _stack_top
.extern _sdata
.extern _edata
.extern _sidata
.extern _sbss
.extern _ebss
