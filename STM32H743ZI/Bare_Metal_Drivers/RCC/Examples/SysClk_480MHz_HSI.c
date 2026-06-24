/*
 * Example: System clock = 480 MHz (max), source = HSI via PLL1
 * ---------------------------------------------------------------
 *  Clock tree:
 *      HSI (64 MHz)
 *         |-- /8 (HSIDIV)  = 8 MHz
 *         |     |
 *         |   PLL1
 *         |     DIVM = 1  (prescaler  /1)
 *         |     DIVN = 120 (multiplier  x120)
 *         |     DIVP = 2   (post divider /2)
 *         |
 *         v
 *      VCO = 8 * 120 = 960 MHz
 *      PLL1_P = VCO / 2 = 480 MHz  -->  SYSCLK
 *
 *  PLL input frequency range = 8-16 MHz  =>  PLL1RGE = 11
 *  VCO range 192-836 MHz      =>  PLL1VCOSEL = 0 (wide range)
 *
 *  Bus prescalers:
 *      D1CPRE  = /1
 *      HPRE    = /2  (so AHB = 240 MHz, max allowed)
 *      D1PPRE  = /2  (so APB3 = 120 MHz, max allowed)
 *      D2PPRE1 = /2  (so APB1 = 120 MHz)
 *      D2PPRE2 = /2  (so APB2 = 120 MHz)
 *      D3PPRE  = /2  (so APB4 = 120 MHz)
 *
 *  Flash latency = 4 wait states (required for 240 MHz AHB at VOS1).
 *
 *  Voltage scaling:
 *      This example assumes the device is in VOS1 (highest performance).
 *      VOS1 must be selected in PWR CR1 before changing the system clock.
 */

#include "RCCH743ZI.h"

void SystemClock_480MHz_HSI(void)
{
    /* 1) Enable HSI and wait */
    RCC_HSIEnable(1);
    while (!RCC_HSIReady()) { /* wait */ }

    /* 2) HSIDIV = /8  =>  hsi_ck = 8 MHz (pll reference) */
    RCC_HSIConfig(3);                 /* 3 = /8 */

    /* 3) Flash latency: 4 wait states for 240 MHz AHB at VOS1 */
    RCC_SetFlashLatency(4);

    /* 4) Bus prescalers (do this BEFORE switching to PLL) */
    RCC_SetD1CPRE   (AHB_PRESCALER_DIV1);
    RCC_SetAHBPrescaler (AHB_PRESCALER_DIV2);
    RCC_SetD1PPRE   (APB_PRESCALER_DIV2);
    RCC_SetD2PPRE1  (APB_PRESCALER_DIV2);
    RCC_SetD2PPRE2  (APB_PRESCALER_DIV2);
    RCC_SetD3PPRE   (APB_PRESCALER_DIV2);

    /* 5) Configure PLL1 */
    RCC_PLL1_Init_t pll1 = {
        .pll_src    = RCC_PLLSRC_HSI,
        .divm       = 1,
        .divn       = 120,
        .divp       = 2,
        .divq       = 0,
        .divr       = 0,
        .pll_rge    = 3,            /* 8-16 MHz reference   */
        .pll_vcosel = 0,            /* wide VCO 192-836 MHz */
        .enable_p   = 1,
        .enable_q   = 0,
        .enable_r   = 0
    };
    RCC_PLL1Config(&pll1);
    RCC_PLL1Enable(1);
    while (!RCC_PLL1Ready()) { /* wait */ }

    /* 6) Switch SYSCLK to PLL1 */
    RCC_SetSysClockSrc(RCC_SYSCLK_PLL1);
}
