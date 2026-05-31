#include "fuota_packat_Process.h"
#include "CirBuffer.h"
#include "crc.h"

void user_fuota_reply(uint8_t *data, uint16_t size)     __attribute__((weak));
{
    // Implement the function to send data back to the sender
    // This is a placeholder for actual implementation
    // You can use UART, SPI, or any other communication interface to send the data
    printf("Replying with data: ");
    for (uint16_t i = 0; i < size; i++)
    {
        printf("%02X ", data[i]);
    }
    printf("\n");
}


///static CircularBuffer *txBuffer = NULL;
static CircularBuffer *rxBuffer = NULL;

#if (CPU_VAL == CPU_8Bit)
#define DATA_SIZE (FOTA_BUFFER_SIZE + TxRx_Data_POS)
uint8_t Fudata[DATA_SIZE] = {0};
#elif ((CPU_VAL == CPU_16Bit) || (CPU_VAL == CPU_32Bit))
#define DATA_SIZE (FOTA_BUFFER_SIZE + TxRx_Data_POS)
uint8_t Fudata[DATA_SIZE] = {0};
#endif

fw_up_str fw_info;


fw_err fuota_init(void)
{
    rxBuffer = circular_buffer_create(RECV_BUFFER_SIZE);

    //Create RX circular buffer
    if (rxBuffer == NULL)
    {
        // Handle error - buffer creation failed
        return P_ERR;
    }
    return SUC;
}


fw_err fuota_incomming_buffer_process(char *data, uint16_t size)
{
    if(!data)
        return P_ERR;

    for(int i=0; i<size; i++)
    {
        //Check if buffer has space using API
        if (circular_buffer_is_full(rxBuffer))
        {
            return P_ERR;  //Buffer full
        }

        //Write data to circular buffer using API
        if (circular_buffer_write(rxBuffer, (char)data[i]) != BUFFER_OK)
        {
            return P_ERR;  //Write failed
        }
    }

    return SUC;
}

fw_err fuota_process_Data(void)
{
    uint8_t data = 0;
    uint16_t replylen = 0;

    static uint16_t length = 0, rcvlen=0;
    static uint16_t crc16_Received = 0, crc16_Generated = 0;

    static uint8_t status = ST_INIT;
    static uint8_t substate = ST_INIT;

    if(!circular_buffer_available(rxBuffer))
        return DATA_NA;

    if (circular_buffer_is_empty(rxBuffer))
        return P_ERR;  // Buffer empty

    switch (status)
    {
    case ST_INIT:
        length = crc16_Received = crc16_Generated = rcvlen = 0;
        // Read data from circular buffer using API
        if (circular_buffer_read(rxBuffer, (char *)&data) != BUFFER_OK)
        {
            return P_ERR;  // Read failed
        }

        if(data == DATA_HEADER)
        {
            status = ST_CRC_DEC;
        }
        break;
    case ST_CRC_DEC:

        substate = ST_CRC_DEC_SUB1;
        switch (substate)
        {
        case ST_CRC_DEC_SUB1:
            // Read data from circular buffer using API
            if (circular_buffer_read(rxBuffer, (char *)&data) != BUFFER_OK)
            {
                return P_ERR;  // Read failed
            }
            crc16_Received = data;
            substate = ST_CRC_DEC_SUB2;
            break;
        case ST_CRC_DEC_SUB2:
            // Read data from circular buffer using API
            if (circular_buffer_read(rxBuffer, (char *)&data) != BUFFER_OK)
            {
                return P_ERR;  // Read failed
            }
            crc16_Received |= (data << 8);
            status = ST_LEN_DEC;
            break;
        }
        break;
    case ST_LEN_DEC:

        substate = ST_LEN_DEC_SUB1;
        switch (substate)
        {
        case ST_LEN_DEC_SUB1:
            // Read data from circular buffer using API
            if (circular_buffer_read(rxBuffer, (char *)&data) != BUFFER_OK)
            {
                return P_ERR;  // Read failed
            }
            length = data;
            substate = ST_LEN_DEC_SUB2;
            memset(Fudata, 0x00, sizeof(Fudata));
            break;
        case ST_LEN_DEC_SUB2:
            // Read data from circular buffer using API
            if (circular_buffer_read(rxBuffer, (char *)&data) != BUFFER_OK)
            {
                return P_ERR;  // Read failed
            }
            length |= (data << 8);
            if(length == 0)
            {
                length = fuota_encodeErrro(Fudata, 0x00, FOTA_ERROR_LEN);
                user_fuota_reply(Fudata, length);
                status = ST_INIT;
            }
            else
            {
                status = ST_DATA_VALIDATION;
            }
            break;
        }
        break;
    case ST_DATA_VALIDATION:
        if (circular_buffer_read(rxBuffer, (char *)&data) != BUFFER_OK)
        {
            return P_ERR;  // Read failed
        }
        if(rcvlen < length)
        {
            Fudata[rcvlen++] = data;
        }
        else
        {
            crc16_Generated = CalculateCRC16((uint8_t*)Str, length+2);  // total length + length data bytes
            if(crc16_Generated == crc16_Received)
            {
                length = fuota_encodeErrro(Fudata, Fudata[0], FOTA_ERROR_CRC);
                user_fuota_reply(Fudata, length);
                return P_ERR;
            }
            else
            {
                return SUC;
            }
        }
        break;
    case ST_CM_PROCESS:
        replylen = fuota_cmd_process(Fudata, rcvlen);
        user_fuota_reply(Fudata, replylen);
        break;
    default:
        break;
    }
    //Check if buffer has data using API
}

uint16_t fuota_cmd_process(volatile uint8_t *cmdData, uint16_t cmdLength)
{
    if(!cmdData || cmdLength == 0)
        return 0;

    // Process the command data and perform necessary actions
    // This is a placeholder for actual command processing logic
    // You can implement your specific command handling here
    switch (cmdData[0])
    {
        case CMD_GET_INFO:
            // Handle GET_INFO command
            user_fuota_get_info(&fw_info); // Assuming this function fills fw_info structure
            return fuota_encodeData(Fudata, cmdData[0], (const uint8_t*)&fw_info, sizeof(fw_info));
            break;
        case CMD_SET_CHUNK_SIZE:
            // Handle SET_CHUNK_SIZE command
            break;
        case CMD_SET_ADDR:
            // Handle SET_ADDR command
            break;
        case CMD_FW_DATA:
            // Handle FW_DATA command
            break;
        default:
            length = fuota_encodeErrro(Fudata, Fudata[0], CMD_ERR);
            return P_ERR;  // Unknown command
    }

    return SUC;  // Return success or appropriate error code based on processing result
}


uint16_t fuota_encodeData(volatile uint8_t *encodedData, uint8_t commandVal, const uint8_t *datatoencode, uint16_t sizeofData)
{
    uint16_t length = 0;
    uint16_t crc16 = 0;
    bool success = false;

    if(encodedData == NULL)
        return 0;

    // Data formate == [Start Byte = 0xAA][CRC of (data)][Length of data][data]
    encodedData[0] = DATA_HEADER;
    length = length + 1;

    encodedData[TxRx_Len_POS] = (uint8_t)((uint16_t)(sizeofData + TxRx_ADR_STA_POS + 1) & 0x00FF);
    encodedData[TxRx_Len_POS + 1] = (uint8_t)(((uint16_t)(sizeofData + TxRx_ADR_STA_POS + 1) & 0xFF00) >> 8);
    length = length + 2;

    encodedData[TxRx_CMD_POS] = commandVal;
    length = length + 1;
    
    memcpy((char*)(encodedData + TxRx_ADR_STA_POS), (char*)datatoencode, sizeofData);
    length = length + sizeofData;

    crc16 = CalculateCRC16((uint8_t *)&encodedData[TxRx_Len_POS], (length - 1));
    encodedData[TxRx_CRC_POS] = (uint8_t)(crc16 & 0x00FF);
    encodedData[TxRx_CRC_POS + 1] = (uint8_t)((crc16 & 0xFF00) >> 8);
    length = length + 2;

    return LEN_DATA_TXRX;
}

uint16_t fuota_encodeErrro(volatile uint8_t *encodedData, uint8_t commandVal, uint8_t errorVal)
{
    uint16_t crc16 = 0;
    uint16_t length = 0;

    if(encodedData == NULL)
        return 0;

    // Data formate == [Start Byte = 0xAA][CRC of (data)][Length of data][data]
    encodedData[0] = DATA_HEADER;
    length = length + 1;

    encodedData[TxRx_Len_POS] = (uint8_t)(0x0001 & 0x00FF);
    encodedData[TxRx_Len_POS + 1] = (uint8_t)((0x0001 & 0xFF00) >> 8);
    length = length + 2;

    encodedData[TxRx_CMD_POS] = commandVal;
    length = length + 1;        // CMD

    encodedData[TxRx_CMD_STA_POS] = errorVal;
    length = length + 1;        // Status position

    crc16 = CalculateCRC16((uint8_t *)&encodedData[TxRx_Len_POS], (length - 1));
    encodedData[TxRx_CRC_POS] = (uint8_t)(crc16 & 0x00FF);
    encodedData[TxRx_CRC_POS + 1] = (uint8_t)((crc16 & 0xFF00) >> 8);
    length = length + 2;

    return length;
}

int main(void)
{

    return 0;
}
