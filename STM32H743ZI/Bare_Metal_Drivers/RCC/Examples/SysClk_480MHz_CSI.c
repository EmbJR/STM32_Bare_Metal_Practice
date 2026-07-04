/*
 * Example: System clock = 480 MHz, source = CSI via PLL1
 * ---------------------------------------------------------------
 *  NOTE: STM32H743 has no MSI oscillator (that is an STM32L4 part).
 *  The lowest-power internal RC oscillator on H7 is the CSI, at 4 MHz.
 *  This example therefore uses CSI as the PLL reference.
 *
 *  Clock tree:
 *      CSI (4 MHz)  -->  PLL1
 *                        DIVM = 1   (prescaler  /1)
 *                        DIVN = 120 (multiplier  x120)
 *                        DIVP = 0   (encoding 0 -> output /1)
 *      VCO = 4 * 120 = 480 MHz
 *      PLL1_P = VCO / 1 = 480 MHz  -->  SYSCLK
 *
 *  NOTE on DIVP encoding (per RM0433):
 *      0 -> /1,  1 -> /2,  2 -> NOT ALLOWED,  3 -> /4, ...
 *  Only ODD encodings produce valid output divisions (/2, /4, /6 ... /128).
 *
 *  PLL input frequency range = 2-4 MHz   =>  PLL1RGE = 01
 *  VCO range 150-420 MHz     =>  PLL1VCOSEL = 1 (medium range)
 *
 *  Bus prescalers: AHB /2  (240 MHz),  APBx /2 (120 MHz)
 *  Flash latency = 4 wait states.
 *  Voltage scaling: VOS1 required.
 */

#include "RCCH743ZI.h"

void SystemClock_480MHz_CSI(void)
{
    /* 1) Enable CSI and wait */
    RCC_CSIEnable(1);
    while (!RCC_CSIRdy()) { /* wait */ }

    /* 2) Flash latency: 4 wait states for 240 MHz AHB */
    RCC_SetFlashLatency(4);

    /* 3) Bus prescalers */
    RCC_SetD1CPRE   (AHB_PRESCALER_DIV1);
    RCC_SetAHBPrescaler (AHB_PRESCALER_DIV2);
    RCC_SetD1PPRE   (APB_PRESCALER_DIV2);
    RCC_SetD2PPRE1  (APB_PRESCALER_DIV2);
    RCC_SetD2PPRE2  (APB_PRESCALER_DIV2);
    RCC_SetD3PPRE   (APB_PRESCALER_DIV2);

    /* 4) Configure PLL1 */
    RCC_PLL1_Init_t pll1 = {
        .pll_src    = RCC_PLLSRC_CSI,
        .divm       = 1,
        .divn       = 120,
        .divp       = 0,            /* encoding 0 -> output /1 -> 480 MHz */
        .divq       = 0,
        .divr       = 0,
        .pll_rge    = 1,            /* 2-4 MHz reference     */
        .pll_vcosel = 1,            /* medium VCO 150-420 MHz */
        .enable_p   = 1,
        .enable_q   = 0,
        .enable_r   = 0
    };
    RCC_PLL1Config(&pll1);
    RCC_PLL1Enable(1);
    while (!RCC_PLL1Ready()) { /* wait */ }

    /* 5) Switch SYSCLK to PLL1 */
    RCC_SetSysClockSrc(RCC_SYSCLK_PLL1);
}
