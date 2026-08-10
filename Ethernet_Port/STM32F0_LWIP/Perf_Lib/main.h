#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include "uartF051.h"

void Delay_ms(uint32_t ms);
void read_reg_data(void);

extern uint8_t printdata[50];

#endif	//MAIN_H
