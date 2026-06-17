/**
 * Circular Buffer Management in C
 *
 * This implementation provides a thread-safe circular buffer with
 * proper overflow and underflow detection and appropriate return values.
 */

 /**
 *------------------ Usage of circular buffer --------------------
---------------------------------------------------------------------
1> Define the handles for Circular Buffers for TX and RX
    static CircularBuffer *txBuffer = NULL;
    static CircularBuffer *rxBuffer = NULL;
---------------------------------------------------------------------
2> Create and allcate the memory to the handles
    txBuffer = circular_buffer_create(TX_BUFFER_SIZE);

    //Create RX circular buffer
    if (txBuffer == NULL)
    {
        // Handle error - buffer creation failed
        while (1);
    }

    //Create RX circular buffer
    rxBuffer = circular_buffer_create(RX_BUFFER_SIZE);
    if (rxBuffer == NULL)
    {
        //Handle error - buffer creation failed
        while (1);
    }
---------------------------------------------------------------------
3> Write the circular buffer.
    uint8_t data = 0xAA;
    //Check if buffer has space using API
    if (circular_buffer_is_full(txBuffer))
    {
        return false;  //Buffer full
    }

    //Write data to circular buffer using API
    if (circular_buffer_write(txBuffer, (char)data) != BUFFER_OK)
    {
        return false;  //Write failed
    }
---------------------------------------------------------------------
4> Read the circular buffer.
    uint8_t data = 0;
    if(circular_buffer_available(rxBuffer))
    {
        //Check if buffer has data using API
        if (circular_buffer_is_empty(rxBuffer))
        {
            return false;  // Buffer empty
        }

        // Read data from circular buffer using API
        if (circular_buffer_read(rxBuffer, (char *)data) != BUFFER_OK)
        {
            return false;  // Read failed
        }
    }
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CirBuffer.h"


/**
 * Create and initialize a circular buffer
 *
 * @param capacity - Maximum number of elements the buffer can hold
 * @return Pointer to the created buffer, or NULL on failure
 */
CircularBuffer* circular_buffer_create(size_t capacity) {
    if (capacity == 0) {
        return NULL;
    }

    CircularBuffer *cb = (CircularBuffer*)malloc(sizeof(CircularBuffer));
    if (cb == NULL) {
        return NULL;
    }

    cb->buffer = (char*)malloc(capacity * sizeof(char));
    if (cb->buffer == NULL) {
        free(cb);
        return NULL;
    }

    cb->capacity = capacity;
    cb->size = 0;
    cb->read_idx = 0;
    cb->write_idx = 0;

    return cb;
}

/**
 * Destroy a circular buffer and free all allocated memory
 *
 * @param cb - Pointer to the circular buffer to destroy
 */
void circular_buffer_destroy(CircularBuffer *cb) {
    if (cb != NULL) {
        if (cb->buffer != NULL) {
            free(cb->buffer);
        }
        free(cb);
    }
}


/**
 * Clear all elements from the buffer
 *
 * @param cb - Pointer to the circular buffer
 */
void circular_buffer_clear(CircularBuffer *cb) {
    if (cb != NULL) {
        cb->size = 0;
        cb->read_idx = 0;
        cb->write_idx = 0;
    }
}

/**
 * Check if the buffer is full
 *
 * @param cb - Pointer to the circular buffer
 * @return 1 if full, 0 if not full, -1 if NULL buffer
 */
int circular_buffer_is_full(const CircularBuffer *cb) {
    if (cb == NULL) {
        return -1;
    }
    return (cb->size == cb->capacity);
}

/**
 * Check if the buffer is empty
 *
 * @param cb - Pointer to the circular buffer
 * @return 1 if empty, 0 if not empty, -1 if NULL buffer
 */
int circular_buffer_is_empty(const CircularBuffer *cb) {
    if (cb == NULL) {
        return -1;
    }
    return (cb->size == 0);
}


/**
 * Get the number of available elements in the buffer
 *
 * @param cb - Pointer to the circular buffer
 * @return Number of elements available for reading, 0 if NULL buffer
 */
size_t circular_buffer_available(const CircularBuffer *cb) {
    if (cb == NULL) {
        return 0;
    }
    return cb->size;
}

/**
 * Write a single byte to the circular buffer
 *
 * @param cb   - Pointer to the circular buffer
 * @param data - Data byte to write
 * @return BUFFER_OK on success, BUFFER_OVERFLOW if buffer is full,
 *         BUFFER_NULL if NULL buffer provided
 */
BufferStatus circular_buffer_write(CircularBuffer *cb, char data) {
    /* Validate buffer pointer */
    if (cb == NULL) {
        return BUFFER_NULL;
    }

    /* Check for overflow - buffer is full */
    if (cb->size >= cb->capacity) {
        return BUFFER_OVERFLOW;
    }

    /* Write data at write position */
    cb->buffer[cb->write_idx] = data;

    /* Advance write index (wrap around) */
    cb->write_idx = (cb->write_idx + 1) % cb->capacity;

    /* Increment size */
    cb->size++;

    return BUFFER_OK;
}

/**
 * Read a single byte from the circular buffer
 *
 * @param cb   - Pointer to the circular buffer
 * @param data - Pointer to store the read byte
 * @return BUFFER_OK on success, BUFFER_UNDERFLOW if buffer is empty,
 *         BUFFER_NULL if NULL buffer provided, BUFFER_PARAM_ERROR if data is NULL
 */
BufferStatus circular_buffer_read(CircularBuffer *cb, char *data) {
    /* Validate buffer pointer */
    if (cb == NULL) {
        return BUFFER_NULL;
    }

    /* Validate data pointer */
    if (data == NULL) {
        return BUFFER_PARAM_ERROR;
    }

    /* Check for underflow - buffer is empty */
    if (cb->size == 0) {
        return BUFFER_UNDERFLOW;
    }

    /* Read data from read position */
    *data = cb->buffer[cb->read_idx];

    /* Advance read index (wrap around) */
    cb->read_idx = (cb->read_idx + 1) % cb->capacity;

    /* Decrement size */
    cb->size--;

    return BUFFER_OK;
}

/**
 * Get error message for buffer status code
 *
 * @param status - Buffer status code
 * @return Human-readable error message
 */
const char* circular_buffer_error_msg(BufferStatus status) {
    switch (status) {
        case BUFFER_OK:          return "Success";
        case BUFFER_OVERFLOW:    return "Buffer overflow - cannot write to full buffer";
        case BUFFER_UNDERFLOW:   return "Buffer underflow - cannot read from empty buffer";
        case BUFFER_NULL:        return "NULL buffer pointer provided";
        case BUFFER_PARAM_ERROR: return "Invalid parameter provided";
        default:                 return "Unknown error";
    }
}

/* ==================== MAIN FUNCTION FOR TESTING ==================== */
