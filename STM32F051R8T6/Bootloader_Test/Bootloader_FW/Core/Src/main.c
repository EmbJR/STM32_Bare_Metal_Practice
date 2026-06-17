#include "rcc.h"
#include "uartF051.h"
#include "CirBuffer.h"
#include "uart_API.h"
#include "fuota_packat_Process.h"
#include "FlashF051.h"
#include "boot_jump.h"

#define INFO_BLOCK_ADD	0x8002400u
#define APPLICATION_START_ADDR  0x08002800U // Example address for F0

// Example configuration for 48MHz system clock using PLL from HSE
void SystemClock_Config_48MHz(void) {
    RCC_Config config = {
        .system_clock_source = CLOCK_SOURCE_PLL,
        .target_frequency = SYSTEM_CLOCK_48MHZ,
        .hse_enabled = true,
        .pll_enabled = true,
        .pll_source = PLL_SOURCE_HSE,
        .pll_multiplier = 6,  // HSE 8MHz * 6 = 48MHz
        .ahb_prescaler = AHB_PRESCALER_1,
        .apb_prescaler = APB_PRESCALER_1,
        .hsi48_enabled = false,
        .css_enabled = true
    };

    RCC_Init(&config);
}
#if 1
Flash_Status_t Example_SafeProgramWithRetry(uint32_t flash_addr,
                                           const uint8_t *data,
                                           uint16_t length,
                                           uint8_t max_retries)
{
    Flash_Status_t status;
    uint8_t retry_count = 0;

    while (retry_count < max_retries) {
        status = FLASH_ProgramBuffer(flash_addr, data, length);

        if (status == FLASH_STATUS_OK) {
            /* Verify the data */
            if (FLASH_VerifyBuffer(flash_addr, data, length)) {
                return FLASH_STATUS_OK;
            }
            /* Verification failed, retry */
            retry_count++;
        } else {
            /* Clear errors and retry */
            FLASH_ClearErrors();
            retry_count++;
        }
    }

    return FLASH_STATUS_PGERR;
}

fw_err use_Flash_Write(const uint8_t *data, uint16_t size)
{
    uint32_t current_addr = fw_info.flash_start_addr;

    /* Automatically erase the page if we are starting at a 1KB boundary */
    /* This allows sequential chunks to be written without manual erase commands */
    if ((current_addr % FLASH_PAGE_SIZE) == 0) {
        FLASH_ErasePage(current_addr);
    }

	Flash_Status_t status = Example_SafeProgramWithRetry(current_addr, data, size, 4);
	if(FLASH_STATUS_OK == status)
	{
		return SUC;
	}
	return FOTA_ERROR_FLASH_WR;
}

void user_fuota_reply(uint8_t *data, uint16_t size)
{
	for(uint16_t i = 0; i < size; i++)
	{
		UART_SendDataIT(USART1, data[i]);
	}
}


fw_err user_fuota_get_info(fw_up_str *fw_info)
{
    /* Fix: sizeof(fw_info) was returning 4 because it is a pointer. 
       Use the struct type to copy the entire block. */
	memcpy((uint8_t*)fw_info, (const uint8_t *)INFO_BLOCK_ADD, sizeof(fw_up_str));

	return SUC;
}

fw_err user_fuota_set_info(fw_up_str *fw_info)
{
    /* Fix: FLASH_ErasePage takes a memory address, not a page index number. */
	FLASH_ErasePage(INFO_BLOCK_ADD);

    Flash_Status_t status = Example_SafeProgramWithRetry(INFO_BLOCK_ADD, (const uint8_t *)fw_info, sizeof(fw_up_str), 4);
	if(FLASH_STATUS_OK == status)
	{
		return SUC;
	}
	return FOTA_ERROR_FLASH_WR;
}



#endif
int main(void) {
	fw_err stt = SUC;
	volatile uint32_t repeat = 0;
    // Configure system clock
	SystemClock_Config_48MHz();

    // Get current system clock frequency
    uint32_t sysclk = RCC_GetSystemClockFrequency();

    Bt_Uart_Init();
    fuota_init();

    user_fuota_get_info(&fw_info);

    while (1) {
    	//UART_SendStringIT(USART1, "Hello\n");
    	stt = fuota_process_Data();
    	if(stt == DATA_NA)
    	{
    		repeat++;
    		if((repeat > 200000) && (fw_info.flash_start_addr == APPLICATION_START_ADDR))
    		{
    			Bt_Uart_deinit();
    			fuota_Deinit();
    			// ... Bootloader system init and verification ...
    			// Attempt to jump
    			boot_jump_status_t result = boot_jump_to_app(APPLICATION_START_ADDR);

    			if (result != BOOT_JUMP_OK) {
    				// Jump failed. Stay in bootloader, signal an error, or wait.
    				while(1);
    			}
    		}
    	}
    	else
    	{
    		repeat = 0;
    	}
    	for(int i=0; i < 200; i++);
        // Application code here
    }

    return 0;
}
