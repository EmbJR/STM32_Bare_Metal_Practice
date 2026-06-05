#include "fuota_packat_Process.h"
#include "CirBuffer.h"
#include "crc.h"

#if (CPU_VAL == CPU_8Bit)
#define DATA_SIZE (FOTA_BUFFER_SIZE + TxRx_Data_POS)
volatile uint8_t Fudata[DATA_SIZE] = {0};
#elif ((CPU_VAL == CPU_16Bit) || (CPU_VAL == CPU_32Bit))
#define DATA_SIZE (FOTA_BUFFER_SIZE + TxRx_Data_POS)
volatile uint8_t Fudata[DATA_SIZE] = {0};
volatile uint8_t FudataTx[DATA_SIZE] = {0};
#endif

///static CircularBuffer *txBuffer = NULL;
static volatile CircularBuffer *rxBuffer = NULL;


fw_up_str fw_info;
volatile uint8_t FuComplete = 0;

__attribute__((weak))
fw_err use_Flash_Write(const uint8_t *data, uint16_t size)
{
    return 0;  /* Placeholder implementation */
}


__attribute__((weak))
void user_fuota_reply(uint8_t *data, uint16_t size)
{
#if 0
    // Implement the function to send data back to the sender
    // This is a placeholder for actual implementation
    // You can use UART, SPI, or any other communication interface to send the data
    printf("Replying with data: ");
    for (uint16_t i = 0; i < size; i++)
    {
        printf("%02X ", data[i]);
    }
    printf("\n");
#endif
}


__attribute__((weak))
fw_err user_fuota_get_info(fw_up_str *fw_info)
{
#if 0
    // Implement the function to fill the fw_info structure with actual firmware information
    // This is a placeholder for actual implementation
    fw_info->cpu_Type = 0x01; // Example CPU type
    fw_info->hw_ver[0] = 0x01; // Example hardware version
    fw_info->hw_ver[1] = 0x00;
    fw_info->hw_ver[2] = 0x00;
    fw_info->fw_Ver[0] = 0x01; // Example firmware version
    fw_info->fw_Ver[1] = 0x00;
    fw_info->fw_Ver[2] = 0x00;
    fw_info->fw_Ver[3] = 0x00;
    fw_info->fw_Ver[4] = 0x00;
    fw_info->fw_Ver[5] = 0x00;
    fw_info->fw_Ver[6] = 0x00;
    fw_info->fw_Ver[7] = 0x00;
    fw_info->fw_crc16 = 0x1234; // Example CRC16 of the firmware
    fw_info->chunk_size = 256; // Example chunk size
    fw_info->chunk_addr = 0x8000; // Example chunk address
    fw_info->flash_start_addr = 0x08000000; // Example firmware address
#endif
    return 0;  /* Placeholder implementation */
}

__attribute__((weak))
fw_err user_fuota_set_info(fw_up_str *fw_info)
{
    return 0;  /* Placeholder implementation */
}



//----------------- test function ------------------//
// returns total packet length
uint16_t make_packet(uint8_t cmd, const uint8_t *payload, uint16_t payload_len, uint8_t *out)
{
#if 0
    // out layout: [HDR][CRC_L][CRC_H][LEN_L][LEN_H][CMD][payload...]
    out[0] = 0xA5;
    uint16_t len = (uint16_t)(1 + payload_len); // cmd + payload
    out[3] = (uint8_t)(len & 0xFF);
    out[4] = (uint8_t)((len >> 8) & 0xFF);
    out[5] = cmd;
    if (payload_len && payload != NULL) memcpy(&out[6], payload, payload_len);
    // CRC covers actual data only: CMD + payload, excluding the length bytes.
    uint16_t crc = CalculateCRC16(&out[5], len);
    out[1] = (uint8_t)(crc & 0xFF);
    out[2] = (uint8_t)((crc >> 8) & 0xFF);
    return (uint16_t)(1 + 2 + 2 + len); // header + crc(2) + len(2) + len bytes
#endif
}


fw_err fuota_init(void)
{
    rxBuffer = circular_buffer_create(UART_RECV_BUFFER_SIZE);

    //Create RX circular buffer
    if (rxBuffer == NULL)
    {
        // Handle error - buffer creation failed
        return P_ERR;
    }
    return SUC;
}

void fuota_Deinit(void)
{
	circular_buffer_destroy(rxBuffer);
}


fw_err fuota_incomming_buffer_process(volatile uint8_t *data, uint16_t size)
{
    if(!data)
        return P_ERR;

    for(int i=0; i<size; i++)
    {
        //Check if buffer has space using API
        if (circular_buffer_is_full((CircularBuffer *)rxBuffer))
        {
            return P_ERR;  //Buffer full
        }

        //Write data to circular buffer using API
        if (circular_buffer_write(rxBuffer, (volatile uint8_t)data[i]) != BUFFER_OK)
        {
            return P_ERR;  //Write failed
        }
    }

    return SUC;
}

static uint16_t Clear_And_Build_reply(volatile uint8_t command, uint8_t reply)
{
	volatile uint8_t replystr[2] = {0};
	replystr[0] = reply;
    return fuota_encodeData(FudataTx, command, (const uint8_t*)replystr, 1);
    circular_buffer_clear(rxBuffer);

}

uint16_t crc16_Generated = 0;
fw_err fuota_process_Data(void)
{
    fw_err err = DATA_NA;  /* Initialize to default value */
    volatile uint8_t data = 0;
    volatile uint16_t replylen = 0;

    static volatile uint16_t length = 0, rcvlen=0,  retrycnt = 0;
    static volatile uint16_t crc16_Received = 0;

    static volatile uint8_t status = ST_INIT;
    static volatile uint8_t substate = ST_INIT;

    static volatile BufferStatus recvstatus = BUFFER_OK;

    if(!circular_buffer_available((CircularBuffer *)rxBuffer) && (length == 0))
    {
        return DATA_NA;
    }

//    if (circular_buffer_is_empty(rxBuffer))
//        return P_ERR;  // Buffer empty

    switch (status)
    {
    case ST_INIT:
        length = crc16_Received = crc16_Generated = rcvlen = 0;
        // Read data from circular buffer using API
        if (circular_buffer_read(rxBuffer, &data) != BUFFER_OK)
        {
            return P_ERR;  // Read failed
        }
        if(data == DATA_HEADER)
        {
            status = ST_CRC_DEC;
            substate = ST_CRC_DEC_SUB1;
        }
        break;
    case ST_CRC_DEC:
        switch (substate)
        {
        case ST_CRC_DEC_SUB1:
            // Read data from circular buffer using API
                if (circular_buffer_read(rxBuffer, &data) != BUFFER_OK)
            {
                err = P_ERR;  // Read failed
            }
            else
            {
                crc16_Received = (data << 8);
                substate = ST_CRC_DEC_SUB2;
            }
            break;
        case ST_CRC_DEC_SUB2:
            // Read data from circular buffer using API
                if (circular_buffer_read(rxBuffer, &data) != BUFFER_OK)
            {
                err = P_ERR;  // Read failed
            }
            else
            {
                crc16_Received |= data;
                status = ST_LEN_DEC;
                substate = ST_LEN_DEC_SUB1;
            }

            break;
        }
        break;
    case ST_LEN_DEC:
        switch (substate)
        {
        case ST_LEN_DEC_SUB1:
            // Read data from circular buffer using API
                if (circular_buffer_read(rxBuffer, &data) != BUFFER_OK)
            {
                err = P_ERR;
            }
            else
            {
                length = (data << 8);
                substate = ST_LEN_DEC_SUB2;
                memset((uint8_t *)Fudata, 0x00, sizeof(Fudata));
                memset((uint8_t *)FudataTx, 0x00, sizeof(FudataTx));

            }
            break;
        case ST_LEN_DEC_SUB2:
            // Read data from circular buffer using API
                if (circular_buffer_read(rxBuffer, &data) != BUFFER_OK)
            {
                err = P_ERR;
            }
            else
            {
                length |= data;
                if((length == 0) | (length > DATA_SIZE))
                {
                	length = Clear_And_Build_reply((volatile uint8_t)0x00, (volatile uint8_t)FOTA_ERROR_LEN);
                    user_fuota_reply((uint8_t *)FudataTx, length);
                    status = ST_INIT;
                    err = FOTA_ERROR_LEN;
                }
                else
                {
                    status = ST_DATA_VALIDATION;
                }
            }
            break;
        }
        break;
    case ST_DATA_VALIDATION:
		recvstatus = circular_buffer_read(rxBuffer, &data);
		if (recvstatus != BUFFER_OK)
		{
			// implement retry timeout.
			retrycnt++;
			if(retrycnt > 500)
			{
				length = Clear_And_Build_reply((volatile uint8_t)FudataTx[0], (volatile uint8_t)FOTA_ERROR_LEN);
				user_fuota_reply((uint8_t *)FudataTx, length);
				err = FOTA_ERROR_LEN;
				status = ST_INIT;
				retrycnt = 0;
			}
			err = FOTA_ERROR_LEN;
		}
		else
		{
			Fudata[rcvlen++] = data;
			if(rcvlen >= length)
			{
				crc16_Generated = CalculateCRC16((uint8_t*)Fudata, length);
				if(crc16_Generated == crc16_Received)
				{
					err = SUC;
					status = ST_CM_PROCESS;
				}
				else
				{
					length = Clear_And_Build_reply(FudataTx[0], (volatile uint8_t)FOTA_ERROR_CRC);
					user_fuota_reply((uint8_t *)FudataTx, length);
					err = FOTA_ERROR_CRC;
					status = ST_INIT;
				}
			}
		}
        break;
    case ST_CM_PROCESS:
    	if(Fudata[0] == ST_CM_APP_JUMP)
    	{
    		FuComplete = 0x00;
    	}
        replylen = fuota_cmd_process((volatile uint8_t*)Fudata, rcvlen);
        user_fuota_reply((uint8_t *)FudataTx, replylen);
        if(FuComplete == 0x66)
        {
            fw_info.fw_crc16 = 0x1234; // Example CRC16 of the firmware
            fw_info.chunk_size = 256; // Example chunk size
            fw_info.chunk_addr = 0x8000; // Example chunk address
            fw_info.flash_start_addr = 0x08002800; // Example firmware address
            user_fuota_set_info(&fw_info);
        }
        circular_buffer_clear(rxBuffer);
        status = ST_INIT;
        break;
    default:
        return P_ERR;
        break;
    }
    return err;
}

uint16_t gllength = 0;
uint16_t fuota_cmd_process(volatile uint8_t *cmdData, uint16_t cmdLength)
{
	fw_err err;
    volatile uint16_t replen = 0;
    if(!cmdData || cmdLength == 0)
        return 0;

    gllength = cmdLength;
    // Process the command data and perform necessary actions
    // This is a placeholder for actual command processing logic
    // You can implement your specific command handling here
    switch (cmdData[0])
    {
    case CMD_GET_INFO:
        // Handle GET_INFO command
        user_fuota_get_info(&fw_info); // Assuming this function fills fw_info structure
        replen = fuota_encodeData(FudataTx, cmdData[0], (const uint8_t*)&fw_info, sizeof(fw_info));
        break;
    case CMD_SET_CHUNK_SIZE:
        // Handle SET_CHUNK_SIZE command
        fw_info.chunk_size = (uint16_t)((cmdData[1] << 8) | cmdData[2]);
        fw_info.nb_chunk = (uint16_t)((cmdData[3] << 8) | cmdData[4]);
        replen = Clear_And_Build_reply(cmdData[0], SUC);
        //fw_info.nb_chunk = (uint16_t)(cmdData[1]);
        break;
    case CMD_SET_ADDR:
        // Handle SET_ADDR command//0x08002800;
		fw_info.flash_start_addr =  0x00000000;
    	fw_info.flash_start_addr = (uint32_t)((cmdData[1] << 24) | (cmdData[2] << 16) | (cmdData[3] << 8) | cmdData[4]);
    	replen = Clear_And_Build_reply(cmdData[0], SUC);
        break;
    case CMD_FW_DATA:
        // Handle FW_DATA command
    	err = use_Flash_Write((const uint8_t *)&cmdData[1], (cmdLength - 1));
    	if(err == SUC)
    	{
    		replen = Clear_And_Build_reply(cmdData[0], SUC);
        	fw_info.flash_start_addr = fw_info.flash_start_addr + (cmdLength - 1);
    	}
    	else
    	{
    		replen = fuota_encodeErrro(FudataTx, cmdData[0], (volatile uint8_t)err);
    	}
        break;
    case ST_CM_APP_JUMP:
    	FuComplete = 0x66;
    	replen = Clear_And_Build_reply(cmdData[0], SUC);
    	break;
    default:
        return fuota_encodeErrro(FudataTx, Fudata[0], (volatile uint8_t)CMD_ERR);;  // Unknown command
    }

    return replen;  // Return success or appropriate error code based on processing result
}


uint16_t fuota_encodeData(volatile uint8_t *encodedData, uint8_t commandVal, const uint8_t *datatoencode, uint16_t sizeofData)
{
    uint16_t length = 0;
    uint16_t crc16 = 0;

    if(encodedData == NULL)
        return 0;

    // Data formate == [Start Byte = 0xAA][CRC of (data)][Length of data][data]
    encodedData[0] = DATA_HEADER;
    length = length + 1;

    encodedData[TxRx_Len_POS] = (uint8_t)((uint16_t)(sizeofData + 1) & 0x00FF);
    encodedData[TxRx_Len_POS + 1] = (uint8_t)(((uint16_t)(sizeofData + 1) & 0xFF00) >> 8);
    length = length + 2;

    encodedData[TxRx_CMD_POS] = commandVal;
    length = length + 1;

    memcpy((uint8_t*)(encodedData + TxRx_ADR_STA_POS), (const uint8_t*)datatoencode, sizeofData);
    length = length + sizeofData;

    crc16 = CalculateCRC16((uint8_t *)&encodedData[TxRx_CMD_POS], (length - 3));    // len - Header(1) - Length bytes(2)
    encodedData[TxRx_CRC_POS] = (uint8_t)(crc16 & 0x00FF);
    encodedData[TxRx_CRC_POS + 1] = (uint8_t)((crc16 & 0xFF00) >> 8);
    length = length + 2;

    return length;
}

uint16_t fuota_encodeErrro(volatile uint8_t *encodedData, volatile uint8_t commandVal, volatile uint8_t errorVal)
{
    volatile uint16_t crc16 = 0;
    volatile uint16_t lengthreply = 0;

    if(encodedData == NULL)
        return 0;

    // Data formate == [Start Byte = 0xAA][CRC of (data)][Length of data][data]
    encodedData[0] = DATA_HEADER;
    lengthreply = lengthreply + 1;

    encodedData[TxRx_Len_POS] = (uint8_t)(0x0002 & 0x00FF);
    encodedData[TxRx_Len_POS + 1] = (uint8_t)((0x0002 & 0xFF00) >> 8);
    lengthreply = lengthreply + 2;

    encodedData[TxRx_CMD_POS] = commandVal;
    lengthreply = lengthreply + 1;        // CMD

    encodedData[TxRx_ADR_STA_POS] = errorVal;
    lengthreply = lengthreply + 1;        // Status position

    crc16 = CalculateCRC16((uint8_t *)&encodedData[TxRx_CMD_POS], (lengthreply - 3));
    encodedData[TxRx_CRC_POS] = (uint8_t)(crc16 & 0x00FF);
    encodedData[TxRx_CRC_POS + 1] = (uint8_t)((crc16 & 0xFF00) >> 8);
    lengthreply = lengthreply + 2;

    return lengthreply;
}

int fuota_test_Case(void)
{
#if 0
    fw_err err;
#if 1
    //----------- Ex1 --------------//
    uint8_t buf1[512];
    uint16_t n1 = make_packet(0x02, NULL, 0, buf1); // cmd=0x02, no payload
    //------------------------------//

    //------------ Ex2 --------------//
    uint8_t payload2[] = { 0x10, 0x00, 0x05, 0x00};

    uint8_t buf2[512];
    uint16_t n2 = make_packet(0x03, payload2, sizeof(payload2), buf2);
    //-------------------------------//

    fuota_init();

    fuota_incomming_buffer_process((const uint8_t *)buf2, n2);
    int len = circular_buffer_available(rxBuffer);

    for(int i=0; i <= len+2; i++)
    {
        err = fuota_process_Data();
        if(err != SUC)
        {
            printf("Error occured..");
        }
    }
#endif
    return 0;
#endif
    return 0;  /* Placeholder for disabled test case */
}
