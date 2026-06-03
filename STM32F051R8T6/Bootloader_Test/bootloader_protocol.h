#ifndef BOOTLOADER_PROTOCOL_H
#define BOOTLOADER_PROTOCOL_H

#include <stdint.h>
#include <stdlib.h>

/* Protocol Constants */
#define BL_HEADER           0xA5
#define BL_RETRY_LIMIT      5
#define BL_TIMEOUT_MS       5000

/* Commands */
#define CMD_READ_MEM        0x01
#define CMD_GET_INFO        0x02
#define CMD_SET_CHUNK_SIZE  0x03
#define CMD_SET_START_ADDR  0x04
#define CMD_FW_DATA         0x05

/* Status and Error Codes */
#define STATUS_SUCCESS      0x64
#define ERR_INTERNAL        0x65
#define ERR_CMD_UNKNOWN     0x66
#define ERR_DATA_UNAVAIL    0x67
#define ERR_INVALID_LEN     0x68
#define ERR_INVALID_CRC     0x69
#define ERR_FLASH_WRITE     0x6A

/**
 * @brief Bootloader information block structure
 */
typedef struct {
    uint8_t  cpu_type;
    uint8_t  hw_ver[3];
    uint8_t  fw_ver[8];
    uint16_t fw_crc16;
    uint16_t chunk_size;
    uint16_t nb_chunk;
    uint16_t chunk_addr; // Current chunk index or relative offset
    uint32_t flash_start_addr;
} fw_up_str;

/**
 * @brief Parser States
 */
typedef enum {
    BL_STATE_IDLE,
    BL_STATE_GET_CRC_L,
    BL_STATE_GET_CRC_H,
    BL_STATE_GET_LEN_L,
    BL_STATE_GET_LEN_H,
    BL_STATE_GET_DATA
} bl_state_t;

/**
 * @brief Protocol Packet Structure (Internal Representation)
 */
typedef struct {
    uint16_t crc16;
    uint16_t length;
    uint8_t  cmd;
    uint8_t  *data;
} bl_packet_t;

/**
 * @brief Bootloader Context to manage state and dynamic buffers
 */
typedef struct {
    fw_up_str info;
    uint8_t   *chunk_buffer;
    uint16_t  current_buffer_size;
    uint8_t   is_initialized;
    bl_state_t state;
} bl_context_t;

/* Function Prototypes */
void BL_Init(void);
uint16_t BL_CalculateCRC16(uint8_t *data, uint16_t length);
int8_t BL_ProcessPacket(bl_packet_t *packet);
void BL_ParseByte(uint8_t byte);
void BL_SendResponse(uint8_t cmd, uint8_t status);

#endif /* BOOTLOADER_PROTOCOL_H */