#ifndef PLAYBACK_H
#define PLAYBACK_H

#include "structs.h"
#include <raylib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define STREAM_FRAMES 4096
#define STREAM_BUFFER_SIZE (STREAM_FRAMES * 4)

struct Playback {
    FILE*                file;
    AudioStream          stream;
    struct MusicMetadata* current;
    bool                 is_playing;   // stream ativo e rodando
    bool                 is_paused;    // pausado (stream alocado, arquivo aberto, posição salva)
    float                volume;       // 0.0 – 1.0
    long                 paused_byte;  // posição no arquivo quando pausou
    int16_t buffer[STREAM_BUFFER_SIZE];
};

void  playback_init(struct Playback* playback);
void  playback_play(struct Playback* playback, struct MusicMetadata* current);
void  playback_pause(struct Playback* playback);
void  playback_resume(struct Playback* playback);
void  playback_stop(struct Playback* playback);
void  playback_update(struct Playback* playback);
float playback_progress(struct Playback* playback);
void  playback_set_volume(struct Playback* playback, float volume);

#endif
