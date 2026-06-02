#include "rcc.h"
#include "uartF051.h"
#include "CirBuffer.h"
#include "uart_API.h"
#include "fuota_packat_Process.h"
#include "FlashF051.h"


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

fw_err use_Flash_Write(uint8_t *data, uint16_t size)
{
	Flash_Status_t status = Example_SafeProgramWithRetry(fw_info.flash_start_addr, data, size, 4);
	if(FLASH_STATUS_OK == status)
	{
		return SUC;
	}
	return FOTA_ERROR_FLASH_WR;
}

#endif
int main(void) {
    // Configure system clock
	SystemClock_Config_48MHz();

    // Get current system clock frequency
    uint32_t sysclk = RCC_GetSystemClockFrequency();

    Bt_Uart_Init();
    fuota_init();

    while (1) {
    	//UART_SendStringIT(USART1, "Hello\n");
    	fuota_process_Data();
    	for(int i=0; i < 20; i++);
        // Application code here
    }

    return 0;
}
