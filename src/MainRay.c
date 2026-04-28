#include "raylib.h"
#include "wavParser.h"
#include "navfolder.h"
#include <stdio.h>


int main(void) {
    const int MIN_WIDTH = 1024;
    const int MIN_HEIGHT = 600;


    const int PADDING = 20;
    const int MAX_LEFT_SIZE = 320;
    const int MIN_LEFT_SIZE = 256;
    const int CONTROL_PANEL_HEIGHT = 100;
    const int TOP_BAR_HEIGHT = 25;

    struct MusicMetadata lib[MAX_MUSIC_COUNT];
    int count = 0;
    nav_folder("Test_music_files", 0, lib, &count);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(MIN_WIDTH, MIN_HEIGHT, "MuPlayer - Responsive UI");
    SetTargetFPS(60);

    SetWindowMinSize(MIN_WIDTH, MIN_HEIGHT);

    while (!WindowShouldClose()) {

        int screen_w = GetScreenWidth();
        int screen_h = GetScreenHeight();

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
        int controlPanelXCenter = controlPanel.x + (controlPanel.width / 2);
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





        BeginDrawing();

            // draw base panels
            // background
            ClearBackground(RAYWHITE);

            // album panel
            DrawRectangleLinesEx(albumPanel, 2, DARKGRAY);
            DrawText("Album", albumPanel.x + 10, albumPanel.y + 10, 20, GRAY);

            // info panel
            DrawRectangleLinesEx(infoPanel, 2, DARKGRAY);
            //DrawText("Infos", infoPanel.x + 10, infoPanel.y + 10, 20, GRAY);
            // text
            DrawText(lib[0].title, infoPanel.x + 10, infoPanel.y + 20, 20, GRAY);
            DrawText(lib[0].artist, infoPanel.x + 10, infoPanel.y + 40, 20, GRAY);

            // control panel
            DrawRectangleLinesEx(controlPanel, 2, DARKGRAY);
            DrawText("Controles", controlPanel.x + 10, controlPanel.y + 10, 20, GRAY);

            // draw buttons
            {
                Vector2 mousePos = GetMousePosition();
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


            // fila panel
            DrawRectangleLinesEx(filaPanel, 2, DARKGRAY);
            DrawText("Fila", filaPanel.x + 10, filaPanel.y + 10, 20, GRAY);

            // biblioteca panel
            DrawRectangleLinesEx(bibliotecaPanel, 2, DARKGRAY);
            for(int i = 0; i < count; i++)
            {
                DrawText(lib[i].title, bibliotecaPanel.x + 10, bibliotecaPanel.y + 10 + i * 20, 20, GRAY);
            }
            //DrawText("Biblioteca", bibliotecaPanel.x + 10, bibliotecaPanel.y + 10, 20, GRAY);
            //printf("%s\n", lib[count - 1].filepath);



        EndDrawing();
    }

    CloseWindow();
    return 0;
}
