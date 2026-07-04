#ifndef H7_PWR_H
#define H7_PWR_H

#include <stdint.h>

/* =====================================================================
 *  STM32H743ZIT6 - Bare Metal PWR (Power Control) Driver
 *  No CMSIS / HAL dependency. Direct register access only.
 *
 *  Reference: RM0433 Rev 7/8 - Chapter 6 "Power control (PWR)"
 *
 *  The PWR peripheral on H743 sits at 0x58024800 on the D3 bus.
 *
 *  Register map (offsets from PWR base):
 *      0x00  PWR_CR1    (control)
 *      0x04  PWR_CSR1   (status - read-only companion of CR1)
 *      0x08  PWR_CR2    (control)
 *      0x0C  PWR_CR3    (control)
 *      0x10  PWR_CPUCR  (CPU power / low-power mode entry)
 *      0x18  PWR_D3CR   (D3 domain / VOS scaling)
 *      0x20  PWR_WKUPCR (write-1-to-clear wake-up flags)
 *      0x24  PWR_WKUPFR (read wake-up flags)
 *      0x28  PWR_WKUPEPR(wake-up enable / polarity / pull)
 *
 *  Notes:
 *    - Run-mode VOS1..VOS3 voltage-scaling bits live in PWR_D3CR[15:14]
 *      (VOS0 is enabled through SYSCFG_PWRCR.ODEN, not a PWR bit).
 *    - PVDE / PLS / PVDO live in CR1 / CSR1 (not CR3).
 *    - Wake-up enable bits live in PWR_WKUPEPR (not WKUPMR).
 *    - There is no PWR_D1CR, PWR_D2CR, PWR_ASCR, PWR_RSR,
 *      PWR_SCCR, PWR_BDCR1 or PWR_BDCR2 register on H743.
 *      Reset-cause flags are in RCC->RSR (0x58024400 + 0xD0).
 * ===================================================================== */

/* ---------------------------------------------------------------------
 *  PWR base address and register offsets
 * ------------------------------------------------------------------- */
#define PWR_BASE_ADDR              0x58024800UL

#define PWR_CR1_OFFSET             0x00U
#define PWR_CSR1_OFFSET            0x04U
#define PWR_CR2_OFFSET             0x08U
#define PWR_CR3_OFFSET             0x0CU
#define PWR_CPUCR_OFFSET           0x10U
#define PWR_D3CR_OFFSET            0x18U
#define PWR_WKUPCR_OFFSET          0x20U
#define PWR_WKUPFR_OFFSET          0x24U
#define PWR_WKUPEPR_OFFSET         0x28U

#define PWR_REG(off)               (*(volatile uint32_t *)(PWR_BASE_ADDR + (off)))

/* ---------------------------------------------------------------------
 *  Bit manipulation macros
 * ------------------------------------------------------------------- */
#define SET_BIT(REG, BIT)          ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)        ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)         ((REG) & (BIT))

/* ---------------------------------------------------------------------
 *  PWR_CR1 bit definitions  (RM0433 §6.8.1)
 * ------------------------------------------------------------------- */
#define PWR_CR1_LPDS               (1U << 0)    /* Low-power regulator in Stop when SVOS3 is selected */
#define PWR_CR1_PVDE               (1U << 4)    /* Programmable voltage detector enable */
#define PWR_CR1_PLS_Pos            5U
#define PWR_CR1_PLS_Msk            (7U << PWR_CR1_PLS_Pos)  /* PVD level 0..7 */
#define PWR_CR1_DBP                (1U << 8)    /* Disable backup domain write protection */
#define PWR_CR1_FLPS               (1U << 9)    /* Flash low-power mode in DStop */
#define PWR_CR1_SVOS_Pos           14U
#define PWR_CR1_SVOS_Msk           (3U << PWR_CR1_SVOS_Pos)
#define PWR_CR1_SVOS_5             (1U << PWR_CR1_SVOS_Pos)
#define PWR_CR1_SVOS_4             (2U << PWR_CR1_SVOS_Pos)
#define PWR_CR1_SVOS_3             (3U << PWR_CR1_SVOS_Pos)
#define PWR_CR1_AVDEN              (1U << 16)   /* Analog voltage detector enable */
#define PWR_CR1_ALS_Pos            17U
#define PWR_CR1_ALS_Msk            (3U << PWR_CR1_ALS_Pos)   /* AVD level selection */
#define PWR_CR1_ALS_1V7            (0U << PWR_CR1_ALS_Pos)
#define PWR_CR1_ALS_2V1            (1U << PWR_CR1_ALS_Pos)
#define PWR_CR1_ALS_2V5            (2U << PWR_CR1_ALS_Pos)
#define PWR_CR1_ALS_2V8            (3U << PWR_CR1_ALS_Pos)

/* ---------------------------------------------------------------------
 *  PWR_CSR1 bit definitions  (read-only status register)
 * ------------------------------------------------------------------- */
#define PWR_CSR1_PVDO              (1U << 4)    /* PVD output */
#define PWR_CSR1_ACTVOSRDY         (1U << 13)   /* VOS change complete */
#define PWR_CSR1_ACTVOS_Pos        14U
#define PWR_CSR1_ACTVOS_Msk        (3U << PWR_CSR1_ACTVOS_Pos)
#define PWR_CSR1_AVDO              (1U << 16)   /* AVD output */

/* ---------------------------------------------------------------------
 *  PWR_CR2 bit definitions
 * ------------------------------------------------------------------- */
#define PWR_CR2_BREN               (1U << 0)    /* Backup regulator enable */
#define PWR_CR2_MONEN              (1U << 4)    /* VBAT and temperature monitoring enable */
#define PWR_CR2_BRRDY              (1U << 16)   /* Backup regulator ready */
#define PWR_CR2_VBATL              (1U << 20)   /* VBAT below low threshold */
#define PWR_CR2_VBATH              (1U << 21)   /* VBAT above high threshold */
#define PWR_CR2_TEMPL              (1U << 22)   /* Temperature below low threshold */
#define PWR_CR2_TEMPH              (1U << 23)   /* Temperature above high threshold */

/* ---------------------------------------------------------------------
 *  PWR_CR3 bit definitions
 * ------------------------------------------------------------------- */
#define PWR_CR3_BYPASS             (1U << 0)
#define PWR_CR3_LDOEN              (1U << 1)
#define PWR_CR3_SCUEN              (1U << 2)
#define PWR_CR3_VBE                (1U << 8)    /* VBAT battery charging enable */
#define PWR_CR3_VBRS               (1U << 9)
#define PWR_CR3_USB33DEN           (1U << 24)   /* VDD33USB detector enable */
#define PWR_CR3_USBREGEN           (1U << 25)   /* USB regulator enable */
#define PWR_CR3_USB33RDY           (1U << 26)   /* USB supply ready */

/* ---------------------------------------------------------------------
 *  PWR_CPUCR bit definitions  (RM0433 §6.8.5)
 * ------------------------------------------------------------------- */
#define PWR_CPUCR_PDDS_D1          (1U << 0)
#define PWR_CPUCR_PDDS_D2          (1U << 1)
#define PWR_CPUCR_PDDS_D3          (1U << 2)
#define PWR_CPUCR_PDDS             (PWR_CPUCR_PDDS_D1 | PWR_CPUCR_PDDS_D2 | PWR_CPUCR_PDDS_D3)
#define PWR_CPUCR_STOPF            (1U << 5)    /* Stop flag */
#define PWR_CPUCR_SBF              (1U << 6)    /* Standby flag */
#define PWR_CPUCR_SBF_D1           (1U << 7)
#define PWR_CPUCR_SBF_D2           (1U << 8)
#define PWR_CPUCR_CSSF             (1U << 9)    /* Clear SBF / STOPF (write 1) */
#define PWR_CPUCR_RUN_D3           (1U << 11)   /* D3 stays in Run while D1/D2 sleep */

/* ---------------------------------------------------------------------
 *  PWR_D3CR bit definitions  (RM0433 §6.8.6) - VOS is here
 * ------------------------------------------------------------------- */
#define PWR_D3CR_VOS_Pos           14U
#define PWR_D3CR_VOS_Msk           (3U << PWR_D3CR_VOS_Pos)
#define PWR_D3CR_VOS_RESERVED      (0U << PWR_D3CR_VOS_Pos)  /* Reserved; hardware selects Scale 3 */
#define PWR_D3CR_VOS_3             (1U << PWR_D3CR_VOS_Pos)  /* Scale 3 */
#define PWR_D3CR_VOS_2             (2U << PWR_D3CR_VOS_Pos)  /* Scale 2 */
#define PWR_D3CR_VOS_1             (3U << PWR_D3CR_VOS_Pos)  /* Scale 1 */
#define PWR_D3CR_VOSRDY            (1U << 13)   /* Voltage scaling ready */

/* ---------------------------------------------------------------------
 *  PWR_WKUPCR / PWR_WKUPFR / PWR_WKUPEPR
 * ------------------------------------------------------------------- */
#define PWR_WKUPCR_WKUPC1          (1U << 0)    /* write 1 to clear flag 1 */
#define PWR_WKUPCR_WKUPC2          (1U << 1)
#define PWR_WKUPCR_WKUPC3          (1U << 2)
#define PWR_WKUPCR_WKUPC4          (1U << 3)
#define PWR_WKUPCR_WKUPC5          (1U << 4)
#define PWR_WKUPCR_WKUPC6          (1U << 5)

#define PWR_WKUPFR_WKUPF1          (1U << 0)    /* read flag 1 */
#define PWR_WKUPFR_WKUPF2          (1U << 1)
#define PWR_WKUPFR_WKUPF3          (1U << 2)
#define PWR_WKUPFR_WKUPF4          (1U << 3)
#define PWR_WKUPFR_WKUPF5          (1U << 4)
#define PWR_WKUPFR_WKUPF6          (1U << 5)

#define PWR_WKUPEPR_WKUPEN1        (1U << 0)    /* Enable WKUP pin 1 */
#define PWR_WKUPEPR_WKUPEN2        (1U << 1)
#define PWR_WKUPEPR_WKUPEN3        (1U << 2)
#define PWR_WKUPEPR_WKUPEN4        (1U << 3)
#define PWR_WKUPEPR_WKUPEN5        (1U << 4)
#define PWR_WKUPEPR_WKUPEN6        (1U << 5)
#define PWR_WKUPEPR_WKUPP1         (1U << 8)    /* Polarity: 0 = rising, 1 = falling */
#define PWR_WKUPEPR_WKUPP2         (1U << 9)
#define PWR_WKUPEPR_WKUPP3         (1U << 10)
#define PWR_WKUPEPR_WKUPP4         (1U << 11)
#define PWR_WKUPEPR_WKUPP5         (1U << 12)
#define PWR_WKUPEPR_WKUPP6         (1U << 13)
#define PWR_WKUPEPR_WKUPPUPD1_Pos  16U          /* Pull-up/down for WKUP1 */
#define PWR_WKUPEPR_WKUPPUPD2_Pos  18U
#define PWR_WKUPEPR_WKUPPUPD3_Pos  20U
#define PWR_WKUPEPR_WKUPPUPD4_Pos  22U
#define PWR_WKUPEPR_WKUPPUPD5_Pos  24U
#define PWR_WKUPEPR_WKUPPUPD6_Pos  26U
#define PWR_WKUPEPR_WKUPPUPD1_Msk  (3U << PWR_WKUPEPR_WKUPPUPD1_Pos)
#define PWR_WKUPEPR_WKUPPUPD2_Msk  (3U << PWR_WKUPEPR_WKUPPUPD2_Pos)
#define PWR_WKUPEPR_WKUPPUPD3_Msk  (3U << PWR_WKUPEPR_WKUPPUPD3_Pos)
#define PWR_WKUPEPR_WKUPPUPD4_Msk  (3U << PWR_WKUPEPR_WKUPPUPD4_Pos)
#define PWR_WKUPEPR_WKUPPUPD5_Msk  (3U << PWR_WKUPEPR_WKUPPUPD5_Pos)
#define PWR_WKUPEPR_WKUPPUPD6_Msk  (3U << PWR_WKUPEPR_WKUPPUPD6_Pos)
#define PWR_WKUPEPR_WKUPPUPD_NO_PULL    0U
#define PWR_WKUPEPR_WKUPPUPD_PULL_UP    1U
#define PWR_WKUPEPR_WKUPPUPD_PULL_DOWN  2U

/* ---------------------------------------------------------------------
 *  Voltage scaling (VOS) enum
 *      VOS0 = highest performance, core up to 480 MHz (1.35 V)
 *      VOS1 = up to 400 MHz                                (1.20 V)
 *      VOS2 = up to 300 MHz                                (1.10 V)
 *      VOS3 = lowest power,    up to 200 MHz               (1.00 V)
 *  See RM0433 §6.5.1 and datasheet DS12110.
 * ------------------------------------------------------------------- */
typedef enum
{
    PWR_VOS_3 = 1,   /* Scale 3: lowest run-mode scale */
    PWR_VOS_2 = 2,   /* Scale 2 */
    PWR_VOS_1 = 3,   /* Scale 1 */
    PWR_VOS_0 = 4    /* Scale 0: VOS1 in PWR_D3CR plus SYSCFG_PWRCR.ODEN */
} PWR_VOS_t;

/* ---------------------------------------------------------------------
 *  Stop mode sub-level enum
 *      STOP_0 = full SRAM retention,   fast wake-up,  highest power
 *      STOP_1 = partial SRAM retention, normal wake-up
 *      STOP_2 = no SRAM retention,     slow wake-up,   lowest power
 * ------------------------------------------------------------------- */
typedef enum
{
    PWR_STOP_0 = 0,
    PWR_STOP_1 = 1,
    PWR_STOP_2 = 2
} PWR_StopLevel_t;

/* ---------------------------------------------------------------------
 *  Public API
 * ------------------------------------------------------------------- */

/* ---- Init / de-init ------------------------------------------------ */
void     PWR_Init          (void);
void     PWR_DeInit        (void);

/* ---- Voltage scaling ----------------------------------------------- */
void     PWR_SetVoltageScale(PWR_VOS_t scale);
PWR_VOS_t PWR_GetVoltageScale(void);
uint8_t  PWR_RegulatorReady(void);

/* ---- Low-power mode entry ------------------------------------------ */
void     PWR_EnterSleepMode      (uint8_t sleep_on_exit);
void     PWR_EnterStopMode       (PWR_StopLevel_t level, uint8_t low_power_regulator);
void     PWR_EnterStandbyMode    (uint8_t retain_d3);

/* ---- Wake-up pins (pins 0..5 == WKUP1..WKUP6) ---------------------- */
void     PWR_EnableWakeUpPin     (uint8_t pin, uint8_t polarity_falling);
void     PWR_DisableWakeUpPin    (uint8_t pin);
uint8_t  PWR_GetWakeUpFlag       (uint8_t pin);
void     PWR_ClearWakeUpFlag     (uint8_t pin);

/* ---- Backup domain access ----------------------------------------- */
void     PWR_EnableBackupAccess  (void);
void     PWR_DisableBackupAccess (void);

/* ---- Regulator control -------------------------------------------- */
void     PWR_LowPowerRegulatorEnable  (uint8_t enable);
void     PWR_BackupRegulatorEnable    (uint8_t enable);
void     PWR_OverDriveEnable          (uint8_t enable);
void     PWR_BoostEnable              (uint8_t enable);

/* ---- PVD (programmable voltage detector) -------------------------- */
void     PWR_PVDEnable  (uint8_t enable);
void     PWR_PVDSetLevel(uint8_t pls);    /* 0..7 */
uint8_t  PWR_PVDStatus  (void);

/* ---- Status / flags ------------------------------------------------ */
uint8_t  PWR_GetStandbyFlag   (void);
uint8_t  PWR_GetStopFlag      (void);
void     PWR_ClearStandbyFlag (void);
void     PWR_ClearStopFlag    (void);

#endif /* H7_PWR_H */
