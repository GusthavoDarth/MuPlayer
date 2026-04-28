#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>



struct streaminfo
{
  uint16_t min_blockSize;
  uint16_t max_blockSize;
  uint8_t min_frameSize[3];
  uint8_t max_frameSize[3];
  uint64_t packedData;
  uint64_t MD5[2];
};


void encode_bigend_u64(uint64_t value, void* dest)
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
    if (file == NULL)
    {
        printf("Failed to open file: %s\n", filepath);
        exit(1);
    }

    uint8_t flac[4];
    fread(&flac, sizeof(uint8_t[4]), 1, file);
    printf("ID: %.4s\n",flac);

    fclose(file);
}
