/**
 * @file AdcIntExample.c
 * @brief Example: ADC end-of-conversion interrupt using NVIC driver.
 *
 * Demonstrates NVIC-based interrupt handling for ADC end-of-conversion.
 * ADC1 performs a single conversion on channel 0 (PA0), triggers an
 * interrupt on completion, and the callback reads the result.
 *
 * ADC1+ADC2 share IRQ 18.
 * GPIO: PA0 (analog input, ADC1_INP0)
 */

#include "Bare_Metal_Drivers/NVIC/h7nvic.h"
#include "Bare_Metal_Drivers/GPIO/GPIOH743ZI.h"

/* =====================================================================
 *  ADC1 register structure
 * ===================================================================== */
typedef struct {
    volatile uint32_t ISR;
    volatile uint32_t IER;
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CFGR2;
    volatile uint32_t SMPR1;
    volatile uint32_t SMPR2;
    volatile uint32_t PCSEL;
    uint32_t           RESERVED0;
    volatile uint32_t LTR1;
    volatile uint32_t HTR1;
    uint32_t           RESERVED1[2];
    volatile uint32_t SQR1;
    volatile uint32_t SQR2;
    volatile uint32_t SQR3;
    volatile uint32_t SQR4;
    volatile uint32_t DR;
    uint32_t           RESERVED2[2];
    volatile uint32_t AWD2CR;
    volatile uint32_t AWD3CR;
    uint32_t           RESERVED3;
    volatile uint32_t LTR2;
    volatile uint32_t HTR2;
    volatile uint32_t LTR3;
    volatile uint32_t HTR3;
} ADC_TypeDef;

#define ADC1_BASE               0x40022000UL
#define ADC1                    ((ADC_TypeDef *)ADC1_BASE)

/* ADC register bits */
#define ADC_ISR_EOC             (1U << 2)
#define ADC_ISR_ADRDY           (1U << 0)

#define ADC_IER_EOCIE           (1U << 2)

#define ADC_CR_ADEN             (1U << 0)
#define ADC_CR_ADDIS            (1U << 1)
#define ADC_CR_ADSTART          (1U << 2)
#define ADC_CR_BOOST_Pos        8U

#define ADC_CFGR_RES_Pos        3U
#define ADC_CFGR_RES_12BIT      (0U << ADC_CFGR_RES_Pos)

#define ADC_PCSEL_PCSEL0        (1U << 0)

#define ADC_SQR1_SQ1_Pos        6U

/* =====================================================================
 *  RCC clock enables
 * ===================================================================== */
#define RCC_BASE                0x58024400UL
#define RCC_AHB1ENR             (*(volatile uint32_t *)(RCC_BASE + 0xD8U))
#define RCC_AHB1ENR_ADC12EN     (1U << 5)

/* =====================================================================
 *  Conversion result storage
 * ===================================================================== */
static volatile uint32_t adc_result = 0U;
static volatile uint8_t  adc_done   = 0U;

/* =====================================================================
 *  ADC callback
 * ===================================================================== */
void ADC_Callback(void)
{
    if (ADC1->ISR & ADC_ISR_EOC)
    {
        adc_result = ADC1->DR;
        adc_done   = 1U;
    }
}

/* =====================================================================
 *  GPIO initialisation: PA0 as analog input
 * ===================================================================== */
static void ADC_GPIO_Init(void)
{
    GPIO_PinConfig cfg = {
        .pin  = 0U,
        .mode = GPIO_MODE_ANALOG,
        .pull = GPIO_PULL_NONE
    };
    GPIO_Init(GPIOA, &cfg);
}

/* =====================================================================
 *  ADC1 initialisation: single conversion on channel 0, 12-bit
 * ===================================================================== */
static void ADC1_Init(void)
{
    RCC_AHB1ENR |= RCC_AHB1ENR_ADC12EN;

    /* Set boost mode for ADC clock */
    ADC1->CR |= (3U << ADC_CR_BOOST_Pos);

    /* Wait for ADC voltage regulator startup */
    for (volatile uint32_t i = 0U; i < 100000U; i++) { }

    /* Calibrate */
    ADC1->CR &= ~ADC_CR_ADEN;
    for (volatile uint32_t i = 0U; i < 1000U; i++) { }
    ADC1->CR |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY)) { }

    /* 12-bit resolution */
    ADC1->CFGR = ADC_CFGR_RES_12BIT;

    /* Select ADC_INP0 (PA0) */
    ADC1->PCSEL = ADC_PCSEL_PCSEL0;

    /* Sequence: 1 conversion, channel 0 */
    ADC1->SQR1 = (0U << ADC_SQR1_SQ1_Pos);

    /* Sample time: default (SMPR1 = 0) */

    /* Enable end-of-conversion interrupt */
    ADC1->IER = ADC_IER_EOCIE;
}

/* =====================================================================
 *  Trigger a conversion
 * ===================================================================== */
static void ADC1_StartConversion(void)
{
    adc_done = 0U;
    ADC1->CR |= ADC_CR_ADSTART;
}

/* =====================================================================
 *  Main example
 * ===================================================================== */
int main(void)
{
    NVIC_Init();

    ADC_GPIO_Init();
    ADC1_Init();

    NVIC_RegisterCallback(NVIC_IRQ_ADC, ADC_Callback);
    NVIC_SetPriority(NVIC_IRQ_ADC, 0x10U);
    NVIC_EnableIRQ(NVIC_IRQ_ADC);

    while (1)
    {
        ADC1_StartConversion();

        while (!adc_done) { }

        /* adc_result contains the 12-bit conversion value */
        /* Application processes it here                    */
    }

    return 0;
}
