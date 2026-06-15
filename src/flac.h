#ifndef FLAC_H
#define FLAC_H

#include <stdint.h>

struct streaminfo {
    uint16_t min_blockSize;
    uint16_t max_blockSize;
    uint8_t  min_frameSize[3];
    uint8_t  max_frameSize[3];
    uint64_t packedData;
    uint64_t MD5[2];
};

void encode_bigend_u64(uint64_t value, void *dest);
void read_flac(char *filepath);

#endif
