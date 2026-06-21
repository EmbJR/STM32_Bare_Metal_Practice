#ifndef RCC_EXAMPLES_H
#define RCC_EXAMPLES_H

#include "RCCH743ZI.h"

#define HSE_FREQUENCY_8MHZ       8000000UL
#define TARGET_SYSCLK_4MHZ        4000000UL
#define TARGET_SYSCLK_16MHZ       16000000UL
#define TARGET_SYSCLK_480MHZ      480000000UL

void Example_1A_HSI_4MHz(void);
void Example_1B_PLL_4MHz(void);
void Example_1C_CSI_4MHz(void);
void Example_1D_HSI_4MHz_SysclkDiv(void);

void Example_2A_HSE_16MHz(void);
void Example_2B_PLL_16MHz(void);
void Example_2C_CSI_16MHz(void);
void Example_2D_HSI_16MHz(void);

void Example_3A_HSE_480MHz(void);
void Example_3B_PLL_480MHz(void);
void Example_3C_CSI_480MHz(void);
void Example_3D_HSI_480MHz(void);

void RCC_PrintClockInfo(void);
void Delay_Init(void);
void Delay_ms(uint32_t ms);

#endif
