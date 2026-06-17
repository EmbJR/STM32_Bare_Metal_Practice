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
#include <stdint.h>

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
    size_t capacity;      /* Maximum number of elements */
    size_t size;       /* Current number of elements (volatile for ISR safety) */
    size_t read_idx;   /* Index for reading (volatile for ISR safety) */
    size_t write_idx;  /* Index for writing (volatile for ISR safety) */
} CircularBuffer;

/* Function Prototypes */
CircularBuffer* circular_buffer_create(size_t capacity);
void circular_buffer_destroy(CircularBuffer *cb);
BufferStatus circular_buffer_write(volatile CircularBuffer *cb, volatile uint8_t data);
BufferStatus circular_buffer_read(volatile CircularBuffer *cb, volatile uint8_t *data);
int circular_buffer_is_full(CircularBuffer *cb);
int circular_buffer_is_empty(CircularBuffer *cb);
size_t circular_buffer_available(CircularBuffer *cb);

void circular_buffer_clear(volatile CircularBuffer *cb);


#endif /* CIRBUFFER_H */
