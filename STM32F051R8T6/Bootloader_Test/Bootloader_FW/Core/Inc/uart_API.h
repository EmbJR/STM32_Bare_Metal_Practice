#ifndef UAPI_H
#define UAPI_H
#include "uartF051.h"

int Bt_Uart_Init(void);
bool UART_SendStringIT(USART_TypeDef *USARTx, const char *str);

#endif//UAPI_H
