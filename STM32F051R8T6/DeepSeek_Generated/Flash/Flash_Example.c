#include "flash.h"
#include "rcc.h"

int main(void)
{
    /* Configure system clock to 32MHz */
    SystemClock_Config_HSI_32MHz();
    
    /* Configure Flash latency for 32MHz (1 wait state) */
    FLASH_ConfigureForFrequency(32000000);
    
    /* Example: Write data to Flash */
    uint32_t flash_address = 0x08008000;  /* Some address in Flash */
    uint16_t data_to_write = 0x1234;
    
    /* First erase the page */
    if(FLASH_ErasePage(flash_address) == FLASH_STATUS_READY) {
        /* Then program data */
        FLASH_ProgramHalfWord(flash_address, data_to_write);
    }
    
    /* Read back and verify */
    uint16_t read_data = *(__IO uint16_t*)flash_address;
    
    /* Configure option bytes */
    FLASH_OPT_UNLOCK();
    SET_BIT(FLASH->CR, FLASH_CR_OPTWRE);  /* Enable option byte write */
    
    /* Set hardware watchdog */
    MODIFY_REG(FLASH->OBR, FLASH_OBR_WDG_SW_Msk, FLASH_OBR_WDG_SW);
    
    return 0;
}