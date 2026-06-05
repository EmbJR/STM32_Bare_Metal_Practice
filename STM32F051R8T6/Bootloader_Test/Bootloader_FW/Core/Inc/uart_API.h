#ifndef UAPI_H
#define UAPI_H
#include "uartF051.h"

int Bt_Uart_Init(void);
void Bt_Uart_deinit(void);
bool UART_SendDataIT(USART_TypeDef *USARTx, const uint8_t data);

#endif//UAPI_H
