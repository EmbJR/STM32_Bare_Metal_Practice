#include "bootloader_protocol.h"
#include <string.h>

static bl_context_t bl_ctx;

/* Static packet used by the parser to avoid stack allocation in interrupts */
static bl_packet_t rx_packet;
static uint16_t rx_data_index = 0;
static uint8_t rx_data_buffer[1024];

/**
 * @brief Initializes the bootloader context.
 */
void BL_Init(void) {
    memset(&bl_ctx, 0, sizeof(bl_context_t));
    bl_ctx.is_initialized = 1;
    bl_ctx.state = BL_STATE_IDLE;
}

/**
 * @brief Standard CRC16-MODBUS implementation.
 */
uint16_t BL_CalculateCRC16(uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/**
 * @brief Core logic for processing commands from the GUI.
 */
int8_t BL_ProcessPacket(bl_packet_t *packet) {
    if (!bl_ctx.is_initialized) return ERR_INTERNAL;

    switch (packet->cmd) {
        case CMD_GET_INFO:
            // Send back the bl_ctx.info structure
            // Implementation for UART TX would go here
            BL_SendResponse(CMD_GET_INFO, STATUS_SUCCESS);
            break;

        case CMD_SET_CHUNK_SIZE:
            if (packet->length < 5) return ERR_INVALID_LEN;
            
            // Parse new chunk size and total chunks
            uint16_t new_size = (packet->data[0] << 8) | packet->data[1];
            bl_ctx.info.chunk_size = new_size;
            bl_ctx.info.nb_chunk = (packet->data[2] << 8) | packet->data[3];

            // Reallocate buffer if size changed
            if (bl_ctx.chunk_buffer != NULL) {
                free(bl_ctx.chunk_buffer);
            }
            
            bl_ctx.chunk_buffer = (uint8_t *)malloc(new_size);
            if (bl_ctx.chunk_buffer == NULL) {
                return ERR_INTERNAL;
            }
            
            BL_SendResponse(CMD_SET_CHUNK_SIZE, STATUS_SUCCESS);
            break;

        case CMD_SET_START_ADDR:
            if (packet->length < 5) return ERR_INVALID_LEN;
            
            // Extract 32-bit address
            bl_ctx.info.flash_start_addr = ((uint32_t)packet->data[0] << 24) |
                                           ((uint32_t)packet->data[1] << 16) |
                                           ((uint32_t)packet->data[2] << 8)  |
                                           ((uint32_t)packet->data[3]);

            // Some host implementations may also include an optional chunk-size field
            // after the address, but the address itself is mandatory.
            if (packet->length >= 7) {
                uint16_t start_chunk_size = (packet->data[4] << 8) | packet->data[5];
                if (start_chunk_size != 0) {
                    bl_ctx.info.chunk_size = start_chunk_size;
                }
            }
            
            BL_SendResponse(CMD_SET_START_ADDR, STATUS_SUCCESS);
            break;

        case CMD_FW_DATA:
            if (bl_ctx.chunk_buffer == NULL) return ERR_INTERNAL;
            
            // Validate data fits in allocated buffer
            uint16_t payload_len = packet->length - 1; // Length minus CMD byte
            if (payload_len > bl_ctx.info.chunk_size) return ERR_INVALID_LEN;

            // Copy data to buffer for flashing
            memcpy(bl_ctx.chunk_buffer, packet->data, payload_len);
            
            // TODO: Call Flash Writing Routine here
            // if (Flash_Write(bl_ctx.info.flash_start_addr, bl_ctx.chunk_buffer, payload_len) != 0) 
            //     return ERR_FLASH_WRITE;

            BL_SendResponse(CMD_FW_DATA, STATUS_SUCCESS);
            break;

        default:
            return ERR_CMD_UNKNOWN;
    }

    return STATUS_SUCCESS;
}

/**
 * @brief UART byte-by-byte parser state machine.
 * @param byte Incoming byte from UART.
 */
void BL_ParseByte(uint8_t byte) {
    if (!bl_ctx.is_initialized) return;

    switch (bl_ctx.state) {
        case BL_STATE_IDLE:
            if (byte == BL_HEADER) {
                rx_data_index = 0;
                bl_ctx.state = BL_STATE_GET_CRC_L;
            }
            break;

        case BL_STATE_GET_CRC_L:
            rx_packet.crc16 = (uint16_t)byte << 8; // MSB received first
            bl_ctx.state = BL_STATE_GET_CRC_H;
            break;

        case BL_STATE_GET_CRC_H:
            rx_packet.crc16 |= (uint16_t)byte;      // LSB received second
            bl_ctx.state = BL_STATE_GET_LEN_L;
            break;

        case BL_STATE_GET_LEN_L:
            rx_packet.length = (uint16_t)byte << 8; // MSB received first
            bl_ctx.state = BL_STATE_GET_LEN_H;
            break;

        case BL_STATE_GET_LEN_H:
            rx_packet.length |= (uint16_t)byte;      // LSB received second
            
            // Basic validation: length must include at least the CMD byte
            if (rx_packet.length > 0 && rx_packet.length <= 1024) { // 1024 is a safety limit
                bl_ctx.state = BL_STATE_GET_DATA;
            } else {
                bl_ctx.state = BL_STATE_IDLE;
            }
            break;

        case BL_STATE_GET_DATA:
            // If this is the first data byte, it is the Command
            if (rx_data_index == 0) {
                rx_packet.cmd = byte;
                rx_packet.data = rx_data_buffer;
            } else {
                // Store subsequent payload bytes in the temporary receive buffer
                if ((rx_data_index - 1) < sizeof(rx_data_buffer)) {
                    rx_data_buffer[rx_data_index - 1] = byte;
                }
            }

            rx_data_index++;

            // Check if we have received all bytes defined in length (CMD + Payload)
            if (rx_data_index >= rx_packet.length) {
                BL_ProcessPacket(&rx_packet);
                bl_ctx.state = BL_STATE_IDLE;
            }
            break;

        default:
            bl_ctx.state = BL_STATE_IDLE;
            break;
    }
}

/**
 * @brief Placeholder for sending a response back to the GUI.
 * @note You will need to implement the actual UART HAL call here.
 */
void BL_SendResponse(uint8_t cmd, uint8_t status) {
    // 1. Construct response packet: Header | CRC | Len | Cmd | Status
    // 2. Calculate CRC on Cmd and Status
    // 3. Transmit via UART
}