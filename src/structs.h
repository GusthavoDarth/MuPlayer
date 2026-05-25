#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

struct MusicMetadata {
    char title[256];
    char artist[256];
    char filepath[1024];
    uint16_t bits_per_sample;
    uint32_t total_samples;
    uint32_t sample_rate;
    uint32_t num_channels;
    long file_offset;
};

struct chunk {
    uint32_t id;
    uint32_t size;
};

struct fmt_chunk {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

#endif
