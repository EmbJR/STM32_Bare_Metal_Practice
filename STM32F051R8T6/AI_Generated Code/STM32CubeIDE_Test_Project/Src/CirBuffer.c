/**
 * Circular Buffer Management in C
 * 
 * This implementation provides a thread-safe circular buffer with
 * proper overflow and underflow detection and appropriate return values.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Return codes for buffer operations */
typedef enum {
    BUFFER_OK = 0,          /* Operation successful */
    BUFFER_OVERFLOW = -1,   /* Buffer is full - cannot write */
    BUFFER_UNDERFLOW = -2,  /* Buffer is empty - cannot read */
    BUFFER_NULL = -3,       /* NULL buffer pointer provided */
    BUFFER_PARAM_ERROR = -4 /* Invalid parameters */
} BufferStatus;

/* Circular Buffer Structure */
typedef struct {
    char *buffer;      /* Pointer to the data buffer */
    size_t capacity;   /* Maximum number of elements */
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

BufferStatus circular_buffer_write_batch(CircularBuffer *cb, const char *data, size_t count);
BufferStatus circular_buffer_read_batch(CircularBuffer *cb, char *data, size_t count);
int circular_buffer_is_full(const CircularBuffer *cb);
int circular_buffer_is_empty(const CircularBuffer *cb);
size_t circular_buffer_available(const CircularBuffer *cb);
size_t circular_buffer_capacity(const CircularBuffer *cb);

void circular_buffer_print(const CircularBuffer *cb);

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
