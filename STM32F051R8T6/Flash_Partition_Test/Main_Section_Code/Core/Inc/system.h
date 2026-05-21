/**
 * @file system.h
 * @brief System configuration and declarations for PIC18F45K20
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

// Define the function signatures for your API
typedef void (*void_func_t)(void);
typedef int32_t (*math_func_t)(int32_t, int32_t);

// The Jump Table Structure
typedef struct {
    uint32_t magic_number;    // Optional: To verify API is valid/present
    void_func_t init_peripheral;
    math_func_t process_data;
} api_table_t;

// The fixed flash address where the API table will live
#define API_TABLE_ADDRESS    ((uint32_t)0x0800C000)

#endif // SYSTEM_H
