#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "stdint.h"

#define BUFFER_CAPACITY 8196

struct CircularBuffer {
    uint8_t data[BUFFER_CAPACITY];
    int write_index;
    int read_index;
    int count;
};


bool Buffer_write(struct CircularBuffer* cb, uint8_t data);

bool Buffer_read(struct CircularBuffer* cb, uint8_t *data_out);

#endif
