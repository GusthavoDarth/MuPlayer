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
   printf("file_offset: %ld, total_samples: %u, channels: %u, bps: %u\n",
          current->file_offset, current->total_samples,
          current->num_channels, current->bits_per_sample);
   playback->current = current;
   playback->is_playing = true;
   playback->stream = LoadAudioStream(current->sample_rate, current->bits_per_sample, current->num_channels);

   PlayAudioStream(playback->stream);
}

void playback_pause(struct Playback* playback) {
    if (playback->is_playing) {
        PauseAudioStream(playback->stream);
        playback->is_playing = false;
    }
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
        // Raylib handles the state natively; no need for a custom flag
        if (IsAudioStreamProcessed(playback->stream)) {
            int samples_to_read = playback->current->num_channels * STREAM_FRAMES;
            int samples = fread(playback->buffer, sizeof(int16_t), samples_to_read, playback->file);
            int frames = samples / playback->current->num_channels;

            if (samples == 0) {
                playback_stop(playback);
                return;
            }

            if (frames > STREAM_FRAMES) frames = STREAM_FRAMES;
            UpdateAudioStream(playback->stream, playback->buffer, frames);
        }
    }
}

float playback_progress(struct Playback* playback) {
    if (!playback->is_playing || playback->current == NULL) return 0.0f;

    long current_byte = ftell(playback->file);
    long start = playback->current->file_offset;
    long total = playback->current->total_samples
               * playback->current->num_channels
               * (playback->current->bits_per_sample / 8);

    return (float)(current_byte - start) / (float)total;
}
