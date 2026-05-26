#include "playback.h"
#include "raylib.h"
#include "structs.h"

void playback_init(struct Playback *playback)
{
    playback->file = NULL;
    playback->current = NULL;
    playback->is_playing = false;
    playback->volume = 1.0f;
}

void playback_play(struct Playback* playback, struct MusicMetadata* current)
{
   playback_stop(playback);
   if((playback->file = fopen(current->filepath, "rb")) == NULL) { return; }

   fseek(playback->file, current->file_offset, SEEK_SET);
   playback->current = current;
   playback->is_playing = true;
   playback->stream = LoadAudioStream(current->sample_rate, current->bits_per_sample, current->num_channels);

   PlayAudioStream(playback->stream);
}

void playback_stop(struct Playback* playback)
{
    if (playback->is_playing) {
        StopAudioStream(playback->stream);
        playback->is_playing = false;
        UnloadAudioStream(playback->stream);
        fclose(playback->file);
        playback->file = NULL;
    }
}



void playback_update(struct Playback *playback)
{
    if (playback->is_playing) {
        if (IsAudioStreamProcessed(playback->stream)) {
            int samples = fread(playback->buffer, sizeof(int16_t), playback->current->num_channels * STREAM_FRAMES, playback->file);
            int frames = samples / playback->current->num_channels;
            if (samples == 0) {
                playback_stop(playback);
                return;
            }
            UpdateAudioStream(playback->stream, playback->buffer, frames);
        }
    }
}
