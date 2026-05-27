# MuPlayer

A music player built from scratch in C, using [Raylib](https://www.raylib.com/) for the graphical interface and audio streaming. The main goal of the project is to implement all the low-level logic manually — from binary file parsing to audio playback pipeline — without relying on high-level media libraries.

## Features

- **Custom WAV binary parser** — reads the RIFF/WAVE chunk structure directly, extracting `fmt` (sample rate, bit depth, channels), `LIST/INFO` metadata tags (title, artist) and the `data` chunk offset for streaming
- **Streaming playback** — audio is read from disk in 4096-frame blocks via `Raylib AudioStream`, avoiding loading the entire file into memory
- **Recursive directory traversal** — scans subdirectories using POSIX `dirent` to build the music library automatically
- **Circular buffer** — lock-free byte-level ring buffer implementation, designed for the future audio pipeline
- **Responsive UI** — layout built entirely with Raylib primitives, adapting panels and controls to window resizing; includes library list with scroll, queue panel, info panel and transport controls

## Architecture

```
src/
├── MainRay.c        # Entry point — UI loop and input handling (Raylib)
├── main.c           # CLI entry point for testing
├── wavParser.c/h    # RIFF/WAVE binary parser
├── playback.c/h     # Audio streaming engine (Raylib AudioStream)
├── navfolder.c/h    # Recursive directory scanner (POSIX dirent)
├── cicularBuffer.c/h # Ring buffer implementation
├── flac.h           # FLAC structure definitions (in progress)
└── structs.h        # Shared data structures (MusicMetadata, chunk, fmt_chunk)
```

## Building

**Requirements:** GCC, Make, Raylib installed on the system.

```bash
git clone https://github.com/GusthavoDarth/MuPlayer
cd MuPlayer
make
./MuPlayer
```

The player scans the `Test_music_files/` directory at startup. Place `.wav` files there or modify the path in `MainRay.c`.

## Status

| Format | Parsing | Playback |
|--------|---------|----------|
| WAV    | ✅ Complete | ✅ Functional |
| FLAC   | 🔄 In progress | ❌ Pending |
| MP3    | ❌ Pending | ❌ Pending |

## Roadmap

- [ ] Connect play/pause/forward buttons to the playback engine
- [ ] Integrate circular buffer into the audio streaming pipeline
- [ ] Implement FLAC decoder (frame parsing, prediction filters, Rice coding)
- [ ] Add MP3 parser (ID3 tags + frame decoding)
- [ ] Volume control (sample-level float multiplication)
- [ ] **ESP32 port** — replace POSIX file I/O with SD card libraries, map audio output to I2S protocol

## Why this project

Building a media player without FFmpeg or similar libraries forces a direct understanding of how audio data is actually stored and streamed. Parsing RIFF chunks manually, calculating file offsets for streaming, and managing an audio buffer at the sample level are problems that map directly to embedded systems work — which is where this project is headed with the planned ESP32 port.
