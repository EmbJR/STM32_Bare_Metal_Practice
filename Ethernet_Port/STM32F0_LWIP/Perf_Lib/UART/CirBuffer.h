/**
 * @file    CirBuffer.h
 * @brief   Circular Buffer Header File
 * @author  AI Generated
 * @date    2026-03-06
 * 
 * @note    This header provides declarations for circular buffer management
 */

#ifndef CIRBUFFER_H
#define CIRBUFFER_H

//#include <stddef.h>
#include <stdint.h>

#define UART_BUFF_SIZE  25

typedef enum
{
    BUFFER_NO_ERROR,
    BUFFER_OVERFLOW,
    BUFFER_UNDEFLOW,
    BUFFER_ERROR,
} eBuff_Err_Type;

typedef struct
{

    uint8_t buffer[UART_BUFF_SIZE];
    uint16_t u8ReadBuffCnt;
    uint16_t u8WriteBuffCnt;
    uint8_t eBuff_error;
} cirbuff_str;

uint8_t circular_buff_Read_byte(cirbuff_str* buff, eBuff_Err_Type *err);
eBuff_Err_Type circular_buff_Write_byte(cirbuff_str* buff, uint8_t data);
void circular_buff_reset(cirbuff_str* buff);
eBuff_Err_Type circular_buff_write_array(cirbuff_str* buff, uint8_t *data, uint16_t len);



#endif /* CIRBUFFER_H */
