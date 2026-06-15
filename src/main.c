#include "playback.h"
#include "raylib.h"
#include "wavParser.h"
#include "navfolder.h"
#include <stdio.h>
#include "structs.h"

int main(void) {
    const int   MIN_WIDTH           = 1024;
    const int   MIN_HEIGHT          = 600;
    const int   PADDING             = 10;
    const int   MAX_LEFT_SIZE       = 320;
    const int   MIN_LEFT_SIZE       = 256;
    const int   CONTROL_PANEL_HEIGHT = 100;
    const int   TOP_BAR_HEIGHT      = 25 + PADDING;
    const int   ITEM_HEIGHT         = 22;
    const int   SCROLL_WIDTH        = 10;
    const int   TEXT_SIZE           = 20;
    const int   btnHeight           = 40;
    const int   btnWidth            = 40;

    /* Volume slider geometry (posição relativa ao controlPanel — calculada no loop) */
    //const int   SLIDER_W = 80;
    //const int   SLIDER_H = 8;

    struct MusicMetadata lib[MAX_MUSIC_COUNT];
    struct Playback playback;

    int   count               = 0;
    int   scrollOffset        = 0;
    int   dragStartScrollOffset = 0;
    int   dragStartMouseY     = 0;
    float progress            = 0.0f;
    int   currentMusic        = 0;
    bool  isDraggingScrollbar = false;
    bool  isDraggingSlider    = false;

    nav_folder("Test_music_files", 0, lib, &count);
    nav_folder("Musics", 0, lib, &count);

    playback_init(&playback);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(MIN_WIDTH, MIN_HEIGHT, "MuPlayer");
    SetTargetFPS(60);
    SetWindowMinSize(MIN_WIDTH, MIN_HEIGHT);
    SetAudioStreamBufferSizeDefault(STREAM_FRAMES);
    InitAudioDevice();

    while (!WindowShouldClose())
    {
        int     screen_w  = GetScreenWidth();
        int     screen_h  = GetScreenHeight();
        Vector2 mousePos  = GetMousePosition();

        /* --- layouts --- */
        int leftWidth = (screen_w + screen_h) / 8;
        if (leftWidth < MIN_LEFT_SIZE) leftWidth = MIN_LEFT_SIZE;
        if (leftWidth > MAX_LEFT_SIZE) leftWidth = MAX_LEFT_SIZE;

        int rightHeight = screen_h - (CONTROL_PANEL_HEIGHT + TOP_BAR_HEIGHT + (PADDING * 3));
        int rigthWidth  = screen_w - (leftWidth + (PADDING * 2));

        Rectangle topPanel      = { PADDING, PADDING, screen_w - (2*PADDING), TOP_BAR_HEIGHT - PADDING };
        Rectangle albumPanel    = { PADDING, TOP_BAR_HEIGHT + PADDING, leftWidth, leftWidth };
        Rectangle infoPanel     = { PADDING, albumPanel.y + albumPanel.height + PADDING,
                                    leftWidth,
                                    screen_h - (albumPanel.height + CONTROL_PANEL_HEIGHT + TOP_BAR_HEIGHT + (PADDING * 4)) };
        Rectangle controlPanel  = { PADDING, infoPanel.y + infoPanel.height + PADDING,
                                    screen_w - (2*PADDING), CONTROL_PANEL_HEIGHT };
        Rectangle filaPanel     = { screen_w - rigthWidth, TOP_BAR_HEIGHT + PADDING,
                                    ((float)rigthWidth/2) - PADDING, rightHeight };
        Rectangle bibliotecaPanel = { filaPanel.x + filaPanel.width + PADDING, TOP_BAR_HEIGHT + PADDING,
                                      ((float)rigthWidth/2) - PADDING, rightHeight };

        int controlPanelXCenter = albumPanel.x + (albumPanel.width / 2);
        int controlPanelYCenter = controlPanel.y + (controlPanel.height / 2);

        /* --- botões de controle --- */
        Vector2 circleCenter = { controlPanelXCenter, controlPanelYCenter };

        Rectangle PlayButton = {
            controlPanelXCenter - (btnWidth / 2.0f),
            controlPanelYCenter - (btnHeight / 2.0f),
            btnWidth, btnHeight
        };
        Rectangle ForwardButton = {
            controlPanelXCenter + (btnWidth / 2.0f) + PADDING,
            controlPanelYCenter - (btnHeight / 2.0f),
            btnWidth, btnHeight
        };
        Rectangle BackwardButton = {
            controlPanelXCenter - (btnWidth * 1.5f) - PADDING,
            controlPanelYCenter - (btnHeight / 2.0f),
            btnWidth, btnHeight
        };

        /* --- progress bar --- */
        Rectangle ProgressBar = {
            controlPanel.x + albumPanel.width + (rigthWidth*0.125f) + (2*PADDING),
            controlPanel.y - 5 + (controlPanel.height / 2),
            (rigthWidth*0.75f) - (2*PADDING), 10
        };
        Rectangle ProgressBarFill = { ProgressBar.x, ProgressBar.y,
                                      ProgressBar.width * progress, 10 };

        /* --- slider de volume (canto direito do controlPanel) --- */
        int volHeight = 70;
        Rectangle VolumeTrack = {
            controlPanel.x + controlPanel.width - (4*PADDING),
            controlPanelYCenter - volHeight / 2,
            14, volHeight
        };
        Rectangle VolumeFill = {
            VolumeTrack.x,
            VolumeTrack.y + (int)(volHeight * (1.0f - playback.volume)),
            14,
            (int)(volHeight * playback.volume)
        };
        Rectangle VolumeThumb = {
            VolumeTrack.x - 3,
            VolumeTrack.y + (int)(volHeight * (1.0f - playback.volume)) - 5,
            20, 10
        };

        /* --- scrollbar biblioteca --- */
        Rectangle scrollbar = {
            bibliotecaPanel.x + bibliotecaPanel.width - SCROLL_WIDTH,
            bibliotecaPanel.y + (scrollOffset / (float)(count * ITEM_HEIGHT)) * bibliotecaPanel.height,
            SCROLL_WIDTH,
            (bibliotecaPanel.height) * (bibliotecaPanel.height + ITEM_HEIGHT) / (float)(count * ITEM_HEIGHT)
        };

        /* ====== INPUT ====== */

        /* scroll lista */
        if (count * ITEM_HEIGHT > (int)bibliotecaPanel.height + ITEM_HEIGHT) {
            int maxScroll = (count * ITEM_HEIGHT) - ((int)bibliotecaPanel.height + ITEM_HEIGHT);
            if (CheckCollisionPointRec(mousePos, bibliotecaPanel)) {
                scrollOffset -= (int)(GetMouseWheelMove() * ITEM_HEIGHT);
                if (scrollOffset < 0) scrollOffset = 0;
                if (scrollOffset > maxScroll) scrollOffset = maxScroll;
            }
        } else { scrollOffset = 0; }

        /* drag scrollbar */
        if (CheckCollisionPointRec(mousePos, scrollbar) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            isDraggingScrollbar = true;
            dragStartScrollOffset = scrollOffset;
            dragStartMouseY = (int)mousePos.y;
        } else if (isDraggingScrollbar && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            int maxScroll = (count * ITEM_HEIGHT) - ((int)bibliotecaPanel.height + ITEM_HEIGHT);
            float ratio = maxScroll / bibliotecaPanel.height;
            scrollOffset = dragStartScrollOffset + (int)((mousePos.y - dragStartMouseY) * ratio * 2);
            if (scrollOffset < 0) scrollOffset = 0;
            if (scrollOffset > maxScroll) scrollOffset = maxScroll;
        } else if (isDraggingScrollbar && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isDraggingScrollbar = false;
        }

        /* seek na progress bar */
        if (CheckCollisionPointRec(mousePos, ProgressBar) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
            && (playback.is_playing || playback.is_paused))
        {
            progress = (mousePos.x - ProgressBar.x) / ProgressBar.width;
            long total_data_size = (long)playback.current->total_samples
                                 * playback.current->num_channels
                                 * (playback.current->bits_per_sample / 8);
            long seek_byte = playback.current->file_offset + (long)(progress * total_data_size);
            int  frame_size = playback.current->num_channels * (playback.current->bits_per_sample / 8);
            seek_byte -= (seek_byte - playback.current->file_offset) % frame_size;
            fseek(playback.file, seek_byte, SEEK_SET);
            if (playback.is_paused) {
                playback.paused_byte = seek_byte;
            } else {
                StopAudioStream(playback.stream);
                PlayAudioStream(playback.stream);
            }
        }

        /* slider de volume */
        if ((CheckCollisionPointRec(mousePos, VolumeTrack) ||
             CheckCollisionPointRec(mousePos, VolumeThumb)) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            isDraggingSlider = true;
        }
        if (isDraggingSlider && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            float v = 1.0f - (mousePos.y - VolumeTrack.y) / VolumeTrack.height;
            playback_set_volume(&playback, v);
        }
        if (isDraggingSlider && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isDraggingSlider = false;
        }

        /* ----- botão Play/Pause ----- */
        Color PlaybuttonColor    = DARKGRAY;
        Color ForwardButtonColor = DARKGRAY;
        Color BackwardButtonColor = DARKGRAY;

        if (CheckCollisionPointRec(mousePos, PlayButton)) {
            PlaybuttonColor = GRAY;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (playback.is_playing) {
                    playback_pause(&playback);
                } else if (playback.is_paused) {
                    playback_resume(&playback);
                } else {
                    playback_play(&playback, &lib[currentMusic]);
                }
            }
        }

        /* ----- botão Forward (próxima) ----- */
        if (CheckCollisionPointRec(mousePos, ForwardButton)) {
            ForwardButtonColor = GRAY;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (count > 0) {
                    currentMusic = (currentMusic + 1) % count;
                    playback_play(&playback, &lib[currentMusic]);
                }
            }
        }

        /* ----- botão Backward (anterior) ----- */
        if (CheckCollisionPointRec(mousePos, BackwardButton)) {
            BackwardButtonColor = GRAY;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (count > 0) {
                    currentMusic = (currentMusic - 1 + count) % count;
                    playback_play(&playback, &lib[currentMusic]);
                }
            }
        }

        /* auto-avanço quando música termina */
        if (!playback.is_playing && !playback.is_paused && progress >= 0.99f) {
            currentMusic = (currentMusic + 1) % count;
            playback_play(&playback, &lib[currentMusic]);
            progress = 0.0f;
        }

        /* atualiza progresso */
        progress = playback_progress(&playback);
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        playback_update(&playback);

        /* ====== DRAW ====== */
        BeginDrawing();
        ClearBackground(RAYWHITE);

        /* top panel */
        DrawRectangleLinesEx(topPanel, 2, LIGHTGRAY);
        //DrawText("MuPlayer", PADDING + 5, PADDING + 2, TEXT_SIZE - 4, DARKGRAY);

        /* album panel */
        DrawRectangleLinesEx(albumPanel, 2, DARKGRAY);
        DrawText("Album", albumPanel.x + 10, albumPanel.y + 10, TEXT_SIZE, GRAY);

        /* info panel */
        DrawRectangleLinesEx(infoPanel, 2, DARKGRAY);
        BeginScissorMode(infoPanel.x, infoPanel.y, infoPanel.width, infoPanel.height);
        DrawText(lib[currentMusic].title,  infoPanel.x + 10, infoPanel.y + 10, TEXT_SIZE, DARKGRAY);
        DrawText(lib[currentMusic].artist, infoPanel.x + 10, infoPanel.y + 35, TEXT_SIZE - 4, GRAY);
        EndScissorMode();

        /* control panel */
        DrawRectangleLinesEx(controlPanel, 2, DARKGRAY);

        /* play/pause ícone */
        DrawCircleV(circleCenter, (float)btnWidth / 2, PlaybuttonColor);
        if (playback.is_playing) {
            /* ícone pause: duas barras */
            DrawRectangle((int)circleCenter.x - 9, (int)circleCenter.y - 8, 6, 16, WHITE);
            DrawRectangle((int)circleCenter.x + 3,  (int)circleCenter.y - 8, 6, 16, WHITE);
        } else {
            /* ícone play: triângulo */
            DrawTriangle(
                (Vector2){ circleCenter.x - 6, circleCenter.y - 10 },
                (Vector2){ circleCenter.x - 6, circleCenter.y + 10 },
                (Vector2){ circleCenter.x + 12, circleCenter.y },
                WHITE
            );
        }

        /* forward / backward */
        DrawRectangleRec(ForwardButton, ForwardButtonColor);
        DrawText(">>", (int)ForwardButton.x + 6, (int)ForwardButton.y + 10, TEXT_SIZE - 4, WHITE);
        DrawRectangleRec(BackwardButton, BackwardButtonColor);
        DrawText("<<", (int)BackwardButton.x + 4, (int)BackwardButton.y + 10, TEXT_SIZE - 4, WHITE);

        /* progress bar */
        DrawRectangleRounded(ProgressBar,     1, 10, LIGHTGRAY);
        DrawRectangleRounded(ProgressBarFill, 1, 10, RED);

        /* volume slider */
        //DrawText("vol", (int)VolumeTrack.x, (int)VolumeTrack.y - 14, TEXT_SIZE - 8, GRAY);
        DrawRectangleRec(VolumeTrack, LIGHTGRAY);
        DrawRectangleRec(VolumeFill, RED);
        DrawRectangleRec(VolumeThumb, DARKGRAY);

        /* fila panel */
        DrawRectangleLinesEx(filaPanel, 2, DARKGRAY);
        DrawText("Fila", filaPanel.x + 10, filaPanel.y + 10, TEXT_SIZE, GRAY);

        /* biblioteca panel */
        BeginScissorMode(bibliotecaPanel.x, bibliotecaPanel.y,
                         bibliotecaPanel.width, bibliotecaPanel.height);
        for (int i = 0; i < count; i++) {
            int yPos = bibliotecaPanel.y + (i * ITEM_HEIGHT) - scrollOffset;
            Rectangle item = { bibliotecaPanel.x, yPos,
                               bibliotecaPanel.width - SCROLL_WIDTH, ITEM_HEIGHT };
            bool hovered = CheckCollisionPointRec(mousePos, item);

            if (i == currentMusic || hovered) {
                DrawRectangleRec(item, i == currentMusic ? DARKGRAY : (Color){200,200,200,255});
            }
            DrawText(lib[i].title,
                     bibliotecaPanel.x + (hovered || i == currentMusic ? 12 : 5),
                     yPos, TEXT_SIZE - 2,
                     (i == currentMusic || hovered) ? RAYWHITE : GRAY);

            if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                currentMusic = i;
                playback_play(&playback, &lib[currentMusic]);
            }
        }
        EndScissorMode();

        if (count * ITEM_HEIGHT > (int)bibliotecaPanel.height + ITEM_HEIGHT)
            DrawRectangleRec(scrollbar, DARKGRAY);
        DrawRectangleLinesEx(bibliotecaPanel, 2, DARKGRAY);

        EndDrawing();
    }

    playback_stop(&playback);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
