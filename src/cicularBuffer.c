
#include "stdint.h"
#include "cicularBuffer.h"


bool Buffer_write(struct CircularBuffer* cb, uint8_t data) {
    if (cb->count == BUFFER_CAPACITY) {
        return false;
    }
    cb->data[cb->write_index] = data;
    cb->write_index = (cb->write_index + 1) % BUFFER_CAPACITY;
    cb->count++;
    return true;
}

bool Buffer_read(struct CircularBuffer* cb, uint8_t *data_out){
    if (cb->count == 0){
        return false;
    }
    *data_out = cb->data[cb->read_index];
    cb->read_index = (cb->read_index + 1) % BUFFER_CAPACITY;
    cb->count--;
    return true;
}
