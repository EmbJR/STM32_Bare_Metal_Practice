/**
 * @file h7timer.h
 * @brief STM32H743 Timer (TIM) driver header file.
 * @author Kilo
 * @date 2026-07-06
 *
 * This header file contains definitions for Timer (TIM) peripheral on STM32H743.
 * It provides basic timer functionality including time base, PWM, input capture,
 * encoder mode, and interrupts.
 */

#ifndef H7TIMER_H
#define H7TIMER_H

#include <stdint.h>

/* Base addresses for TIM instances */
#define TIM1_BASE       ((uint32_t)0x40010000)
#define TIM2_BASE       ((uint32_t)0x40000000)
#define TIM3_BASE       ((uint32_t)0x40000400)
#define TIM4_BASE       ((uint32_t)0x40000800)
#define TIM5_BASE       ((uint32_t)0x40000C00)
#define TIM6_BASE       ((uint32_t)0x40001000)
#define TIM7_BASE       ((uint32_t)0x40001400)
#define TIM8_BASE       ((uint32_t)0x40010400)
#define TIM12_BASE      ((uint32_t)0x40001800)
#define TIM13_BASE      ((uint32_t)0x40001C00)
#define TIM14_BASE      ((uint32_t)0x40002000)
#define TIM15_BASE      ((uint32_t)0x40014000)
#define TIM16_BASE      ((uint32_t)0x40014400)
#define TIM17_BASE      ((uint32_t)0x40014800)

/* Timer register structure */
typedef struct {
    volatile uint32_t CR1;        /*!< Control register 1 */
    volatile uint32_t CR2;        /*!< Control register 2 */
    volatile uint32_t SMCR;       /*!< Slave mode control register */
    volatile uint32_t DIER;       /*!< DMA/interrupt enable register */
    volatile uint32_t SR;         /*!< Status register */
    volatile uint32_t EGR;        /*!< Event generation register */
    volatile uint32_t CCMR1;      /*!< Capture/compare mode register 1 */
    volatile uint32_t CCMR2;      /*!< Capture/compare mode register 2 */
    volatile uint32_t CCER;       /*!< Capture/compare enable register */
    volatile uint32_t CNT;        /*!< Counter */
    volatile uint32_t PSC;        /*!< Prescaler */
    volatile uint32_t ARR;        /*!< Auto-reload register */
    volatile uint32_t RCR;        /*!< Repetition counter register */
    volatile uint32_t CCR1;       /*!< Capture/compare register 1 */
    volatile uint32_t CCR2;       /*!< Capture/compare register 2 */
    volatile uint32_t CCR3;       /*!< Capture/compare register 3 */
    volatile uint32_t CCR4;       /*!< Capture/compare register 4 */
    volatile uint32_t BDTR;       /*!< Break and dead-time register */
    volatile uint32_t DCR;        /*!< DMA control register */
    volatile uint32_t DMAR;       /*!< DMA address for burst mode */
    volatile uint32_t OR1;        /*!< Option register 1 */
    volatile uint32_t CCMR3;      /*!< Capture/compare mode register 3 */
    volatile uint32_t CCR5;       /*!< Capture/compare register 5 */
    volatile uint32_t CCR6;       /*!< Capture/compare register 6 */
    volatile uint32_t OR2;        /*!< Option register 2 */
    volatile uint32_t OR3;        /*!< Option register 3 */
} TIM_TypeDef;

/* Timer counter modes */
#define TIM_COUNTERMODE_UP             ((uint32_t)0x00000000)
#define TIM_COUNTERMODE_DOWN           ((uint32_t)0x00000010)
#define TIM_COUNTERMODE_CENTERALIGNED  ((uint32_t)0x00000020)

/* PWM output polarity */
#define TIM_OCPOLARITY_HIGH            ((uint32_t)0x00000000)
#define TIM_OCPOLARITY_LOW             ((uint32_t)0x00000001)

/* PWM output mode */
#define TIM_OCMODE_TIMING              ((uint32_t)0x00000000)
#define TIM_OCMODE_ACTIVE              ((uint32_t)0x00000001)
#define TIM_OCMODE_PWM1                ((uint32_t)0x00000006)
#define TIM_OCMODE_PWM2                ((uint32_t)0x00000007)

/* Interrupt sources */
#define TIM_UPDATE_INTERRUPT           ((uint32_t)0x00000001)
#define TIM_CC1_INTERRUPT              ((uint32_t)0x00000002)
#define TIM_CC2_INTERRUPT              ((uint32_t)0x00000004)
#define TIM_CC3_INTERRUPT              ((uint32_t)0x00000008)
#define TIM_CC4_INTERRUPT              ((uint32_t)0x00000010)
#define TIM_COM_INTERRUPT              ((uint32_t)0x00000020)
#define TIM_BREAK_INTERRUPT            ((uint32_t)0x00000040)

/* Function prototypes */
void H7_TIM_Init(TIM_TypeDef *tim, uint32_t prescaler, uint32_t period, uint32_t counterMode);
void H7_TIM_DeInit(TIM_TypeDef *tim);
void H7_TIM_PWM_ConfigChannel(TIM_TypeDef *tim, uint32_t channel, uint32_t compare, uint32_t polarity, uint32_t outputMode);
void H7_TIM_Start(TIM_TypeDef *tim);
void H7_TIM_Stop(TIM_TypeDef *tim);
void H7_TIM_EnableInterrupt(TIM_TypeDef *tim, uint32_t src);
void H7_TIM_DisableInterrupt(TIM_TypeDef *tim, uint32_t src);
void H7_TIM_IC_ConfigChannel(TIM_TypeDef *tim, uint32_t channel, uint32_t filter);
void H7_TIM_Encoder_Start(TIM_TypeDef *tim);
uint32_t H7_TIM_GetCounter(TIM_TypeDef *tim);
void H7_TIM_SetCounter(TIM_TypeDef *tim, uint32_t counter);

#endif /* H7TIMER_H */