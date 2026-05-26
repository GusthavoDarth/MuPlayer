#include "playback.h"
#include "raylib.h"
#include "wavParser.h"
#include "navfolder.h"
#include <stdio.h>
#include "structs.h"
#include "playback.h"


int main(void) {
    const int MIN_WIDTH = 1024;
    const int MIN_HEIGHT = 600;


    const int PADDING = 20;
    const int MAX_LEFT_SIZE = 320;
    const int MIN_LEFT_SIZE = 256;
    const int CONTROL_PANEL_HEIGHT = 100;
    const int TOP_BAR_HEIGHT = 25;
    const int ITEM_HEIGHT = 22;

    struct MusicMetadata lib[MAX_MUSIC_COUNT];
    struct Playback playback;

    int count = 0;
    int scrollOffset = 0;
    int currentSample = 0;
    float progress = 0.0f;

    int currentMusic = 0;

    float volume = 0.5f;

    nav_folder("Test_music_files", 0, lib, &count);
    playback_init(&playback);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    //SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(MIN_WIDTH, MIN_HEIGHT, "MuPlayer - Responsive UI");
    SetTargetFPS(60);
    SetWindowMinSize(MIN_WIDTH, MIN_HEIGHT);

    InitAudioDevice();

    while (!WindowShouldClose())
    {

        int screen_w = GetScreenWidth();
        int screen_h = GetScreenHeight();
        Vector2 mousePos = GetMousePosition();

        // base panels layouts
        int leftWidth = (screen_w + screen_h) / 8;

        if (leftWidth < MIN_LEFT_SIZE) leftWidth = MIN_LEFT_SIZE;
        if (leftWidth > MAX_LEFT_SIZE) leftWidth = MAX_LEFT_SIZE;


        Rectangle albumPanel = { PADDING, TOP_BAR_HEIGHT + PADDING, leftWidth, leftWidth};

        Rectangle infoPanel = { PADDING, albumPanel.y + albumPanel.height + PADDING, leftWidth, screen_h - (albumPanel.height + CONTROL_PANEL_HEIGHT + TOP_BAR_HEIGHT + (PADDING * 4))};

        Rectangle controlPanel = { PADDING, infoPanel.y + infoPanel.height + PADDING, screen_w - (2*PADDING), CONTROL_PANEL_HEIGHT};


        int rightHeight = screen_h - ( CONTROL_PANEL_HEIGHT + TOP_BAR_HEIGHT + (PADDING * 3));
        int rigthWidth = screen_w - ( leftWidth + (PADDING*2));

        Rectangle filaPanel = {screen_w - rigthWidth, TOP_BAR_HEIGHT + PADDING, (rigthWidth/2) - PADDING, rightHeight};
        Rectangle bibliotecaPanel = {filaPanel.x + filaPanel.width + PADDING, TOP_BAR_HEIGHT + PADDING, (rigthWidth/2) - PADDING, rightHeight};

        // buttons

        int btnHeight = 50;
        int btnWidth = 50;
        int controlPanelXCenter = albumPanel.x + (albumPanel.width / 2);
        int controlPanelYCenter = controlPanel.y + (controlPanel.height / 2);

        Rectangle playButton = {
            controlPanelXCenter - (btnWidth / 2),
            controlPanelYCenter - (btnHeight / 2),
            btnWidth,
            btnHeight
        };
        Rectangle forwardButton = {
            controlPanelXCenter - (btnWidth / 2) + PADDING + btnWidth,
            controlPanelYCenter - (btnHeight / 2),
            btnWidth,
            btnHeight
        };
        Rectangle backwardButton = {
            controlPanelXCenter - (btnWidth / 2) - PADDING - btnWidth,
            controlPanelYCenter - (btnHeight / 2),
            btnWidth,
            btnHeight
        };

        // progress bar
        Rectangle progressBar = {
            controlPanel.x + albumPanel.width + (rigthWidth*0.125) + (2*PADDING),
            controlPanel.y - 5 + (controlPanel.height / 2),
            (rigthWidth*0.75) - (2*PADDING),
            10
        };
        Rectangle progressBarFill = {
            controlPanel.x + albumPanel.width + (rigthWidth*0.125) + (2*PADDING),
            controlPanel.y - 5 + (controlPanel.height / 2),
            ((rigthWidth*0.75) - (2*PADDING)) * progress,
            10
        };

        // musics itens

        if(count * ITEM_HEIGHT > (int)bibliotecaPanel.height + 20)
        {
            if (CheckCollisionPointRec(mousePos, bibliotecaPanel)) {
                scrollOffset -= (int)(GetMouseWheelMove() * ITEM_HEIGHT * 3);

                int maxScroll = (count * ITEM_HEIGHT) - ((int)bibliotecaPanel.height + 20);
                if (scrollOffset < 0) scrollOffset = 0;
                if (scrollOffset > maxScroll && maxScroll > 0) scrollOffset = maxScroll;
            }
        }


        //progress = (mouseX - barX) / barWidth
        //currentSample = (int)(progress * totalSamples)

        //progress += 0.001f;
        //if (progress > 1.0f) progress = 0.0f;

        playback_update(&playback);


        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        if (CheckCollisionPointRec(mousePos, progressBar))
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {

                progress = (mousePos.x - progressBar.x) / progressBar.width;
                currentSample = (int)(progress * lib[0].total_samples);
            }
        }

        BeginDrawing();

            // draw base panels
            // background
            ClearBackground(RAYWHITE);

            // album panel
            DrawRectangleLinesEx(albumPanel, 2, DARKGRAY);
            DrawText("Album", albumPanel.x + 10, albumPanel.y + 10, 20, GRAY);

            // info panel
            DrawRectangleLinesEx(infoPanel, 2, DARKGRAY);
            DrawText(lib[currentMusic].title, infoPanel.x + 10, infoPanel.y + 10, 20, GRAY);
            DrawText(lib[currentMusic].artist, infoPanel.x + 10, infoPanel.y + 30, 20, GRAY);
            // text

            // control panel
            DrawRectangleLinesEx(controlPanel, 2, DARKGRAY);
            DrawText("Controles", controlPanel.x + 10, controlPanel.y + 10, 20, GRAY);

            // draw buttons
            {
                Color PlaybuttonColor = DARKGRAY;
                Color ForwardButtonColor = DARKGRAY;
                Color BackwardButtonColor = DARKGRAY;

                if (CheckCollisionPointRec(mousePos, playButton))
                {
                    PlaybuttonColor = GRAY;

                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        PlaybuttonColor = GREEN;
                    }
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        PlaybuttonColor = RED;
                    }
                }
                else
                {
                    PlaybuttonColor = DARKGRAY;
                }

                if (CheckCollisionPointRec(mousePos, forwardButton))
                {
                    ForwardButtonColor = GRAY;

                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        ForwardButtonColor = GREEN;
                    }
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        ForwardButtonColor = RED;
                    }
                }
                else
                {
                    ForwardButtonColor = DARKGRAY;
                }

                if (CheckCollisionPointRec(mousePos, backwardButton))
                {
                    BackwardButtonColor = GRAY;

                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        BackwardButtonColor = GREEN;
                    }
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        BackwardButtonColor = RED;
                    }
                }
                else
                {
                    BackwardButtonColor = DARKGRAY;
                }

                DrawRectangleRec(playButton, PlaybuttonColor);
                DrawRectangleRec(forwardButton, ForwardButtonColor);
                DrawRectangleRec(backwardButton, BackwardButtonColor);
            }

            // draw progress bar
            //DrawRectangle(controlPanelXCenter - 5, controlPanelYCenter - 5, 1000, 10, RED);
            DrawRectangleRec(progressBar, GRAY);
            DrawRectangleRec(progressBarFill, RED);


            // fila panel
            DrawRectangleLinesEx(filaPanel, 2, DARKGRAY);
            DrawText("Fila", filaPanel.x + 10, filaPanel.y + 10, 20, GRAY);

            // biblioteca panel
            //DrawRectangleLinesEx(bibliotecaPanel, 2, DARKGRAY);

            BeginScissorMode(bibliotecaPanel.x, bibliotecaPanel.y, bibliotecaPanel.width, bibliotecaPanel.height);
            for(int i = 0; i < count; i++)
            {
                int yPos = bibliotecaPanel.y + 10 + (i * ITEM_HEIGHT) - scrollOffset;
                Rectangle itens = {
                    bibliotecaPanel.x + 10,
                    yPos,
                    bibliotecaPanel.width - 20,
                    ITEM_HEIGHT
                };
                DrawRectangleLinesEx(itens, 1, GRAY);
                DrawText(lib[i].title, bibliotecaPanel.x + 10, yPos, 20, GRAY);

                if (CheckCollisionPointRec(mousePos, itens))
                {
                    DrawRectangleRec(itens, DARKGRAY);
                    DrawText(lib[i].title, bibliotecaPanel.x + 10, yPos, 20, LIGHTGRAY);
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    {
                        DrawRectangleLinesEx(itens, 1, DARKGRAY);
                        currentMusic = i;  // <-- the index becomes the selected track ID
                    }
                }
                if(i == currentMusic)
                {
                    DrawRectangleRec(itens, DARKGRAY);
                    DrawText(lib[i].title, bibliotecaPanel.x + 10, yPos, 20, LIGHTGRAY);
                }

            }
            EndScissorMode();

            if(count * ITEM_HEIGHT > (int)bibliotecaPanel.height + 20)
            {
                float ratio = bibliotecaPanel.height / (float)(count * ITEM_HEIGHT);
                float barH = bibliotecaPanel.height * ratio;
                float barY = bibliotecaPanel.y + (scrollOffset / (float)(count * ITEM_HEIGHT)) * bibliotecaPanel.height;
                DrawRectangle(bibliotecaPanel.x + bibliotecaPanel.width - 6, barY, 4, barH, DARKGRAY);
            }


        EndDrawing();
    }

    CloseAudioDevice();

    CloseWindow();
    return 0;
}
