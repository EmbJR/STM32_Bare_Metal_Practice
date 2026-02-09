#include "flash.h"

/**
  * @brief  Set Flash latency (wait states)
  * @param  latency: Number of wait states (0 or 1)
  * @retval None
  */
void FLASH_SetLatency(FLASH_Latency_t latency)
{
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY_Msk, (latency << FLASH_ACR_LATENCY_Pos));
}

/**
  * @brief  Enable Flash prefetch buffer
  * @param  None
  * @retval None
  */
void FLASH_EnablePrefetchBuffer(void)
{
    SET_BIT(FLASH->ACR, FLASH_ACR_PRFTBE);
}

/**
  * @brief  Disable Flash prefetch buffer
  * @param  None
  * @retval None
  */
void FLASH_DisablePrefetchBuffer(void)
{
    CLEAR_BIT(FLASH->ACR, FLASH_ACR_PRFTBE);
}

/**
  * @brief  Get current Flash operation status
  * @param  None
  * @retval FLASH_Status_t: Current Flash status
  */
FLASH_Status_t FLASH_GetStatus(void)
{
    FLASH_Status_t status = FLASH_STATUS_READY;
    
    if(READ_BIT(FLASH->SR, FLASH_SR_BSY)) {
        status = FLASH_STATUS_BUSY;
    } else if(READ_BIT(FLASH->SR, FLASH_SR_PGERR)) {
        status = FLASH_STATUS_PROGRAM_ERROR;
    } else if(READ_BIT(FLASH->SR, FLASH_SR_WRPRTERR)) {
        status = FLASH_STATUS_WRITE_PROTECT_ERROR;
    } else if(READ_BIT(FLASH->OBR, FLASH_OBR_OPTERR)) {
        status = FLASH_STATUS_OPTION_BYTE_ERROR;
    }
    
    return status;
}

/**
  * @brief  Erase a Flash page (1KB or 2KB depending on device)
  * @param  page_address: Address within the page to erase
  * @retval FLASH_Status_t: Operation status
  */
FLASH_Status_t FLASH_ErasePage(uint32_t page_address)
{
    FLASH_Status_t status = FLASH_GET_STATUS();
    
    if(status == FLASH_STATUS_READY) {
        /* Unlock Flash */
        FLASH_UNLOCK();
        
        /* Set PER bit and page address */
        SET_BIT(FLASH->CR, FLASH_CR_PER);
        FLASH->AR = page_address;
        
        /* Start erase operation */
        SET_BIT(FLASH->CR, FLASH_CR_STRT);
        
        /* Wait for completion */
        FLASH_WAIT_FOR_BUSY();
        
        /* Check for errors */
        if(READ_BIT(FLASH->SR, FLASH_SR_EOP)) {
            CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
            FLASH_CLEAR_EOP();
            status = FLASH_STATUS_READY;
        } else {
            status = FLASH_GET_STATUS();
        }
        
        /* Lock Flash */
        FLASH_LOCK();
    }
    
    return status;
}

/**
  * @brief  Program a half-word (16-bit) to Flash memory
  * @param  address: Destination address (must be half-word aligned)
  * @param  data: 16-bit data to program
  * @retval FLASH_Status_t: Operation status
  */
FLASH_Status_t FLASH_ProgramHalfWord(uint32_t address, uint16_t data)
{
    FLASH_Status_t status = FLASH_GET_STATUS();
    
    if(status == FLASH_STATUS_READY) {
        /* Unlock Flash */
        FLASH_UNLOCK();
        
        /* Set PG bit */
        SET_BIT(FLASH->CR, FLASH_CR_PG);
        
        /* Program the half-word */
        *(__IO uint16_t*)address = data;
        
        /* Wait for completion */
        FLASH_WAIT_FOR_BUSY();
        
        /* Check for errors */
        if(READ_BIT(FLASH->SR, FLASH_SR_EOP)) {
            CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
            FLASH_CLEAR_EOP();
            status = FLASH_STATUS_READY;
        } else {
            status = FLASH_GET_STATUS();
        }
        
        /* Lock Flash */
        FLASH_LOCK();
    }
    
    return status;
}

/**
  * @brief  Configure Flash for specific system clock frequency
  * @param  sysclk_frequency: System clock frequency in Hz
  * @retval None
  */
void FLASH_ConfigureForFrequency(uint32_t sysclk_frequency)
{
    /* Enable prefetch buffer for better performance */
    FLASH_EnablePrefetchBuffer();
    
    /* Set appropriate latency based on frequency */
    if(sysclk_frequency <= 24000000) {
        FLASH_SetLatency(FLASH_LATENCY_0);      /* 0 wait states */
    } else {
        FLASH_SetLatency(FLASH_LATENCY_1);      /* 1 wait state */
    }
}