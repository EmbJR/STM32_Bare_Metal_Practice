
#include "main.h"

#include "system.h"


/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Actual API implementations
void api_init_peripheral(void) {
    // HAL_GPIO_Init, etc.
}

int32_t api_process_data(int32_t a, int32_t b) {
	uint8_t addValue = 15;
    return (addValue + (a + b));
}

// Force this struct into the specific linker section
__attribute__((section(".my_fixed_section")))
const api_table_t my_api_table = {
    .magic_number = 0xDEADBEEF,
    .init_peripheral = api_init_peripheral,
    .process_data = api_process_data
};
