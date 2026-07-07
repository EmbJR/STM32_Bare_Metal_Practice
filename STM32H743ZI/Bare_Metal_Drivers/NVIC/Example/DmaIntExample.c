/**
 * @file DmaIntExample.c
 * @brief Example: DMA transfer complete interrupt using NVIC driver.
 *
 * Demonstrates NVIC-based interrupt handling for DMA stream transfer
 * complete. DMA2 Stream0 is configured to copy a buffer from memory
 * to memory in block mode.
 *
 * DMA2 Stream0: IRQ 56
 * Transfer: 64 words from src to dst, interrupt on completion.
 */

#include "Bare_Metal_Drivers/NVIC/h7nvic.h"

/* =====================================================================
 *  DMA2 register structure
 * ===================================================================== */
typedef struct {
    volatile uint32_t LISR;
    volatile uint32_t HISR;
    volatile uint32_t LIFCR;
    volatile uint32_t HIFCR;
} DMA_Common_TypeDef;

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t NDTR;
    volatile uint32_t PAR;
    volatile uint32_t M0AR;
    volatile uint32_t M1AR;
    volatile uint32_t FCR;
} DMA_Stream_TypeDef;

#define DMA2_BASE               0x40020400UL
#define DMA2                    ((DMA_Common_TypeDef *)DMA2_BASE)

#define DMA2_Stream0_OFFSET     0x10U
#define DMA2_Stream0            ((DMA_Stream_TypeDef *)(DMA2_BASE + DMA2_Stream0_OFFSET))

/* DMA stream CR bits */
#define DMA_SxCR_EN             (1U << 0)
#define DMA_SxCR_TCIE           (1U << 4)
#define DMA_SxCR_DIR_Pos        6U
#define DMA_SxCR_DIR_M2M        (2U << DMA_SxCR_DIR_Pos)
#define DMA_SxCR_MINC           (1U << 10)
#define DMA_SxCR_PINC           (1U << 9)
#define DMA_SxCR_MSIZE_Pos      13U
#define DMA_SxCR_MSIZE_WORD     (2U << DMA_SxCR_MSIZE_Pos)
#define DMA_SxCR_PSIZE_Pos      11U
#define DMA_SxCR_PSIZE_WORD     (2U << DMA_SxCR_PSIZE_Pos)

/* DMA LISR bits for stream 0 */
#define DMA_LISR_TCIF0          (1U << 5)
#define DMA_LIFCR_CTCIF0        (1U << 5)

/* =====================================================================
 *  RCC clock enable for DMA2
 * ===================================================================== */
#define RCC_BASE                0x58024400UL
#define RCC_AHB1ENR             (*(volatile uint32_t *)(RCC_BASE + 0xD8U))
#define RCC_AHB1ENR_DMA2EN      (1U << 1)

/* =====================================================================
 *  Transfer buffers
 * ===================================================================== */
#define DMA_BUF_SIZE            64U
static uint32_t dma_src_buf[DMA_BUF_SIZE];
static uint32_t dma_dst_buf[DMA_BUF_SIZE];
static volatile uint8_t dma_complete = 0U;

/* =====================================================================
 *  DMA2 Stream0 callback
 * ===================================================================== */
void DMA2_Stream0_Callback(void)
{
    if (DMA2->LISR & DMA_LISR_TCIF0)
    {
        DMA2->LIFCR = DMA_LIFCR_CTCIF0;
        dma_complete = 1U;
    }
}

/* =====================================================================
 *  DMA2 Stream0 initialisation
 * ===================================================================== */
static void DMA2_Stream0_Init(uint32_t *src, uint32_t *dst, uint32_t count)
{
    RCC_AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    /* Disable stream before configuration */
    DMA2_Stream0->CR = 0U;

    /* Wait until disabled */
    while (DMA2_Stream0->CR & DMA_SxCR_EN) { }

    /* Clear pending flags */
    DMA2->LIFCR = 0x3FU;

    /* Set source, destination, and transfer count */
    DMA2_Stream0->PAR  = (uint32_t)src;
    DMA2_Stream0->M0AR = (uint32_t)dst;
    DMA2_Stream0->NDTR = count;

    /* Configure: M2M, memory increment, word size, TC interrupt */
    uint32_t cr = 0U;
    cr |= DMA_SxCR_DIR_M2M;
    cr |= DMA_SxCR_MINC;
    cr |= DMA_SxCR_PINC;
    cr |= DMA_SxCR_MSIZE_WORD;
    cr |= DMA_SxCR_PSIZE_WORD;
    cr |= DMA_SxCR_TCIE;
    DMA2_Stream0->CR = cr;
}

/* =====================================================================
 *  Start DMA transfer
 * ===================================================================== */
static void DMA2_Stream0_Start(void)
{
    dma_complete = 0U;
    DMA2_Stream0->CR |= DMA_SxCR_EN;
}

/* =====================================================================
 *  Main example
 * ===================================================================== */
int main(void)
{
    NVIC_Init();

    /* Fill source buffer with test pattern */
    for (uint32_t i = 0U; i < DMA_BUF_SIZE; i++)
    {
        dma_src_buf[i] = i * 2U;
        dma_dst_buf[i] = 0U;
    }

    DMA2_Stream0_Init(dma_src_buf, dma_dst_buf, DMA_BUF_SIZE);

    NVIC_RegisterCallback(NVIC_IRQ_DMA2_Stream0, DMA2_Stream0_Callback);
    NVIC_SetPriority(NVIC_IRQ_DMA2_Stream0, 0x20U);
    NVIC_EnableIRQ(NVIC_IRQ_DMA2_Stream0);

    DMA2_Stream0_Start();

    while (!dma_complete) { }

    /* Verify transfer:
     * dma_dst_buf should now contain the same data as dma_src_buf */

    while (1) { }

    return 0;
}
