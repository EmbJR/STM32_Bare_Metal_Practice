/**
 * @file system.h
 * @brief System configuration and declarations for PIC18F45K20
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

typedef struct
{
    uint8_t data[10];
    uint32_t value;
} MyStruct;

#endif // SYSTEM_H
