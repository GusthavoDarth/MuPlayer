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
    const int PADDING = 10;
    const int MAX_LEFT_SIZE = 320;
    const int MIN_LEFT_SIZE = 256;
    const int CONTROL_PANEL_HEIGHT = 100;
    const int TOP_BAR_HEIGHT = 25+ PADDING;
    const int ITEM_HEIGHT = 22;
    const int SCROLL_WIDTH = 10;
    const float ROUNDINESS = 0.05;
    const int TEXT_SIZE = 20;

    const int btnHeight = 40;
    const int btnWidth = 40;

    struct MusicMetadata lib[MAX_MUSIC_COUNT];
    struct Playback playback;

    int count = 0;
    int scrollOffset = 0;
    int dragStartScrollOffset = scrollOffset;
    int dragStartMouseY = 0;
    int currentSample = 0;
    float progress = 0.0f;
    int currentMusic = 0;
    bool isDraggingScrollbar = false;
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

        Rectangle topPanel = { PADDING, PADDING, screen_w - (2*PADDING), TOP_BAR_HEIGHT - PADDING};

        Rectangle albumPanel = { PADDING, TOP_BAR_HEIGHT + PADDING, leftWidth, leftWidth};

        Rectangle infoPanel = { PADDING, albumPanel.y + albumPanel.height + PADDING, leftWidth, screen_h - (albumPanel.height + CONTROL_PANEL_HEIGHT + TOP_BAR_HEIGHT + (PADDING * 4))};

        Rectangle controlPanel = { PADDING, infoPanel.y + infoPanel.height + PADDING, screen_w - (2*PADDING), CONTROL_PANEL_HEIGHT};


        int rightHeight = screen_h - ( CONTROL_PANEL_HEIGHT + TOP_BAR_HEIGHT + (PADDING * 3));
        int rigthWidth = screen_w - ( leftWidth + (PADDING*2));

        Rectangle filaPanel = {screen_w - rigthWidth, TOP_BAR_HEIGHT + PADDING, ((float)rigthWidth/2) - PADDING, rightHeight};
        Rectangle bibliotecaPanel = {filaPanel.x + filaPanel.width + PADDING, TOP_BAR_HEIGHT + PADDING, ((float)rigthWidth/2) - PADDING, rightHeight};


        int controlPanelXCenter = albumPanel.x + (albumPanel.width / 2);
        int controlPanelYCenter = controlPanel.y + (controlPanel.height / 2);


        // functions buttons
        Rectangle exitBtn = { screen_w - (MeasureText("Exit", TEXT_SIZE)+10)- PADDING, PADDING, MeasureText("Exit", TEXT_SIZE)+10, topPanel.height};

        // music buttons
        Vector2 circleCenter = { controlPanelXCenter, controlPanelYCenter };

        Rectangle PlayButton = {
            controlPanelXCenter - ((float)btnWidth / 2),
            controlPanelYCenter - ((float)btnHeight / 2),
            btnWidth,
            btnHeight
        };
        Rectangle ForwardButton = {
            controlPanelXCenter - ((float)btnWidth / 2) + PADDING + btnWidth,
            controlPanelYCenter - ((float)btnHeight / 2),
            btnWidth,
            btnHeight
        };
        Rectangle BackwardButton = {
            controlPanelXCenter - ((float)btnWidth / 2) - PADDING - btnWidth,
            controlPanelYCenter - ((float)btnHeight / 2),
            btnWidth,
            btnHeight
        };

        // progress bar
        Rectangle ProgressBar = {
            controlPanel.x + albumPanel.width + (rigthWidth*0.125) + (2*PADDING),
            controlPanel.y - 5 + (controlPanel.height / 2),
            (rigthWidth*0.75) - (2*PADDING),
            10
        };
        Rectangle ProgressBarFill = {
            controlPanel.x + albumPanel.width + (rigthWidth*0.125) + (2*PADDING),
            controlPanel.y - 5 + (controlPanel.height / 2),
            ((rigthWidth*0.75) - (2*PADDING)) * progress,
            10
        };

        // scrollbar
        Rectangle scrollbar = {
            bibliotecaPanel.x + bibliotecaPanel.width - 10,
            bibliotecaPanel.y + (scrollOffset / (float)(count * ITEM_HEIGHT)) * bibliotecaPanel.height,
            SCROLL_WIDTH,
            (bibliotecaPanel.height) * (bibliotecaPanel.height + ITEM_HEIGHT) / (float)(count * ITEM_HEIGHT)
        };

        // musics itens
        if(count * ITEM_HEIGHT > (int)bibliotecaPanel.height + ITEM_HEIGHT)
        {
            int maxScroll = (count * ITEM_HEIGHT) - ((int)bibliotecaPanel.height + ITEM_HEIGHT);
            if (CheckCollisionPointRec(mousePos, bibliotecaPanel)) {
                scrollOffset -= (int)(GetMouseWheelMove() * ITEM_HEIGHT);
                if (scrollOffset < 0) scrollOffset = 0;
                if (scrollOffset > maxScroll && maxScroll > 0) scrollOffset = maxScroll;
            }
        }
        else scrollOffset = 0;

        // scrollbar collision
        if (CheckCollisionPointRec(mousePos, scrollbar) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            isDraggingScrollbar = true;
            dragStartScrollOffset = scrollOffset;
            dragStartMouseY = mousePos.y;
        }
        else if (isDraggingScrollbar && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            int mouseDelta = mousePos.y - dragStartMouseY;
            int maxScroll = (count * ITEM_HEIGHT) - ((int)bibliotecaPanel.height + ITEM_HEIGHT);
            float ratio = maxScroll / bibliotecaPanel.height;

            scrollOffset = dragStartScrollOffset + (int)(mouseDelta * ratio * 2);

            if (scrollOffset < 0) scrollOffset = 0;
            if (scrollOffset > maxScroll && maxScroll > 0) scrollOffset = maxScroll;
        }
        else if (isDraggingScrollbar && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { isDraggingScrollbar = false; }

        //pregress bar
        if (CheckCollisionPointRec(mousePos, ProgressBar))
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                progress = (mousePos.x - ProgressBar.x) / ProgressBar.width;
                currentSample = (int)(progress * lib[currentMusic].total_samples);
            }
        }


        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        playback_update(&playback);

        BeginDrawing();

            // draw base panels
            // background
            ClearBackground(RAYWHITE);

            // top panel
            DrawRectangleLinesEx(topPanel, 2, LIGHTGRAY);

            // draw buttons
            //DrawRectangleLinesEx(exitBtn, 2, DARKGRAY);
            //int posx = exitBtn.x - (((float)MeasureText("Exit", 20) - exitBtn.width)/2);
            //int posy = PADDING - ((TEXT_SIZE - topPanel.height)/2);
            //DrawText("Exit", posx, posy, TEXT_SIZE, GRAY);



            // album panel
            DrawRectangleLinesEx(albumPanel, 2, DARKGRAY);
            DrawText("Album", albumPanel.x + 10, albumPanel.y + 10, TEXT_SIZE, GRAY);

            // info panel
            DrawRectangleLinesEx(infoPanel, 2, DARKGRAY);

            BeginScissorMode(infoPanel.x, infoPanel.y, infoPanel.width, infoPanel.height);

            DrawText(lib[currentMusic].title, infoPanel.x + 10, infoPanel.y + 10, TEXT_SIZE, GRAY);
            DrawText(lib[currentMusic].artist, infoPanel.x + 10, infoPanel.y + 30, TEXT_SIZE, GRAY);

            EndScissorMode();

            // control panel
            DrawRectangleLinesEx(controlPanel, 2, DARKGRAY);
            DrawText("Controles", controlPanel.x + 10, controlPanel.y + 10, TEXT_SIZE, GRAY);

            // draw buttons
            {
                Color PlaybuttonColor = DARKGRAY;
                Color ForwardButtonColor = DARKGRAY;
                Color BackwardButtonColor = DARKGRAY;

                // play/pause button
                if (CheckCollisionPointRec(mousePos, PlayButton))
                {
                    PlaybuttonColor = GRAY;
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { PlaybuttonColor = RED; }
                }
                else { PlaybuttonColor = DARKGRAY; }

                // forward button
                if (CheckCollisionPointRec(mousePos, ForwardButton))
                {
                    ForwardButtonColor = GRAY;
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { ForwardButtonColor = RED; }
                }
                else { ForwardButtonColor = DARKGRAY; }

                // backward button
                if (CheckCollisionPointRec(mousePos, BackwardButton))
                {
                    BackwardButtonColor = GRAY;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        BackwardButtonColor = RED;
                        nav_folder("Musics", 0, lib, &count);
                    }
                }
                else { BackwardButtonColor = DARKGRAY; }

                DrawCircleV(circleCenter, (float)btnWidth / 2, PlaybuttonColor);
                //DrawRectangleRec(PlayButton, PlaybuttonColor);
                DrawRectangleRec(ForwardButton, ForwardButtonColor);
                DrawRectangleRec(BackwardButton, BackwardButtonColor);
            }

            // draw progress bar
            //DrawRectangle(controlPanelXCenter - 5, controlPanelYCenter - 5, 1000, 10, RED);
            DrawRectangleRounded(ProgressBar, 1, 10, GRAY);
            DrawRectangleRounded(ProgressBarFill, 1, 10, RED);


            // fila panel
            DrawRectangleLinesEx(filaPanel, 2, DARKGRAY);
            DrawText("Fila", filaPanel.x + 10, filaPanel.y + 10, TEXT_SIZE, GRAY);

            // biblioteca panel
            BeginScissorMode(bibliotecaPanel.x, bibliotecaPanel.y, bibliotecaPanel.width, bibliotecaPanel.height);

            for(int i = 0; i < count; i++)
            {
                int yPos = bibliotecaPanel.y + (i * ITEM_HEIGHT) - scrollOffset;
                Rectangle itens = {
                    bibliotecaPanel.x,
                    yPos,
                    bibliotecaPanel.width - SCROLL_WIDTH,
                    ITEM_HEIGHT
                };

                DrawText(lib[i].title, bibliotecaPanel.x + 5, yPos, TEXT_SIZE, GRAY);

                if (CheckCollisionPointRec(mousePos, itens))
                {
                    DrawRectangleRec(itens, DARKGRAY);
                    DrawText(lib[i].title, bibliotecaPanel.x + 15, yPos, TEXT_SIZE, LIGHTGRAY);
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    {
                        DrawRectangleLinesEx(itens, 1, DARKGRAY);
                        currentMusic = i;
                    }
                }
                if(i == currentMusic)
                {
                    DrawRectangleRec(itens, DARKGRAY);
                    DrawText(lib[i].title, bibliotecaPanel.x + 10, yPos, TEXT_SIZE, LIGHTGRAY);
                }
            }
            EndScissorMode();


            if(count * ITEM_HEIGHT > (int)bibliotecaPanel.height + ITEM_HEIGHT){
                DrawRectangleRec(scrollbar, DARKGRAY);
            }
            DrawRectangleLinesEx(bibliotecaPanel, 2, DARKGRAY);

        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
