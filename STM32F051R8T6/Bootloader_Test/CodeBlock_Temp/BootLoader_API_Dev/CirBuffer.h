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

#include <stddef.h>

/* Return codes for buffer operations */
typedef enum {
    BUFFER_OK = 0,          /* Operation successful */
    BUFFER_OVERFLOW = -1,   /* Buffer is full - cannot write */
    BUFFER_UNDERFLOW = -2,  /* Buffer is empty - cannot read */
    BUFFER_NULL = -3,       /* NULL buffer pointer provided */
    BUFFER_PARAM_ERROR = -4  /* Invalid parameters */
} BufferStatus;

/* Circular Buffer Structure */
typedef struct {
    uint8_t *buffer;      /* Pointer to the data buffer */
    size_t capacity;  /* Maximum number of elements */
    size_t size;       /* Current number of elements */
    size_t read_idx;   /* Index for reading */
    size_t write_idx;  /* Index for writing */
} CircularBuffer;

/* Function Prototypes */
CircularBuffer* circular_buffer_create(size_t capacity);
void circular_buffer_destroy(CircularBuffer *cb);
BufferStatus circular_buffer_write(CircularBuffer *cb, char data);
BufferStatus circular_buffer_read(CircularBuffer *cb, char *data);
void circular_buffer_clear(CircularBuffer *cb);
int circular_buffer_is_full(const CircularBuffer *cb);
int circular_buffer_is_empty(const CircularBuffer *cb);
size_t circular_buffer_available(const CircularBuffer *cb);
const char* circular_buffer_error_msg(BufferStatus status);

#endif /* CIRBUFFER_H */
