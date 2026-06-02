/**
 * @file    FlashF051.c
 * @brief   Flash memory driver implementation for STM32F051R8T6
 * @details Low-level Flash memory programming, erasing, and configuration
 */

#include "FlashF051.h"
#include <stddef.h>
/* ============================================================================
   Private Macros and Timeout Configuration
   ========================================================================== */
#define FLASH_OPERATION_TIMEOUT    1000000U    /* Loop timeout for busy wait */

/* ============================================================================
   Private Helper Functions
   ========================================================================== */

/**
 * @brief  Wait for Flash to complete current operation
 * @param  None
 * @retval Flash_Status_t: Status code
 */
static Flash_Status_t FLASH_WaitBusyComplete(void)
{
    uint32_t timeout = FLASH_OPERATION_TIMEOUT;
    
    while ((FLASH_SR & FLASH_SR_BSY) && timeout) {
        timeout--;
    }
    
    if (timeout == 0) {
        return FLASH_STATUS_TIMEOUT;
    }
    
    return FLASH_GetStatus();
}

/**
 * @brief  Get error status from Flash SR register
 * @param  None
 * @retval Flash_Status_t: Status code
 */
static Flash_Status_t FLASH_GetErrorStatus(void)
{
    uint32_t sr = FLASH_SR;
    
    if (sr & FLASH_SR_PGERR) {
        return FLASH_STATUS_PGERR;
    }
    
    if (sr & FLASH_SR_WRPRTERR) {
        return FLASH_STATUS_WRPRTERR;
    }
    
    if (FLASH_OBR & FLASH_OBR_OPTERR) {
        return FLASH_STATUS_OPTERR;
    }
    
    return FLASH_STATUS_OK;
}

/**
 * @brief  Check if Flash is locked
 * @param  None
 * @retval true if locked, false if unlocked
 */
static bool FLASH_IsLocked(void)
{
    return (FLASH_CR & FLASH_CR_LOCK) ? true : false;
}

/* ============================================================================
   Public Function Implementations - Configuration
   ========================================================================== */

void FLASH_Init(void)
{
    /* Clear any existing errors */
    FLASH_ClearErrors();
}

void FLASH_SetLatency(Flash_Latency_t latency)
{
    uint32_t acr;
    
    if (latency > FLASH_LATENCY_1) {
        return;
    }
    
    /* Read current ACR value */
    acr = FLASH_ACR;
    
    /* Clear latency bits */
    acr &= ~FLASH_ACR_LATENCY_Msk;
    
    /* Set new latency */
    acr |= (latency << FLASH_ACR_LATENCY_Pos);
    
    /* Write back to ACR */
    FLASH_ACR = acr;
}

Flash_Latency_t FLASH_GetLatency(void)
{
    return (Flash_Latency_t)((FLASH_ACR & FLASH_ACR_LATENCY_Msk) >> FLASH_ACR_LATENCY_Pos);
}

void FLASH_EnablePrefetch(void)
{
    FLASH_ACR |= FLASH_ACR_PRFTBE;
}

void FLASH_DisablePrefetch(void)
{
    FLASH_ACR &= ~FLASH_ACR_PRFTBE;
}

bool FLASH_IsPrefetchReady(void)
{
    return (FLASH_ACR & FLASH_ACR_PRFTBS) ? true : false;
}

/* ============================================================================
   Public Function Implementations - Status and Control
   ========================================================================== */

Flash_Status_t FLASH_GetStatus(void)
{
    if (FLASH_SR & FLASH_SR_BSY) {
        return FLASH_STATUS_BUSY;
    }
    
    return FLASH_GetErrorStatus();
}

bool FLASH_IsBusy(void)
{
    return (FLASH_SR & FLASH_SR_BSY) ? true : false;
}

Flash_Status_t FLASH_WaitForComplete(uint32_t timeout_ms)
{
    uint32_t timeout = timeout_ms * 1000U;
    
    while ((FLASH_SR & FLASH_SR_BSY) && timeout) {
        timeout--;
    }
    
    if (timeout == 0) {
        return FLASH_STATUS_TIMEOUT;
    }
    
    return FLASH_GetStatus();
}

void FLASH_ClearErrors(void)
{
    /* Clear PGERR and WRPRTERR flags */
    FLASH_SR |= (FLASH_SR_PGERR | FLASH_SR_WRPRTERR);
}

void FLASH_ClearEOP(void)
{
    /* Clear EOP flag */
    FLASH_SR |= FLASH_SR_EOP;
}

Flash_Status_t FLASH_Unlock(void)
{
    if (!FLASH_IsLocked()) {
        return FLASH_STATUS_OK;
    }
    
    /* Write unlock keys in sequence */
    FLASH_KEYR = FLASH_UNLOCK_KEY1;
    FLASH_KEYR = FLASH_UNLOCK_KEY2;
    
    /* Verify unlock was successful */
    if (FLASH_IsLocked()) {
        return FLASH_STATUS_TIMEOUT;
    }
    
    return FLASH_STATUS_OK;
}

void FLASH_Lock(void)
{
    FLASH_CR |= FLASH_CR_LOCK;
}

/* ============================================================================
   Public Function Implementations - Erase Operations
   ========================================================================== */

Flash_Status_t FLASH_ErasePage(uint32_t page_addr)
{
    Flash_Status_t status;
    
    /* Validate address */
    if (!FLASH_IsValidAddress(page_addr)) {
        return FLASH_STATUS_ADDRESS_INVALID;
    }
    
    /* Align to page boundary */
    page_addr &= ~(FLASH_PAGE_SIZE - 1);
    
    /* Wait for any ongoing operation */
    status = FLASH_WaitBusyComplete();
    if (status != FLASH_STATUS_OK) {
        return status;
    }
    
    /* Clear any previous errors */
    FLASH_ClearErrors();
    
    /* Unlock Flash */
    status = FLASH_Unlock();
    if (status != FLASH_STATUS_OK) {
        return status;
    }
    
    /* Set Page Erase bit */
    FLASH_CR |= FLASH_CR_PER;
    
    /* Set page address */
    FLASH_AR = page_addr;
    
    /* Start erase operation */
    FLASH_CR |= FLASH_CR_STRT;
    
    /* Wait for completion */
    status = FLASH_WaitBusyComplete();
    
    /* Clear Page Erase bit */
    FLASH_CR &= ~FLASH_CR_PER;
    
    /* Clear EOP flag if operation succeeded */
    if (status == FLASH_STATUS_OK) {
        FLASH_ClearEOP();
    }
    
    /* Lock Flash */
    FLASH_Lock();
    
    return status;
}

Flash_Status_t FLASH_ErasePages(uint32_t start_addr, uint16_t page_count)
{
    Flash_Status_t status;
    uint32_t page_addr;
    uint16_t i;
    
    /* Validate start address */
    if (!FLASH_IsValidAddress(start_addr)) {
        return FLASH_STATUS_ADDRESS_INVALID;
    }
    
    /* Align to page boundary */
    page_addr = start_addr & ~(FLASH_PAGE_SIZE - 1);
    
    /* Erase each page */
    for (i = 0; i < page_count; i++) {
        status = FLASH_ErasePage(page_addr);
        if (status != FLASH_STATUS_OK) {
            return status;
        }
        page_addr += FLASH_PAGE_SIZE;
        
        /* Check for overflow */
        if (page_addr > FLASH_APP_END_ADDR) {
            return FLASH_STATUS_ADDRESS_INVALID;
        }
    }
    
    return FLASH_STATUS_OK;
}

Flash_Status_t FLASH_MassErase(void)
{
    Flash_Status_t status;
    
    /* Wait for any ongoing operation */
    status = FLASH_WaitBusyComplete();
    if (status != FLASH_STATUS_OK) {
        return status;
    }
    
    /* Clear any previous errors */
    FLASH_ClearErrors();
    
    /* Unlock Flash */
    status = FLASH_Unlock();
    if (status != FLASH_STATUS_OK) {
        return status;
    }
    
    /* Set Mass Erase bit */
    FLASH_CR |= FLASH_CR_MER;
    
    /* Start erase operation */
    FLASH_CR |= FLASH_CR_STRT;
    
    /* Wait for completion */
    status = FLASH_WaitBusyComplete();
    
    /* Clear Mass Erase bit */
    FLASH_CR &= ~FLASH_CR_MER;
    
    /* Clear EOP flag if operation succeeded */
    if (status == FLASH_STATUS_OK) {
        FLASH_ClearEOP();
    }
    
    /* Lock Flash */
    FLASH_Lock();
    
    return status;
}

/* ============================================================================
   Public Function Implementations - Program Operations
   ========================================================================== */

Flash_Status_t FLASH_ProgramHalfWord(uint32_t addr, uint16_t data)
{
    Flash_Status_t status;
    volatile uint16_t *flash_ptr;
    
    /* Validate address - must be half-word aligned */
    if (!FLASH_IsValidAddress(addr) || (addr & 0x01)) {
        return FLASH_STATUS_ADDRESS_INVALID;
    }
    
    /* Wait for any ongoing operation */
    status = FLASH_WaitBusyComplete();
    if (status != FLASH_STATUS_OK) {
        return status;
    }
    
    /* Clear any previous errors */
    FLASH_ClearErrors();
    
    /* Unlock Flash */
    status = FLASH_Unlock();
    if (status != FLASH_STATUS_OK) {
        return status;
    }
    
    /* Set Programming bit */
    FLASH_CR |= FLASH_CR_PG;
    
    /* Program half-word */
    flash_ptr = (volatile uint16_t *)addr;
    *flash_ptr = data;
    
    /* Wait for completion */
    status = FLASH_WaitBusyComplete();
    
    /* Clear Programming bit */
    FLASH_CR &= ~FLASH_CR_PG;
    
    /* Clear EOP flag if operation succeeded */
    if (status == FLASH_STATUS_OK) {
        FLASH_ClearEOP();
    }
    
    /* Lock Flash */
    FLASH_Lock();
    
    return status;
}

Flash_Status_t FLASH_ProgramWord(uint32_t addr, uint32_t data)
{
    Flash_Status_t status;
    
    /* Validate address - must be word aligned */
    if (!FLASH_IsValidAddress(addr) || (addr & 0x03)) {
        return FLASH_STATUS_ADDRESS_INVALID;
    }
    
    /* Program low half-word */
    status = FLASH_ProgramHalfWord(addr, (uint16_t)(data & 0xFFFF));
    if (status != FLASH_STATUS_OK) {
        return status;
    }
    
    /* Program high half-word */
    status = FLASH_ProgramHalfWord(addr + 2, (uint16_t)((data >> 16) & 0xFFFF));
    
    return status;
}

Flash_Status_t FLASH_ProgramBuffer(uint32_t dest_addr, const uint8_t *src_data, uint16_t length)
{
    Flash_Status_t status;
    uint16_t i;
    uint16_t half_word;
    volatile uint16_t *flash_ptr;
    
    /* Validate parameters */
    if (src_data == NULL) {
        return FLASH_STATUS_SIZE_INVALID;
    }
    
    if (length == 0 || (length & 0x01)) {
        return FLASH_STATUS_SIZE_INVALID;
    }
    
    if (!FLASH_IsValidAddress(dest_addr) || (dest_addr & 0x01)) {
        return FLASH_STATUS_ADDRESS_INVALID;
    }
    
    if (!FLASH_IsValidAddress(dest_addr + length - 1)) {
        return FLASH_STATUS_ADDRESS_INVALID;
    }
    
    /* Wait for any ongoing operation */
    status = FLASH_WaitBusyComplete();
    if (status != FLASH_STATUS_OK) {
        return status;
    }
    
    /* Clear any previous errors */
    FLASH_ClearErrors();
    
    /* Unlock Flash */
    status = FLASH_Unlock();
    if (status != FLASH_STATUS_OK) {
        return status;
    }
    
    /* Set Programming bit */
    FLASH_CR |= FLASH_CR_PG;
    
    /* Program buffer data half-word by half-word */
    flash_ptr = (volatile uint16_t *)dest_addr;
    for (i = 0; i < length; i += 2) {
        /* Form half-word from two bytes (little-endian) */
        half_word = ((uint16_t)src_data[i + 1] << 8) | (uint16_t)src_data[i];
        
        /* Write half-word */
        *flash_ptr = half_word;
        flash_ptr++;
        
        /* Wait for completion */
        status = FLASH_WaitBusyComplete();
        if (status != FLASH_STATUS_OK) {
            break;
        }
    }
    
    /* Clear Programming bit */
    FLASH_CR &= ~FLASH_CR_PG;
    
    /* Clear EOP flag if operation succeeded */
    if (status == FLASH_STATUS_OK) {
        FLASH_ClearEOP();
    }
    
    /* Lock Flash */
    FLASH_Lock();
    
    return status;
}

/* ============================================================================
   Public Function Implementations - Utility
   ========================================================================== */

bool FLASH_VerifyHalfWord(uint32_t addr, uint16_t expected_value)
{
    volatile uint16_t *flash_ptr = (volatile uint16_t *)addr;
    return (*flash_ptr == expected_value);
}

bool FLASH_VerifyBuffer(uint32_t dest_addr, const uint8_t *src_data, uint16_t length)
{
    uint16_t i;
    volatile uint8_t *flash_ptr = (volatile uint8_t *)dest_addr;
    
    if (src_data == NULL || length == 0) {
        return false;
    }
    
    for (i = 0; i < length; i++) {
        if (flash_ptr[i] != src_data[i]) {
            return false;
        }
    }
    
    return true;
}

bool FLASH_IsValidAddress(uint32_t addr)
{
    return (addr >= FLASH_APP_START_ADDR) && (addr <= FLASH_APP_END_ADDR);
}

int16_t FLASH_GetPageNumber(uint32_t addr)
{
    if (!FLASH_IsValidAddress(addr)) {
        return -1;
    }
    
    return (int16_t)((addr - FLASH_APP_START_ADDR) / FLASH_PAGE_SIZE);
}

uint32_t FLASH_GetPageAddress(uint8_t page_num)
{
    if (page_num >= FLASH_PAGES_COUNT) {
        return 0;
    }
    
    return FLASH_APP_START_ADDR + (page_num * FLASH_PAGE_SIZE);
}

Flash_Status_t FLASH_ConfigureForFrequency(uint32_t sysclk_freq_hz)
{
    /* Enable prefetch buffer for better performance */
    FLASH_EnablePrefetch();
    
    /* Set latency based on frequency */
    if (sysclk_freq_hz <= 24000000) {
        FLASH_SetLatency(FLASH_LATENCY_0);
    } else if (sysclk_freq_hz <= 48000000) {
        FLASH_SetLatency(FLASH_LATENCY_1);
    } else {
        return FLASH_STATUS_TIMEOUT;
    }
    
    return FLASH_STATUS_OK;
}
