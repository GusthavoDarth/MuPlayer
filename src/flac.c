#include "flac.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void encode_bigend_u64(uint64_t value, void *dest)
{
    value =
        ((value & 0xFF00000000000000u) >> 56u) |
        ((value & 0x00FF000000000000u) >> 40u) |
        ((value & 0x0000FF0000000000u) >> 24u) |
        ((value & 0x000000FF00000000u) >>  8u) |
        ((value & 0x00000000FF000000u) <<  8u) |
        ((value & 0x0000000000FF0000u) << 24u) |
        ((value & 0x000000000000FF00u) << 40u) |
        ((value & 0x00000000000000FFu) << 56u);
    memcpy(dest, &value, sizeof(uint64_t));
}

void read_flac(char *filepath)
{
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filepath);
        return;
    }
    uint8_t magic[4];
    fread(&magic, sizeof(magic), 1, file);
    printf("FLAC magic: %.4s\n", magic);
    fclose(file);
}
