/* system_nrf52832.c - System Initialization for nRF52832 */
/* Bare-metal driver configuration */

#include "system_nrf52832.h"

/* nRF52832 Peripheral Addresses */
#define PERIPHERAL_BASE           0x40000000UL
#define NRF_POWER_BASE            (PERIPHERAL_BASE + 0x00000000)
#define NRF_CLOCK_BASE            (PERIPHERAL_BASE + 0x00001000)
#define NRF_RADIO_BASE            (PERIPHERAL_BASE + 0x01001000)
#define NRF_UART0_BASE            (PERIPHERAL_BASE + 0x00002000)
#define NRF_GPIO_BASE             (PERIPHERAL_BASE + 0x13000000)
#define NRF_SPIM0_BASE            (PERIPHERAL_BASE + 0x00002000)
#define NRF_SPIM1_BASE            (PERIPHERAL_BASE + 0x00003000)
#define NRF_TIMER0_BASE           (PERIPHERAL_BASE + 0x00004000)
#define NRF_TIMER1_BASE           (PERIPHERAL_BASE + 0x00005000)
#define NRF_TIMER2_BASE           (PERIPHERAL_BASE + 0x00006000)
#define NRF_RTC0_BASE             (PERIPHERAL_BASE + 0x00014000)
#define NRF_RTC1_BASE             (PERIPHERAL_BASE + 0x00015000)
#define NRF_TEMP_BASE             (PERIPHERAL_BASE + 0x00011000)
#define NRF_RNG_BASE              (PERIPHERAL_BASE + 0x00012000)
#define NRF_ECB_BASE              (PERIPHERAL_BASE + 0x0000E000)
#define NRF_CCM_BASE              (PERIPHERAL_BASE + 0x0000C000)
#define NRF_WDT_BASE              (PERIPHERAL_BASE + 0x00010000)
#define NRF_NVMC_BASE             (PERIPHERAL_BASE + 0x0000E000)
#define NRF_PPI_BASE              (PERIPHERAL_BASE + 0x00020000)
#define NRF_GPIOTE_BASE           (PERIPHERAL_BASE + 0x00019000)
#define NRF_ADC_BASE              (PERIPHERAL_BASE + 0x00007000)

/* Clock Registers */
typedef struct {
    volatile uint32_t TASKS_HFCLKSTART;
    volatile uint32_t TASKS_HFCLKSTOP;
    volatile uint32_t TASKS_LFCLKSTART;
    volatile uint32_t TASKS_LFCLKSTOP;
    volatile uint32_t TASKS_CAL;
    volatile uint32_t TASKS_CTSTART;
    volatile uint32_t TASKS_CTSTOP;
    volatile uint32_t RESERVED0[57];
    volatile uint32_t EVENTS_HFCLKSTARTED;
    volatile uint32_t EVENTS_LFCLKSTARTED;
    volatile uint32_t EVENTS_DONE;
    volatile uint32_t EVENTS_CTTO;
    volatile uint32_t RESERVED1[56];
    volatile uint32_t INTENSET;
    volatile uint32_t INTENCLR;
    volatile uint32_t RESERVED2[63];
    volatile uint32_t HFCLKRUN;
    volatile uint32_t HFCLKSTAT;
    volatile uint32_t RESERVED3[62];
    volatile uint32_t LFCLKRUN;
    volatile uint32_t LFCLKSTAT;
    volatile uint32_t LFCLKSRCCOPY;
    volatile uint32_t LFCLKSRC;
    volatile uint32_t RESERVED4[61];
    volatile uint32_t CTIV;
    volatile uint32_t TRACECONFIG;
    volatile uint32_t TRACECONFIG_SET;
    volatile uint32_t TRACECONFIG_CLR;
} NRF_CLOCK_Type;

#define NRF_CLOCK                 ((NRF_CLOCK_Type *)NRF_CLOCK_BASE)

/* Power Registers */
typedef struct {
    volatile uint32_t TASKS_CONSTLAT;
    volatile uint32_t TASKS_LOWPWR;
    volatile uint32_t RESERVED0[64];
    volatile uint32_t EVENTS_POFWARN;
    volatile uint32_t RESERVED1[20];
    volatile uint32_t INTENSET;
    volatile uint32_t INTENCLR;
    volatile uint32_t RESERVED2[62];
    volatile uint32_t RESET;
    volatile uint32_t RESERVED3[29];
    volatile uint32_t DCDCEN;
    volatile uint32_t DCDCEN0;
    volatile uint32_t RESERVED4[33];
    volatile uint32_t POFCON;
    volatile uint32_t RESERVED5[2];
    volatile uint32_t USBPOWCON;
    volatile uint32_t RESERVED6;
    volatile uint32_t POWERGPO;
} NRF_POWER_Type;

#define NRF_POWER                 ((NRF_POWER_Type *)NRF_POWER_BASE)

/* System Core Clock - Updated by SystemCoreClockUpdate() */
uint32_t SystemCoreClock = 64000000UL;

/* SystemInit - System Initialization */
void SystemInit(void)
{
    /* Configure HFCLK to start using internal RC oscillator */
    /* The HFCLK will run at 64MHz after calibration */
    
    /* Start HFCLK */
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    
    /* Wait for HFCLK to start */
    while (!(NRF_CLOCK->EVENTS_HFCLKSTARTED));
    
    /* Clear the event */
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    
    /* Configure LFCLK source (default: RC oscillator) */
    /* LFCLKSRC = 0: RC oscillator */
    /* LFCLKSRC = 1: Xtal oscillator */
    NRF_CLOCK->LFCLKSRC = 0;
    
    /* Start LFCLK */
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    
    /* Wait for LFCLK to start */
    while (!(NRF_CLOCK->EVENTS_LFCLKSTARTED));
    
    /* Clear the event */
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    
    /* Enable System NVIC */
    __enable_irq();
}

/* SystemCoreClockUpdate - Update System Core Clock */
void SystemCoreClockUpdate(void)
{
    /* Read HFCLKSTAT to get current clock source */
    uint32_t hfclk_stat = NRF_CLOCK->HFCLKSTAT;
    
    /* Check if HFCLK is running */
    if (hfclk_stat & 0x01000000)
    {
        /* HFCLK is running from internal oscillator (64 MHz) */
        SystemCoreClock = 64000000UL;
    }
    else
    {
        /* Default to 64MHz */
        SystemCoreClock = 64000000UL;
    }
}
