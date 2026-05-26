#ifndef PLAYBACK_H
#define PLAYBACK_H

#include "structs.h"
#include <raylib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define STREAM_FRAMES 4096

struct Playback {
    AudioStream stream;
    FILE* file;
    struct MusicMetadata* current;
    bool is_playing;
    float volume;
    int16_t buffer[STREAM_FRAMES * 2];
};

void playback_init(struct Playback* playback);
void playback_stop(struct Playback* playback);
void playback_update(struct Playback* playback);

void playback_play(struct Playback* playback, struct MusicMetadata* current);
void playback_pause(struct Playback* playback);


#endif
