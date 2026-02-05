#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>

/* CMSIS-style memory access macros */
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read/write' permissions */

/* Flash Base Address */
#define FLASH_BASE        0x40022000UL

/* Flash Register Structure Definition */
typedef struct {
    __IO uint32_t ACR;      /*!< Flash access control register,      Address offset: 0x00 */
    __IO uint32_t KEYR;     /*!< Flash key register,                 Address offset: 0x04 */
    __IO uint32_t OPTKEYR;  /*!< Flash option key register,          Address offset: 0x08 */
    __IO uint32_t SR;       /*!< Flash status register,              Address offset: 0x0C */
    __IO uint32_t CR;       /*!< Flash control register,             Address offset: 0x10 */
    __IO uint32_t AR;       /*!< Flash address register,             Address offset: 0x14 */
    __I  uint32_t RESERVED; /*!< Reserved,                           Address offset: 0x18 */
    __IO uint32_t OBR;      /*!< Flash option byte register,         Address offset: 0x1C */
    __IO uint32_t WRPR;     /*!< Flash write protection register,    Address offset: 0x20 */
} FLASH_TypeDef;

/* Flash Peripheral Declaration */
#define FLASH             ((FLASH_TypeDef *) FLASH_BASE)

/* ============================ FLASH_ACR Register Bits ============================ */
/* Flash Access Control Register */
/* Latency configuration */
#define FLASH_ACR_LATENCY_Pos           (0U)                                 
#define FLASH_ACR_LATENCY_Msk           (0x7UL << FLASH_ACR_LATENCY_Pos)     
#define FLASH_ACR_LATENCY_0WS           (0x0UL << FLASH_ACR_LATENCY_Pos)     /*!< Zero wait state */
#define FLASH_ACR_LATENCY_1WS           (0x1UL << FLASH_ACR_LATENCY_Pos)     /*!< One wait state */

/* Prefetch buffer enable */
#define FLASH_ACR_PRFTBE_Pos            (4U)                                 
#define FLASH_ACR_PRFTBE_Msk            (0x1UL << FLASH_ACR_PRFTBE_Pos)      
#define FLASH_ACR_PRFTBE                FLASH_ACR_PRFTBE_Msk                /*!< Prefetch buffer enable */

/* Prefetch buffer status */
#define FLASH_ACR_PRFTBS_Pos            (5U)                                 
#define FLASH_ACR_PRFTBS_Msk            (0x1UL << FLASH_ACR_PRFTBS_Pos)      
#define FLASH_ACR_PRFTBS                FLASH_ACR_PRFTBS_Msk                /*!< Prefetch buffer status */

/* ============================ FLASH_SR Register Bits ============================ */
/* Flash Status Register */
/* Busy flag */
#define FLASH_SR_BSY_Pos                (0U)                                 
#define FLASH_SR_BSY_Msk                (0x1UL << FLASH_SR_BSY_Pos)          
#define FLASH_SR_BSY                    FLASH_SR_BSY_Msk                    /*!< Flash busy flag */

/* Programming error */
#define FLASH_SR_PGERR_Pos              (2U)                                 
#define FLASH_SR_PGERR_Msk              (0x1UL << FLASH_SR_PGERR_Pos)        
#define FLASH_SR_PGERR                  FLASH_SR_PGERR_Msk                  /*!< Programming error */

/* Write protection error */
#define FLASH_SR_WRPRTERR_Pos           (4U)                                 
#define FLASH_SR_WRPRTERR_Msk           (0x1UL << FLASH_SR_WRPRTERR_Pos)     
#define FLASH_SR_WRPRTERR               FLASH_SR_WRPRTERR_Msk               /*!< Write protection error */

/* End of operation */
#define FLASH_SR_EOP_Pos                (5U)                                 
#define FLASH_SR_EOP_Msk                (0x1UL << FLASH_SR_EOP_Pos)          
#define FLASH_SR_EOP                    FLASH_SR_EOP_Msk                    /*!< End of operation */

/* ============================ FLASH_CR Register Bits ============================ */
/* Flash Control Register */
/* Programming */
#define FLASH_CR_PG_Pos                 (0U)                                 
#define FLASH_CR_PG_Msk                 (0x1UL << FLASH_CR_PG_Pos)           
#define FLASH_CR_PG                     FLASH_CR_PG_Msk                     /*!< Flash programming */

/* Page erase */
#define FLASH_CR_PER_Pos                (1U)                                 
#define FLASH_CR_PER_Msk                (0x1UL << FLASH_CR_PER_Pos)          
#define FLASH_CR_PER                    FLASH_CR_PER_Msk                    /*!< Page erase */

/* Mass erase */
#define FLASH_CR_MER_Pos                (2U)                                 
#define FLASH_CR_MER_Msk                (0x1UL << FLASH_CR_MER_Pos)          
#define FLASH_CR_MER                    FLASH_CR_MER_Msk                    /*!< Mass erase */

/* Option byte programming */
#define FLASH_CR_OPTPG_Pos              (4U)                                 
#define FLASH_CR_OPTPG_Msk              (0x1UL << FLASH_CR_OPTPG_Pos)        
#define FLASH_CR_OPTPG                  FLASH_CR_OPTPG_Msk                  /*!< Option byte programming */

/* Option byte erase */
#define FLASH_CR_OPTER_Pos              (5U)                                 
#define FLASH_CR_OPTER_Msk              (0x1UL << FLASH_CR_OPTER_Pos)        
#define FLASH_CR_OPTER                  FLASH_CR_OPTER_Msk                  /*!< Option byte erase */

/* Start */
#define FLASH_CR_STRT_Pos               (6U)                                 
#define FLASH_CR_STRT_Msk               (0x1UL << FLASH_CR_STRT_Pos)         
#define FLASH_CR_STRT                   FLASH_CR_STRT_Msk                   /*!< Start */

/* Lock */
#define FLASH_CR_LOCK_Pos               (7U)                                 
#define FLASH_CR_LOCK_Msk               (0x1UL << FLASH_CR_LOCK_Pos)         
#define FLASH_CR_LOCK                   FLASH_CR_LOCK_Msk                   /*!< Lock */

/* Option bytes write enable */
#define FLASH_CR_OPTWRE_Pos             (9U)                                 
#define FLASH_CR_OPTWRE_Msk             (0x1UL << FLASH_CR_OPTWRE_Pos)       
#define FLASH_CR_OPTWRE                 FLASH_CR_OPTWRE_Msk                 /*!< Option bytes write enable */

/* Error interrupt enable */
#define FLASH_CR_ERRIE_Pos              (10U)                                
#define FLASH_CR_ERRIE_Msk              (0x1UL << FLASH_CR_ERRIE_Pos)        
#define FLASH_CR_ERRIE                  FLASH_CR_ERRIE_Msk                  /*!< Error interrupt enable */

/* End of operation interrupt enable */
#define FLASH_CR_EOPIE_Pos              (12U)                                
#define FLASH_CR_EOPIE_Msk              (0x1UL << FLASH_CR_EOPIE_Pos)        
#define FLASH_CR_EOPIE                  FLASH_CR_EOPIE_Msk                  /*!< End of operation interrupt enable */

/* Force option byte loading */
#define FLASH_CR_OBL_LAUNCH_Pos         (13U)                                
#define FLASH_CR_OBL_LAUNCH_Msk         (0x1UL << FLASH_CR_OBL_LAUNCH_Pos)   
#define FLASH_CR_OBL_LAUNCH             FLASH_CR_OBL_LAUNCH_Msk             /*!< Force option byte loading */

/* ============================ FLASH_OBR Register Bits ============================ */
/* Flash Option Byte Register */
/* Read protection */
#define FLASH_OBR_RDPRT_Pos             (0U)                                 
#define FLASH_OBR_RDPRT_Msk             (0x3UL << FLASH_OBR_RDPRT_Pos)       
#define FLASH_OBR_RDPRT_LEVEL0          (0x0UL << FLASH_OBR_RDPRT_Pos)       /*!< Read protection level 0 */
#define FLASH_OBR_RDPRT_LEVEL1          (0x1UL << FLASH_OBR_RDPRT_Pos)       /*!< Read protection level 1 */
#define FLASH_OBR_RDPRT_LEVEL2          (0x2UL << FLASH_OBR_RDPRT_Pos)       /*!< Read protection level 2 */

/* Option byte error */
#define FLASH_OBR_OPTERR_Pos            (10U)                                
#define FLASH_OBR_OPTERR_Msk            (0x1UL << FLASH_OBR_OPTERR_Pos)      
#define FLASH_OBR_OPTERR                FLASH_OBR_OPTERR_Msk                /*!< Option byte error */

/* WDG_SW: Watchdog selection */
#define FLASH_OBR_WDG_SW_Pos            (8U)                                 
#define FLASH_OBR_WDG_SW_Msk            (0x1UL << FLASH_OBR_WDG_SW_Pos)      
#define FLASH_OBR_WDG_SW                FLASH_OBR_WDG_SW_Msk                /*!< Watchdog software/hardware */

/* nRST_STOP: Reset when entering Stop mode */
#define FLASH_OBR_nRST_STOP_Pos         (9U)                                 
#define FLASH_OBR_nRST_STOP_Msk         (0x1UL << FLASH_OBR_nRST_STOP_Pos)   
#define FLASH_OBR_nRST_STOP             FLASH_OBR_nRST_STOP_Msk             /*!< Reset in Stop mode */

/* nRST_STDBY: Reset when entering Standby mode */
#define FLASH_OBR_nRST_STDBY_Pos        (10U)                                
#define FLASH_OBR_nRST_STDBY_Msk        (0x1UL << FLASH_OBR_nRST_STDBY_Pos)  
#define FLASH_OBR_nRST_STDBY            FLASH_OBR_nRST_STDBY_Msk            /*!< Reset in Standby mode */

/* nBOOT0: Boot configuration */
#define FLASH_OBR_nBOOT0_Pos            (11U)                                
#define FLASH_OBR_nBOOT0_Msk            (0x1UL << FLASH_OBR_nBOOT0_Pos)      
#define FLASH_OBR_nBOOT0                FLASH_OBR_nBOOT0_Msk                /*!< Boot configuration bit */

/* nBOOT1: Boot configuration */
#define FLASH_OBR_nBOOT1_Pos            (12U)                                
#define FLASH_OBR_nBOOT1_Msk            (0x1UL << FLASH_OBR_nBOOT1_Pos)      
#define FLASH_OBR_nBOOT1                FLASH_OBR_nBOOT1_Msk                /*!< Boot configuration bit */

/* VDDA_MONITOR: VDDA power supply supervisor */
#define FLASH_OBR_VDDA_MONITOR_Pos      (13U)                                
#define FLASH_OBR_VDDA_MONITOR_Msk      (0x1UL << FLASH_OBR_VDDA_MONITOR_Pos)
#define FLASH_OBR_VDDA_MONITOR          FLASH_OBR_VDDA_MONITOR_Msk          /*!< VDDA monitor enable */

/* RAM_PARITY_CHECK: RAM parity check enable */
#define FLASH_OBR_RAM_PARITY_CHECK_Pos  (14U)                                
#define FLASH_OBR_RAM_PARITY_CHECK_Msk  (0x1UL << FLASH_OBR_RAM_PARITY_CHECK_Pos)
#define FLASH_OBR_RAM_PARITY_CHECK      FLASH_OBR_RAM_PARITY_CHECK_Msk      /*!< RAM parity check enable */

/* BOOT_SEL: Boot selection */
#define FLASH_OBR_BOOT_SEL_Pos          (15U)                                
#define FLASH_OBR_BOOT_SEL_Msk          (0x1UL << FLASH_OBR_BOOT_SEL_Pos)    
#define FLASH_OBR_BOOT_SEL              FLASH_OBR_BOOT_SEL_Msk              /*!< Boot selection */

/* Data byte 0 */
#define FLASH_OBR_DATA0_Pos             (16U)                                
#define FLASH_OBR_DATA0_Msk             (0xFFUL << FLASH_OBR_DATA0_Pos)      
#define FLASH_OBR_DATA0                 FLASH_OBR_DATA0_Msk                 /*!< User data byte 0 */

/* Data byte 1 */
#define FLASH_OBR_DATA1_Pos             (24U)                                
#define FLASH_OBR_DATA1_Msk             (0xFFUL << FLASH_OBR_DATA1_Pos)      
#define FLASH_OBR_DATA1                 FLASH_OBR_DATA1_Msk                 /*!< User data byte 1 */

/* ============================ Flash Keys ============================ */
/* Flash unlock keys */
#define FLASH_KEY1                      0x45670123UL
#define FLASH_KEY2                      0xCDEF89ABUL

/* Option byte unlock keys */
#define FLASH_OPTKEY1                   0x08192A3BUL
#define FLASH_OPTKEY2                   0x4C5D6E7FUL

/* ============================ Flash Constants ============================ */
/* Flash memory sizes */
#define FLASH_PAGE_SIZE_1KB             1024UL      /* 1 KB page size for F03x/F04x/F05x */
#define FLASH_PAGE_SIZE_2KB             2048UL      /* 2 KB page size for F07x/F09x */

/* Flash memory addresses */
#define FLASH_BASE_ADDRESS              0x08000000UL
#define SYSTEM_MEMORY_BASE              0x1FFFEC00UL  /* Bootloader address */

/* Read protection levels */
typedef enum {
    FLASH_RDP_LEVEL_0 = 0xAA,     /* No protection */
    FLASH_RDP_LEVEL_1 = 0x00,     /* Read protection (default when erased) */
    FLASH_RDP_LEVEL_2 = 0xCC      /* No debug/chip read protection */
} FLASH_RDP_Level_t;

/* Flash latency wait states */
typedef enum {
    FLASH_LATENCY_0 = 0,          /* 0 wait states for SYSCLK ≤ 24 MHz */
    FLASH_LATENCY_1 = 1           /* 1 wait state for 24 MHz < SYSCLK ≤ 48 MHz */
} FLASH_Latency_t;

/* Flash operation status */
typedef enum {
    FLASH_STATUS_BUSY = 0,
    FLASH_STATUS_READY,
    FLASH_STATUS_PROGRAM_ERROR,
    FLASH_STATUS_WRITE_PROTECT_ERROR,
    FLASH_STATUS_OPTION_BYTE_ERROR
} FLASH_Status_t;

/* ============================ Helper Macros ============================ */
/* Bit manipulation macros */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  \
    ((REG) = (((REG) & (~(CLEARMASK))) | (SETMASK)))

/* Flash operation macros */
#define FLASH_WAIT_FOR_BUSY() while(FLASH->SR & FLASH_SR_BSY)
#define FLASH_CLEAR_EOP()     FLASH->SR = FLASH_SR_EOP
#define FLASH_CLEAR_ERRORS()  FLASH->SR = (FLASH_SR_PGERR | FLASH_SR_WRPRTERR)

/* Flash unlock sequence */
#define FLASH_UNLOCK() do { \
    FLASH->KEYR = FLASH_KEY1; \
    FLASH->KEYR = FLASH_KEY2; \
} while(0)

/* Flash lock */
#define FLASH_LOCK()    SET_BIT(FLASH->CR, FLASH_CR_LOCK)

/* Option byte unlock sequence */
#define FLASH_OPT_UNLOCK() do { \
    FLASH->OPTKEYR = FLASH_OPTKEY1; \
    FLASH->OPTKEYR = FLASH_OPTKEY2; \
} while(0)

/* ============================ Function Prototypes ============================ */
/* Core Flash functions */
void FLASH_SetLatency(FLASH_Latency_t latency);
void FLASH_EnablePrefetchBuffer(void);
void FLASH_DisablePrefetchBuffer(void);
FLASH_Status_t FLASH_GetStatus(void);

/* Flash memory operations */
FLASH_Status_t FLASH_ErasePage(uint32_t page_address);
FLASH_Status_t FLASH_MassErase(void);
FLASH_Status_t FLASH_ProgramHalfWord(uint32_t address, uint16_t data);
FLASH_Status_t FLASH_ProgramWord(uint32_t address, uint32_t data);

/* Option byte operations */
FLASH_Status_t FLASH_ProgramOptionByte(uint32_t address, uint16_t data);
FLASH_Status_t FLASH_EraseOptionBytes(void);
void FLASH_LaunchOptionByteLoading(void);

/* Read protection functions */
FLASH_Status_t FLASH_EnableReadProtection(FLASH_RDP_Level_t level);
FLASH_Status_t FLASH_DisableReadProtection(void);
uint8_t FLASH_GetReadProtectionLevel(void);

/* Write protection functions */
void FLASH_EnableWriteProtection(uint32_t sectors);
void FLASH_DisableWriteProtection(void);
uint32_t FLASH_GetWriteProtection(void);

#endif /* FLASH_H */