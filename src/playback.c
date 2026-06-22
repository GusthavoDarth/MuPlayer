#include "playback.h"
#include "raylib.h"
#include "structs.h"
#include <string.h>

void playback_init(struct Playback *playback)
{
    playback->file        = NULL;
    playback->current     = NULL;
    playback->is_playing  = false;
    playback->is_paused   = false;
    playback->volume      = 1.0f;
    playback->paused_byte = 0;
}

/* play (sempre começa do início da faixa) */

void playback_play(struct Playback *playback, struct MusicMetadata *current)
{
    playback_stop(playback);

    playback->file = fopen(current->filepath, "rb");
    if (playback->file == NULL) return;

    fseek(playback->file, current->file_offset, SEEK_SET);

    playback->current    = current;
    playback->is_playing = true;
    playback->is_paused  = false;
    playback->stream     = LoadAudioStream(current->sample_rate,
                                           current->bits_per_sample,
                                           current->num_channels);
    SetAudioStreamVolume(playback->stream, playback->volume);
    PlayAudioStream(playback->stream);
}

/* pause */

void playback_pause(struct Playback *playback)
{
    if (!playback->is_playing) return;

    playback->paused_byte = ftell(playback->file);
    PauseAudioStream(playback->stream);
    playback->is_playing = false;
    playback->is_paused  = true;
}

/* resume (retoma de onde pausou) */

void playback_resume(struct Playback *playback)
{
    if (!playback->is_paused) return;

    fseek(playback->file, playback->paused_byte, SEEK_SET);
    ResumeAudioStream(playback->stream);
    playback->is_playing = true;
    playback->is_paused  = false;
}

/* stop (libera tudo) */

void playback_stop(struct Playback *playback)
{
    if (playback->is_playing || playback->is_paused) {
        StopAudioStream(playback->stream);
        UnloadAudioStream(playback->stream);
        fclose(playback->file);
        playback->file       = NULL;
        playback->is_playing = false;
        playback->is_paused  = false;
        playback->paused_byte = 0;
    }
}

/* update (chamado todo frame) */

void playback_update(struct Playback *playback)
{
    if (!playback->is_playing) return;

    if (IsAudioStreamProcessed(playback->stream)) {
        int samples_to_read = playback->current->num_channels * STREAM_FRAMES;
        int samples = fread(playback->buffer, sizeof(int16_t),
                            samples_to_read, playback->file);

        if (samples == 0) {
            playback_stop(playback);
            return;
        }

        int frames = samples / playback->current->num_channels;
        if (frames > STREAM_FRAMES) frames = STREAM_FRAMES;
        UpdateAudioStream(playback->stream, playback->buffer, frames);
    }
}

/* progress (0.0 a 1.0) */

float playback_progress(struct Playback *playback)
{
    if (!playback->is_playing && !playback->is_paused) return 0.0f;
    if (playback->current == NULL) return 0.0f;

    long current_byte = playback->is_paused
                        ? playback->paused_byte
                        : ftell(playback->file);
    long start = playback->current->file_offset;
    long total = (long)playback->current->total_samples
               * playback->current->num_channels
               * (playback->current->bits_per_sample / 8);

    if (total <= 0) return 0.0f;
    return (float)(current_byte - start) / (float)total;
}

/* volume */

void playback_set_volume(struct Playback *playback, float volume)
{
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    playback->volume = volume;
    if (playback->is_playing || playback->is_paused)
        SetAudioStreamVolume(playback->stream, volume);
}
