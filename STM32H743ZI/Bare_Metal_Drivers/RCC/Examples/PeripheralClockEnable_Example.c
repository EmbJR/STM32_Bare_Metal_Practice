/*
 * Example: enabling peripheral clocks
 * ---------------------------------------------------------------
 *  Shows how to enable clocks for common peripherals (GPIO, USART,
 *  SPI, I2C, TIM, ADC, DMA) using the convenience helpers in the
 *  RCC driver.  Each helper uses the correct enable / reset register
 *  for the bus the peripheral is on.
 */

#include "RCCH743ZI.h"

static void delay(volatile uint32_t n) { while (n--) __asm__ volatile("nop"); }

void PeripheralClockEnable_Example(void)
{
    /* GPIO ports on AHB4 */
    RCC_PeriphEnable(RCC_AHB4_GPIOA);
    RCC_PeriphEnable(RCC_AHB4_GPIOB);
    RCC_PeriphEnable(RCC_AHB4_GPIOC);
    RCC_PeriphEnable(RCC_AHB4_GPIOD);
    RCC_PeriphEnable(RCC_AHB4_GPIOE);
    RCC_PeriphEnable(RCC_AHB4_GPIOF);
    RCC_PeriphEnable(RCC_AHB4_GPIOG);
    RCC_PeriphEnable(RCC_AHB4_GPIOH);
    RCC_PeriphEnable(RCC_AHB4_GPIOI);
    RCC_PeriphEnable(RCC_AHB4_GPIOJ);
    RCC_PeriphEnable(RCC_AHB4_GPIOK);

    /* USART / UART */
    RCC_PeriphEnable(RCC_APB2_USART1);    /* APB2 */
    RCC_PeriphEnable(RCC_APB1L_USART2);   /* APB1L */
    RCC_PeriphEnable(RCC_APB1L_USART3);
    RCC_PeriphEnable(RCC_APB1L_UART4);
    RCC_PeriphEnable(RCC_APB1L_UART5);
    RCC_PeriphEnable(RCC_APB2_USART6);
    RCC_PeriphEnable(RCC_APB1L_UART7);
    RCC_PeriphEnable(RCC_APB1L_UART8);
    RCC_PeriphEnable(RCC_APB4_LPUART1);

    /* SPI */
    RCC_PeriphEnable(RCC_APB2_SPI1);      /* APB2 */
    RCC_PeriphEnable(RCC_APB1L_SPI2);
    RCC_PeriphEnable(RCC_APB1L_SPI3);
    RCC_PeriphEnable(RCC_APB2_SPI4);
    RCC_PeriphEnable(RCC_APB2_SPI5);
    RCC_PeriphEnable(RCC_APB4_SPI6);

    /* I2C */
    RCC_PeriphEnable(RCC_APB1L_I2C1);
    RCC_PeriphEnable(RCC_APB1L_I2C2);
    RCC_PeriphEnable(RCC_APB1L_I2C3);
    RCC_PeriphEnable(RCC_APB4_I2C4);

    /* Timers (basic, general-purpose, advanced) */
    RCC_PeriphEnable(RCC_APB1L_TIM2);
    RCC_PeriphEnable(RCC_APB1L_TIM3);
    RCC_PeriphEnable(RCC_APB1L_TIM4);
    RCC_PeriphEnable(RCC_APB1L_TIM5);
    RCC_PeriphEnable(RCC_APB1L_TIM6);
    RCC_PeriphEnable(RCC_APB1L_TIM7);
    RCC_PeriphEnable(RCC_APB1L_TIM12);
    RCC_PeriphEnable(RCC_APB1L_TIM13);
    RCC_PeriphEnable(RCC_APB1L_TIM14);
    RCC_PeriphEnable(RCC_APB2_TIM1);
    RCC_PeriphEnable(RCC_APB2_TIM8);
    RCC_PeriphEnable(RCC_APB2_TIM15);
    RCC_PeriphEnable(RCC_APB2_TIM16);
    RCC_PeriphEnable(RCC_APB2_TIM17);

    /* ADC */
    RCC_PeriphEnable(RCC_AHB1_ADC12);
    RCC_PeriphEnable(RCC_AHB4_ADC3);

    /* DMA */
    RCC_PeriphEnable(RCC_AHB1_DMA1);
    RCC_PeriphEnable(RCC_AHB1_DMA2);
    RCC_PeriphEnable(RCC_AHB3_MDMA);
    RCC_PeriphEnable(RCC_AHB3_DMA2D);
    RCC_PeriphEnable(RCC_AHB4_BDMA);

    /* Reset example - reset GPIOA */
    RCC_PeriphReset(RCC_AHB4_GPIOA);
    delay(100);
    RCC_PeriphEnable(RCC_AHB4_GPIOA);

    /* Direct bus-level access (e.g. enable multiple peripherals at once) */
    RCC_AHB1_ClkEnable(RCC_AHB1_DMA1 | RCC_AHB1_DMA2, 1);
    RCC_APB1L_ClkEnable(RCC_APB1L_I2C1 | RCC_APB1L_I2C2 | RCC_APB1L_I2C3, 1);
    RCC_APB4_ClkEnable(RCC_APB4_SYSCFG, 1);
}
