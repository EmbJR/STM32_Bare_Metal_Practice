// Example
// cirbuff_str RxBuff =
// {
//     .buffer = {0},
//     .u8ReadBuffCnt = 0,
//     .u8WriteBuffCnt = 0,
//     .eBuff_error = BUFFER_NO_ERROR
// };

// cirbuff_str TxBuff =
// {
//     .buffer = {0},
//     .u8ReadBuffCnt = 0,
//     .u8WriteBuffCnt = 0,
//     .eBuff_error = BUFFER_NO_ERROR
// };
// eBuff_Err_Type eBuff_error = BUFFER_NO_ERROR;
// uint8_t DataToWrite[12] = "Hello World\n";
// uint8_t DataToWrite2[12] = "Bhai Keseho\n";
// int main()
// {
//     uint8_t readdata = 0;
//     uint16_t cnt = 0;
//     uint8_t data[100] = {0};
//     eBuff_Err_Type errval = BUFFER_NO_ERROR;
//     circular_buff_reset(&RxBuff);
//     circular_buff_reset(&TxBuff);

//     while(1)
//     {
//         for(int i = 0; i< 100000; i++);
//         errval = circular_buff_write_array(&RxBuff, (uint8_t *)DataToWrite, sizeof(DataToWrite));
//         if(errval == BUFFER_NO_ERROR)
//         {
//             errval = circular_buff_write_array(&TxBuff, (uint8_t *)DataToWrite2, sizeof(DataToWrite2));
//             if(errval == BUFFER_NO_ERROR)
//             {
//                 cnt = 0;
//                 memset((uint8_t*)data, 0x00, sizeof(data));
//                 do
//                 {
//                     readdata = circular_buff_Read_byte(&TxBuff, &errval);
//                     if(errval == BUFFER_NO_ERROR)
//                     {
//                         data[cnt++] = readdata;
//                     }

//                 }
//                 while(errval == BUFFER_NO_ERROR);
//                 printf("Reading---Data = %s\n", (char *)data);
//             }
//             cnt = 0;
//             memset((uint8_t*)data, 0x00, sizeof(data));
//             do
//             {
//                 readdata = circular_buff_Read_byte(&RxBuff, &errval);
//                 if(errval == BUFFER_NO_ERROR)
//                 {
//                     data[cnt++] = readdata;
//                 }

//             }
//             while(errval == BUFFER_NO_ERROR);
//             printf("Reading---Data = %s\n", (char *)data);

//         }
//     }
//     return 0;
// }

//#include <stdio.h>
//#include <stdint.h>
//#include <stdlib.h>
#include <string.h>
#include "CirBuffer.h"


eBuff_Err_Type circular_buff_Read_byte(cirbuff_str* buff, uint8_t *data)
{
    eBuff_Err_Type errval = BUFFER_NO_ERROR;
    if(buff->u8ReadBuffCnt != buff->u8WriteBuffCnt)
    {
        *data = buff->buffer[buff->u8ReadBuffCnt];
        buff->u8ReadBuffCnt++;
        buff->u8ReadBuffCnt = (buff->u8ReadBuffCnt % UART_BUFF_SIZE);
        buff->eBuff_error = BUFFER_NO_ERROR;
        errval = BUFFER_NO_ERROR;
    }
    else
    {
        buff->eBuff_error = BUFFER_UNDEFLOW;
        errval = BUFFER_UNDEFLOW;
    }
    return errval;
}

eBuff_Err_Type circular_buff_Write_byte(cirbuff_str* buff, uint8_t data)
{
    eBuff_Err_Type errval = BUFFER_NO_ERROR;
    uint16_t nextWrite = (buff->u8WriteBuffCnt + 1) % UART_BUFF_SIZE;
    if(nextWrite != buff->u8ReadBuffCnt)
    {
        buff->buffer[buff->u8WriteBuffCnt] = data;
        buff->u8WriteBuffCnt = nextWrite;
        buff->eBuff_error = BUFFER_NO_ERROR;
        errval = BUFFER_NO_ERROR;
    }
    else
    {
        buff->eBuff_error = BUFFER_OVERFLOW;
        errval = BUFFER_OVERFLOW;
    }
    return errval;
}

void circular_buff_reset(cirbuff_str* buff)
{
    buff->u8ReadBuffCnt = 0;
    buff->u8WriteBuffCnt = 0;
    buff->eBuff_error = BUFFER_NO_ERROR;
    memset(buff->buffer, 0x00, sizeof(buff->buffer));
}

eBuff_Err_Type circular_buff_write_array(cirbuff_str* buff, uint8_t *data, uint16_t len)
{
    eBuff_Err_Type errval = BUFFER_NO_ERROR;
    uint16_t i = 0;

    for(i = 0; i < len; i++)
    {
        errval = circular_buff_Write_byte(buff, data[i]);
        if(errval != BUFFER_NO_ERROR)
        {
            break;
        }
    }

    if(i < len)
    {
        buff->eBuff_error = errval;
        return errval;
    }

    buff->eBuff_error = BUFFER_NO_ERROR;
    return BUFFER_NO_ERROR;
}
