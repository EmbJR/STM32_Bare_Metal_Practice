/**
 * @file h7timer.c
 * @brief STM32H743 Timer (TIM) driver source file.
 * @author Kilo
 * @date 2026-07-06
 *
 * This source file implements the Timer (TIM) driver for STM32H743.
 * It provides functions for timer initialization, PWM generation,
 * input capture, encoder mode, and interrupt handling.
 */

#include "h7timer.h"

/**
 * @brief Initialize a timer peripheral.
 *
 * Configures the timer with the specified prescaler, period, and counter mode.
 *
 * @param tim Pointer to the TIM instance.
 * @param prescaler Prescaler value (timer clock divided by (prescaler + 1)).
 * @param period Auto-reload value.
 * @param counterMode Counter mode: TIM_COUNTERMODE_UP, DOWN, or CENTERALIGNED.
 */
void H7_TIM_Init(TIM_TypeDef *tim, uint32_t prescaler, uint32_t period, uint32_t counterMode)
{
    /* Reset the timer to default state */
    tim->CR1 = 0;
    tim->CR2 = 0;
    tim->SMCR = 0;
    tim->DIER = 0;
    tim->SR = 0;
    tim->EGR = 0;
    tim->CCMR1 = 0;
    tim->CCMR2 = 0;
    tim->CCMR3 = 0;
    tim->CCER = 0;
    tim->CNT = 0;
    tim->PSC = prescaler;
    tim->ARR = period;
    tim->RCR = 0;
    tim->CCR1 = 0;
    tim->CCR2 = 0;
    tim->CCR3 = 0;
    tim->CCR4 = 0;
    tim->CCR5 = 0;
    tim->CCR6 = 0;
    tim->BDTR = 0;
    tim->DCR = 0;
    tim->DMAR = 0;
    tim->OR1 = 0;
    tim->OR2 = 0;
    tim->OR3 = 0;

    /* Configure counter mode */
    tim->CR1 &= ~0x00000010;  /* Clear DIR bit */
    tim->CR1 |= counterMode;

    /* Enable auto-reload preload if center-aligned mode */
    if (counterMode == TIM_COUNTERMODE_CENTERALIGNED)
    {
        tim->CR1 |= 0x00000080;  /* Enable ARPE */
    }
}

/**
 * @brief Deinitialize a timer peripheral.
 *
 * Resets the timer to its default state.
 *
 * @param tim Pointer to the TIM instance.
 */
void H7_TIM_DeInit(TIM_TypeDef *tim)
{
    /* Reset all registers to default */
    tim->CR1 = 0;
    tim->CR2 = 0;
    tim->SMCR = 0;
    tim->DIER = 0;
    tim->SR = 0;
    tim->EGR = 0;
    tim->CCMR1 = 0;
    tim->CCMR2 = 0;
    tim->CCMR3 = 0;
    tim->CCER = 0;
    tim->CNT = 0;
    tim->PSC = 0;
    tim->ARR = 0;
    tim->RCR = 0;
    tim->CCR1 = 0;
    tim->CCR2 = 0;
    tim->CCR3 = 0;
    tim->CCR4 = 0;
    tim->CCR5 = 0;
    tim->CCR6 = 0;
    tim->BDTR = 0;
    tim->DCR = 0;
    tim->DMAR = 0;
    tim->OR1 = 0;
    tim->OR2 = 0;
    tim->OR3 = 0;
}

/**
 * @brief Configure a timer channel for PWM output.
 *
 * @param tim Pointer to the TIM instance.
 * @param channel Timer channel (1-6).
 * @param compare Compare value for PWM duty cycle.
 * @param polarity Output polarity: TIM_OCPOLARITY_HIGH or LOW.
 * @param outputMode Output mode: TIM_OCMODE_PWM1 or PWM2.
 */
void H7_TIM_PWM_ConfigChannel(TIM_TypeDef *tim, uint32_t channel, uint32_t compare, uint32_t polarity, uint32_t outputMode)
{
    uint32_t shift;
    uint32_t mask;

    /* Set compare value */
    switch (channel)
    {
        case 1:
            tim->CCR1 = compare;
            break;
        case 2:
            tim->CCR2 = compare;
            break;
        case 3:
            tim->CCR3 = compare;
            break;
        case 4:
            tim->CCR4 = compare;
            break;
        case 5:
            tim->CCR5 = compare;
            break;
        case 6:
            tim->CCR6 = compare;
            break;
        default:
            return; /* Invalid channel */
    }

    /* Configure output mode and polarity */
    shift = (channel - 1) * 8;
    mask = 0xFF << shift;

    /* Clear OCxM bits and OCxPE bit */
    tim->CCMR1 = (tim->CCMR1 & ~mask) | ((outputMode & 0x07) << shift);
    if (channel <= 4)
    {
        tim->CCMR1 = (tim->CCMR1 & ~(1 << ((channel - 1) * 8 + 3))) | ((outputMode & 0x08) ? (1 << ((channel - 1) * 8 + 3)) : 0);
    }

    /* Set output polarity */
    if (channel <= 4)
    {
        if (polarity == TIM_OCPOLARITY_LOW)
        {
            tim->CCER |= (1 << ((channel - 1) * 4 + 1));
        }
        else
        {
            tim->CCER &= ~(1 << ((channel - 1) * 4 + 1));
        }
    }

    /* Enable output on the channel */
    if (channel <= 4)
    {
        tim->CCER |= (1 << ((channel - 1) * 4));
    }
}

/**
 * @brief Start the timer counter.
 *
 * @param tim Pointer to the TIM instance.
 */
void H7_TIM_Start(TIM_TypeDef *tim)
{
    tim->CR1 |= 0x00000001;  /* Enable counter */
}

/**
 * @brief Stop the timer counter.
 *
 * @param tim Pointer to the TIM instance.
 */
void H7_TIM_Stop(TIM_TypeDef *tim)
{
    tim->CR1 &= ~0x00000001;  /* Disable counter */
}

/**
 * @brief Enable a timer interrupt.
 *
 * @param tim Pointer to the TIM instance.
 * @param src Interrupt source (TIM_UPDATE_INTERRUPT, TIM_CC1_INTERRUPT, etc.).
 */
void H7_TIM_EnableInterrupt(TIM_TypeDef *tim, uint32_t src)
{
    tim->DIER |= src;
}

/**
 * @brief Disable a timer interrupt.
 *
 * @param tim Pointer to the TIM instance.
 * @param src Interrupt source (TIM_UPDATE_INTERRUPT, TIM_CC1_INTERRUPT, etc.).
 */
void H7_TIM_DisableInterrupt(TIM_TypeDef *tim, uint32_t src)
{
    tim->DIER &= ~src;
}

/**
 * @brief Configure a timer channel for input capture.
 *
 * @param tim Pointer to the TIM instance.
 * @param channel Timer channel (1-4).
 * @param filter Digital filter value.
 */
void H7_TIM_IC_ConfigChannel(TIM_TypeDef *tim, uint32_t channel, uint32_t filter)
{
    uint32_t shift = (channel - 1) * 8;
    uint32_t mask = 0xFF << shift;

    /* Configure channel for input capture */
    tim->CCMR1 = (tim->CCMR1 & ~mask) | (0x01 << shift);  /* CC1S = 01 for input capture */

    /* Set filter */
    tim->CCMR1 = (tim->CCMR1 & ~(0xF0 << shift)) | ((filter & 0x0F) << (shift + 4));
}

/**
 * @brief Start encoder mode on a timer.
 *
 * @param tim Pointer to the TIM instance.
 */
void H7_TIM_Encoder_Start(TIM_TypeDef *tim)
{
    tim->SMCR |= 0x00000003;  /* SMS = 011 for encoder mode */
    tim->CCMR1 |= 0x00000003; /* CC1S = 01 and CC2S = 10 for encoder mode */
    tim->CCER &= ~0x00000011; /* CC1P and CC2P cleared */
    tim->CR1 |= 0x00000001;   /* Enable counter */
}

/**
 * @brief Get the current timer counter value.
 *
 * @param tim Pointer to the TIM instance.
 * @return Current counter value.
 */
uint32_t H7_TIM_GetCounter(TIM_TypeDef *tim)
{
    return tim->CNT;
}

/**
 * @brief Set the timer counter value.
 *
 * @param tim Pointer to the TIM instance.
 * @param counter Value to set.
 */
void H7_TIM_SetCounter(TIM_TypeDef *tim, uint32_t counter)
{
    tim->CNT = counter;
}