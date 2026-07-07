/**
 * @file SpiIntExample.c
 * @brief Example: SPI receive interrupt using NVIC driver.
 *
 * Demonstrates NVIC-based interrupt handling for SPI1. The SPI is
 * configured in master mode. Upon receiving data, the callback
 * reads and stores the received byte.
 *
 * SPI1: APB2 bus, IRQ 35
 * SCK: PA5 (AF5), MISO: PA6 (AF5), MOSI: PA7 (AF5)
 */

#include "Bare_Metal_Drivers/NVIC/h7nvic.h"
#include "Bare_Metal_Drivers/GPIO/GPIOH743ZI.h"

/* =====================================================================
 *  SPI1 register structure
 * ===================================================================== */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t CRCPR;
    volatile uint32_t RXCRCR;
    volatile uint32_t TXCRCR;
    volatile uint32_t I2SCFGR;
    volatile uint32_t I2SPR;
} SPI_TypeDef;

#define SPI1_BASE               0x40013000UL
#define SPI1                    ((SPI_TypeDef *)SPI1_BASE)

/* SPI1 register bits */
#define SPI_CR1_SPE             (1U << 6)
#define SPI_CR1_BR_Pos          3U
#define SPI_CR1_MSTR            (1U << 2)
#define SPI_CR1_CPOL            (1U << 1)
#define SPI_CR1_CPHA            (1U << 0)

#define SPI_CR2_RXNEIE          (1U << 6)
#define SPI_CR2_SSOE            (1U << 2)

#define SPI_SR_RXNE             (1U << 0)

/* =====================================================================
 *  RCC clock enables
 * ===================================================================== */
#define RCC_BASE                0x58024400UL
#define RCC_APB2ENR             (*(volatile uint32_t *)(RCC_BASE + 0xF0U))
#define RCC_APB2ENR_SPI1EN      (1U << 12)

/* =====================================================================
 *  Receive buffer (ring buffer for demo)
 * ===================================================================== */
#define SPI_RX_BUF_SIZE         16U
static uint8_t  spi_rx_buf[SPI_RX_BUF_SIZE];
static uint32_t spi_rx_head = 0U;

/* =====================================================================
 *  SPI1 RX callback
 * ===================================================================== */
void SPI1_Callback(void)
{
    if (SPI1->SR & SPI_SR_RXNE)
    {
        uint8_t data = (uint8_t)(SPI1->DR & 0xFFU);
        spi_rx_buf[spi_rx_head % SPI_RX_BUF_SIZE] = data;
        spi_rx_head++;
    }
}

/* =====================================================================
 *  GPIO initialisation for SPI1 (AF5)
 * ===================================================================== */
static void SPI1_GPIO_Init(void)
{
    GPIO_PinConfig sck_cfg = {
        .pin = 5U, .mode = GPIO_MODE_ALTERNATE, .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH, .pull = GPIO_PULL_NONE, .alternate = 5U
    };
    GPIO_PinConfig miso_cfg = {
        .pin = 6U, .mode = GPIO_MODE_ALTERNATE, .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH, .pull = GPIO_PULL_NONE, .alternate = 5U
    };
    GPIO_PinConfig mosi_cfg = {
        .pin = 7U, .mode = GPIO_MODE_ALTERNATE, .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH, .pull = GPIO_PULL_NONE, .alternate = 5U
    };

    GPIO_Init(GPIOA, &sck_cfg);
    GPIO_Init(GPIOA, &miso_cfg);
    GPIO_Init(GPIOA, &mosi_cfg);
}

/* =====================================================================
 *  SPI1 initialisation as master
 * ===================================================================== */
static void SPI1_Init(void)
{
    RCC_APB2ENR |= RCC_APB2ENR_SPI1EN;

    SPI1->CR1 = 0U;

    /* Master mode, clock polarity 0, phase 0, baud = fPCLK/256 */
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR1 |= (7U << SPI_CR1_BR_Pos);

    /* Enable RXNE interrupt */
    SPI1->CR2 = SPI_CR2_RXNEIE;

    /* Enable SPI */
    SPI1->CR1 |= SPI_CR1_SPE;
}

/* =====================================================================
 *  Main example
 * ===================================================================== */
int main(void)
{
    NVIC_Init();

    SPI1_GPIO_Init();
    SPI1_Init();

    NVIC_RegisterCallback(NVIC_IRQ_SPI1, SPI1_Callback);
    NVIC_SetPriority(NVIC_IRQ_SPI1, 0x20U);
    NVIC_EnableIRQ(NVIC_IRQ_SPI1);

    /* Send a dummy byte to start clocking in data */
    SPI1->DR = 0xFFU;

    while (1)
    {
        /* Received data accumulates in spi_rx_buf via interrupt */
        /* Application processes it in the main loop as needed    */
    }

    return 0;
}
