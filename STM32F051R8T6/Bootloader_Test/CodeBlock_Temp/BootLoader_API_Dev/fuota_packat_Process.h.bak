#ifndef PACKET_PROCESS_H
#define PACKET_PROCESS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

//-------------------- Header file content --------------------------//

#define    CPU_8Bit     1       // memory with 8bit address increment.
#define    CPU_32Bit    2       // memory with 32bit address increment.
#define    CPU_16Bit    3       // memory with 16bit address increment.

#define CPU_VAL  CPU_16Bit

#if (CPU_VAL == CPU_8Bit)
#define FOTA_BUFFER_SIZE    128
#define RECV_BUFFER_SIZE    100
#elif ((CPU_VAL == CPU_16Bit) || (CPU_VAL == CPU_32Bit))
#define FOTA_BUFFER_SIZE    512
#define RECV_BUFFER_SIZE    200
#endif

#define TxRx_CRC_POS			    1	// 1 byte
#define TxRx_Len_POS			    3	// 2 bytes
#define TxRx_CMD_POS			    5	// 1 byte
#define TxRx_ADR_STA_POS		    6	// 1 byte
#define TxRx_Data_POS			    8	// 264 bytes

#define LEN_DATA_TXRX			(272)
#define LEN_PAGE_SIZE			(264)
#define LEN_REPLY_CMD			(264 + 2 + 1)

#define DATA_HEADER 			0xA5

/*
1>		0x01		Get memory continously
2>		0x02		Get information data.
2>		0x03		Set Transfer units	// mainly used for changing the chunk sizes information in case of changing the protocol.
4>		0x04		Set Start address.
5>		0x05		Fw data
-------------------------
*/
typedef enum
{
    CMD_GET_INFO = 0x02,
    CMD_SET_CHUNK_SIZE,
    CMD_SET_ADDR,
    CMD_FW_DATA,
} cmd_type;

typedef enum
{
    SUC = 0x64,
    P_ERR,
    CMD_ERR,
    DATA_NA,
    FOTA_ERROR_LEN,
    FOTA_ERROR_CRC,
} fw_err;

typedef struct
{
    uint8_t cpu_Type;
    uint8_t hw_ver[3];
    uint8_t fw_Ver[8];
    uint16_t fw_crc16;
    uint16_t chunk_size;
    uint16_t nb_chunk;
    uint16_t chunk_addr;
    uint32_t fw_addr;
} fw_up_str;

enum
{
    ST_INIT,
    ST_CRC_DEC,
    ST_CRC_DEC_SUB1,
    ST_CRC_DEC_SUB2,
    ST_LEN_DEC,
    ST_LEN_DEC_SUB1,
    ST_LEN_DEC_SUB2,
    ST_DATA_VALIDATION,
    ST_CM_PROCESS,
};

//--------------------------------------------------------------------//
fw_err fuota_init(void);
fw_err fuota_incomming_buffer_process(const uint8_t *data, uint16_t size);
fw_err fuota_process_Data(void);
uint16_t fuota_cmd_process(volatile uint8_t *cmdData, uint16_t cmdLength);
uint16_t fuota_encodeData(volatile uint8_t *encodedData, uint8_t commandVal, const uint8_t *datatoencode, uint16_t sizeofData);
uint16_t fuota_encodeErrro(volatile uint8_t *encodedData, uint8_t commandVal, uint8_t errorVal);

//----- Test function -------------//
uint16_t make_packet(uint8_t cmd, const uint8_t *payload, uint16_t payload_len, uint8_t *out);
int fuota_test_Case(void);

//----------- User Functions --------------------/
void user_fuota_reply(uint8_t *data, uint16_t size);
void user_fuota_get_info(fw_up_str *fw_info);


#endif /* PACKET_PROCESS_H */
