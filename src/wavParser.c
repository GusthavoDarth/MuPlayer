#include "wavParser.h"
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

struct MusicMetadata wavParser(const char *filename)
{

    struct MusicMetadata metadata = {0};

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return metadata;
    }
    snprintf(metadata.filepath, 1024, "%s", filename);

    fseek(file, 12, SEEK_SET);
    struct chunk chunk;
    while (fread(&chunk, sizeof(chunk), 1, file) == 1)
    {
        if (chunk.id == 0x20746d66 ) { // "fmt "
            struct fmt_chunk fmt;
            fread(&fmt, sizeof(fmt), 1, file);
            metadata.sample_rate = fmt.sample_rate;
            metadata.num_channels = fmt.num_channels;
            metadata.bits_per_sample = fmt.bits_per_sample;
            fseek(file, chunk.size - sizeof(struct fmt_chunk) + (chunk.size % 2), SEEK_CUR);
        }
        else if (chunk.id == 0x5453494c) // "LIST"
        {
            uint32_t info;
            fread(&info, sizeof(info), 1, file);
            if (info == 0x4F464E49) // "INFO"
            {
                uint32_t bytes_read = 4;

                while (bytes_read < chunk.size)
                {
                    struct chunk sub;
                    if (fread(&sub, sizeof(sub), 1, file) != 1) break;
                    uint32_t read_len = (sub.size < 127) ? sub.size : 127;
                    if (sub.id == 0x4D414E49) { // "title"
                        fread(metadata.title, read_len, 1, file);
                        metadata.title[read_len] = '\0';
                        fseek(file, sub.size - read_len, SEEK_CUR);
                    }
                    else if (sub.id == 0x54524149) { // "artist"
                        fread(metadata.artist, read_len, 1, file);
                        metadata.artist[read_len] = '\0';
                        fseek(file, sub.size - read_len, SEEK_CUR);
                    }
                    else
                    {
                        fseek(file, sub.size, SEEK_CUR);
                    }
                    uint32_t padding = sub.size % 2;
                    if (padding) fseek(file, padding, SEEK_CUR);
                    bytes_read += 8 + sub.size + padding;
                }
            }
            else{
                fseek(file, (chunk.size - 4) + chunk.size % 2, SEEK_CUR);
            }
        }
        else if (chunk.id == 0x61746164) // "data"
        {
            metadata.file_offset = ftell(file);
            metadata.total_samples = chunk.size / (metadata.num_channels * (metadata.bits_per_sample / 8));
            fseek(file, chunk.size + (chunk.size % 2), SEEK_CUR);
        }
        else
        {
            fseek(file, chunk.size + (chunk.size % 2), SEEK_CUR);
        }
    }
    fclose(file);

    return metadata;
}
