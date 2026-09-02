
#ifndef F051NVIC_H
#define F051NVIC_H

#include <stdint.h>
#include "gpio.h"
/*============================================================================
 * EXTI Line Definitions
 *============================================================================*/
#define EXTI_BASE      (0x40010400UL)
#define EXTI_IMR        (*(volatile uint32_t *)(EXTI_BASE + 0x00))
#define EXTI_EMR        (*(volatile uint32_t *)(EXTI_BASE + 0x04))
#define EXTI_RTSR       (*(volatile uint32_t *)(EXTI_BASE + 0x08))
#define EXTI_FTSR       (*(volatile uint32_t *)(EXTI_BASE + 0x0C))
#define EXTI_SWIER      (*(volatile uint32_t *)(EXTI_BASE + 0x10))
#define EXTI_PR         (*(volatile uint32_t *)(EXTI_BASE + 0x14))

/* EXTI Line Configuration */
#define EXTI_LINE_0    (0x0001UL)
#define EXTI_LINE_1    (0x0002UL)
#define EXTI_LINE_2    (0x0004UL)
#define EXTI_LINE_3    (0x0008UL)
#define EXTI_LINE_4    (0x0010UL)
#define EXTI_LINE_5    (0x0020UL)
#define EXTI_LINE_6    (0x0040UL)
#define EXTI_LINE_7    (0x0080UL)
#define EXTI_LINE_8    (0x0100UL)
#define EXTI_LINE_9    (0x0200UL)
#define EXTI_LINE_10   (0x0400UL)
#define EXTI_LINE_11   (0x0800UL)
#define EXTI_LINE_12   (0x1000UL)
#define EXTI_LINE_13   (0x2000UL)
#define EXTI_LINE_14   (0x4000UL)
#define EXTI_LINE_15   (0x8000UL)

/* SYSCFG Base Address */
#define SYSCFG_BASE    (0x40010000UL)
#define SYSCFG_CR      (*(volatile uint32_t *)(SYSCFG_BASE + 0x00))
#define SYSCFG_EXTICR1 (*(volatile uint32_t *)(SYSCFG_BASE + 0x08))
#define SYSCFG_EXTICR2 (*(volatile uint32_t *)(SYSCFG_BASE + 0x0C))
#define SYSCFG_EXTICR3 (*(volatile uint32_t *)(SYSCFG_BASE + 0x10))
#define SYSCFG_EXTICR4 (*(volatile uint32_t *)(SYSCFG_BASE + 0x14))

/* EXTI Trigger Types */
typedef enum {
    EXTI_TRIGGER_RISING    = 0x01,  /*!< Rising edge trigger */
    EXTI_TRIGGER_FALLING   = 0x02,  /*!< Falling edge trigger */
    EXTI_TRIGGER_BOTH      = 0x03   /*!< Rising and falling edge trigger */
} EXTI_TriggerTypeDef;

/*============================================================================
 * NVIC Definitions
 *============================================================================*/
#define NVIC_BASE       (0xE000E100UL)
#define NVIC_ISER       (*(volatile uint32_t *)(NVIC_BASE + 0x00))
#define NVIC_ICER       (*(volatile uint32_t *)(NVIC_BASE + 0x80))
#define NVIC_IPR        ((volatile uint8_t *)(0xE000E400UL))

/* Interrupt Numbers */
#define EXTI0_1_IRQn       5   /*!< EXTI Line 0 and 1 Interrupt */
#define EXTI2_3_IRQn       6   /*!< EXTI Line 2 and 3 Interrupt */
#define EXTI4_15_IRQn      7   /*!< EXTI Line 4 to 15 Interrupt */

/*============================================================================
 * IRQn_Type Definition (Simplified for STM32F0xx)
 *============================================================================*/
typedef int32_t IRQn_Type;

/*============================================================================
 * NVIC Driver API Prototypes
 *============================================================================*/
void NVIC_EnableIRQ(IRQn_Type IRQn);
void NVIC_DisableIRQ(IRQn_Type IRQn);
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t Priority);
void NVIC_ClearPendingIRQ(IRQn_Type IRQn);

/*============================================================================
 * EXTI Driver API Prototypes
 *============================================================================*/
void EXTI_DeInit(void);
void EXTI_LineConfig(uint32_t EXTI_Line, GPIO_TypeDef *GPIOx, uint16_t Pin);
void EXTI_EnableExtiLine(uint32_t EXTI_Line, uint8_t Enable);
void EXTI_SetTrigger(uint32_t EXTI_Line, EXTI_TriggerTypeDef Trigger);
void EXTI_ClearFlag(uint32_t EXTI_Line);
uint32_t EXTI_GetFlag(uint32_t EXTI_Line);
uint32_t EXTI_GetPending(uint32_t EXTI_Line);
void EXTI_ClearPending(uint32_t EXTI_Line);

#endif	// F051NVIC_H
