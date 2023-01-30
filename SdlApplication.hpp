#pragma once

#include "renderwindow.h"
#include "player.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

class SdlApplication
{
protected:
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 480;

    const Uint8 *keyState;

    SDL_Window* window;
    SDL_Renderer* renderer;
    RenderWindow* render_window;

    bool gameRunning = true;

public:
    SdlApplication();
    ~SdlApplication();

    virtual void gameLoop() = 0;
    bool is_game_running() const { return gameRunning; }
};
