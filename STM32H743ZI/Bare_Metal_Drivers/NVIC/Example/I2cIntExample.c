/**
 * @file I2cIntExample.c
 * @brief Example: I2C event and error interrupts using NVIC driver.
 *
 * Demonstrates NVIC-based interrupt handling for I2C1. Both event
 * and error interrupts are configured with separate callbacks.
 *
 * I2C1: APB1L bus, IRQ 31 (EV), IRQ 32 (ER)
 * SCL: PB6 (AF4), SDA: PB7 (AF4)
 * Mode: Master, 100 kHz standard mode
 */

#include "Bare_Metal_Drivers/NVIC/h7nvic.h"
#include "Bare_Metal_Drivers/GPIO/GPIOH743ZI.h"

/* =====================================================================
 *  I2C1 register structure
 * ===================================================================== */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t TIMINGR;
    volatile uint32_t TIMEOUTR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t PECR;
    volatile uint32_t RXDR;
    volatile uint32_t TXDR;
} I2C_TypeDef;

#define I2C1_BASE               0x40005400UL
#define I2C1                    ((I2C_TypeDef *)I2C1_BASE)

/* I2C1 register bits */
#define I2C_CR1_PE              (1U << 0)
#define I2C_CR1_TXIE            (1U << 1)
#define I2C_CR1_RXIE            (1U << 2)
#define I2C_CR1_ADDRIE          (1U << 3)
#define I2C_CR1_NACKIE          (1U << 4)
#define I2C_CR1_STOPIE          (1U << 5)
#define I2C_CR1_TCIE            (1U << 6)
#define I2C_CR1_ERRIE           (1U << 7)

#define I2C_CR2_ADD10           (1U << 12)
#define I2C_CR2_NBYTES_Pos      16U
#define I2C_CR2_RD_WRN          (1U << 10)
#define I2C_CR2_START           (1U << 13)
#define I2C_CR2_STOP            (1U << 14)
#define I2C_CR2_SADD_Pos        1U

#define I2C_ISR_TXE             (1U << 0)
#define I2C_ISR_TXIS            (1U << 1)
#define I2C_ISR_RXNE            (1U << 2)
#define I2C_ISR_ADDR            (1U << 3)
#define I2C_ISR_NACKF           (1U << 4)
#define I2C_ISR_STOPF           (1U << 5)
#define I2C_ISR_TC              (1U << 6)
#define I2C_ISR_TCR             (1U << 7)

#define I2C_ICR_ADDRCF          (1U << 3)
#define I2C_ICR_NACKCF          (1U << 4)
#define I2C_ICR_STOPCF          (1U << 5)

/* =====================================================================
 *  RCC clock enables
 * ===================================================================== */
#define RCC_BASE                0x58024400UL
#define RCC_APB1LENR            (*(volatile uint32_t *)(RCC_BASE + 0xE8U))
#define RCC_APB1LENR_I2C1EN     (1U << 21)

/* I2C timing: 100 kHz in standard mode, assuming 64 MHz I2C clock */
#define I2C_TIMINGR_100KHZ_64MHZ  0x10E0E3D2U

/* =====================================================================
 *  I2C1 event callback
 * ===================================================================== */
void I2C1_EV_Callback(void)
{
    uint32_t isr = I2C1->ISR;

    if (isr & I2C_ISR_TXIS)
    {
        I2C1->TXDR = 0xA5U;
    }

    if (isr & I2C_ISR_RXNE)
    {
        volatile uint8_t rx __attribute__((unused));
        rx = (uint8_t)(I2C1->RXDR & 0xFFU);
    }

    if (isr & I2C_ISR_ADDR)
    {
        I2C1->ICR = I2C_ICR_ADDRCF;
    }

    if (isr & I2C_ISR_STOPF)
    {
        I2C1->ICR = I2C_ICR_STOPCF;
    }
}

/* =====================================================================
 *  I2C1 error callback
 * ===================================================================== */
void I2C1_ER_Callback(void)
{
    if (I2C1->ISR & I2C_ISR_NACKF)
    {
        I2C1->ICR = I2C_ICR_NACKCF;
    }
}

/* =====================================================================
 *  GPIO initialisation for I2C1 (AF4)
 * ===================================================================== */
static void I2C1_GPIO_Init(void)
{
    GPIO_PinConfig scl_cfg = {
        .pin = 6U, .mode = GPIO_MODE_ALTERNATE, .otype = GPIO_OTYPE_OPENDRAIN,
        .speed = GPIO_SPEED_LOW, .pull = GPIO_PULL_NONE, .alternate = 4U
    };
    GPIO_PinConfig sda_cfg = {
        .pin = 7U, .mode = GPIO_MODE_ALTERNATE, .otype = GPIO_OTYPE_OPENDRAIN,
        .speed = GPIO_SPEED_LOW, .pull = GPIO_PULL_NONE, .alternate = 4U
    };

    GPIO_Init(GPIOB, &scl_cfg);
    GPIO_Init(GPIOB, &sda_cfg);
}

/* =====================================================================
 *  I2C1 initialisation
 * ===================================================================== */
static void I2C1_Init(void)
{
    RCC_APB1LENR |= RCC_APB1LENR_I2C1EN;

    /* Reset I2C */
    I2C1->CR1 &= ~I2C_CR1_PE;

    /* Configure timing for 100 kHz standard mode */
    I2C1->TIMINGR = I2C_TIMINGR_100KHZ_64MHZ;

    /* Enable interrupts */
    I2C1->CR1 = I2C_CR1_TXIE | I2C_CR1_RXIE | I2C_CR1_STOPIE |
                 I2C_CR1_ADDRIE | I2C_CR1_NACKIE | I2C_CR1_ERRIE;

    I2C1->CR1 |= I2C_CR1_PE;
}

/* =====================================================================
 *  Master transmit to slave address 0x50
 * ===================================================================== */
static void I2C1_MasterTransmit(uint8_t dev_addr, uint8_t *data, uint32_t len)
{
    I2C1->CR2 = ((uint32_t)dev_addr << I2C_CR2_SADD_Pos) | (1U << I2C_CR2_START);
}

/* =====================================================================
 *  Main example
 * ===================================================================== */
int main(void)
{
    NVIC_Init();

    I2C1_GPIO_Init();
    I2C1_Init();

    NVIC_RegisterCallback(NVIC_IRQ_I2C1_EV, I2C1_EV_Callback);
    NVIC_RegisterCallback(NVIC_IRQ_I2C1_ER, I2C1_ER_Callback);
    NVIC_SetPriority(NVIC_IRQ_I2C1_EV, 0x10U);
    NVIC_SetPriority(NVIC_IRQ_I2C1_ER, 0x10U);
    NVIC_EnableIRQ(NVIC_IRQ_I2C1_EV);
    NVIC_EnableIRQ(NVIC_IRQ_I2C1_ER);

    while (1)
    {
        /* I2C communication handled via interrupts */
    }

    return 0;
}
