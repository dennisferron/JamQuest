#include "SdlApplication.hpp"

#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <stdlib.h>

SdlApplication::SdlApplication()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    window = SDL_CreateWindow("SDL Application",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);

    if (window == NULL)
        std::cerr << "Window failed to init. Error: " << SDL_GetError() << std::endl;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    render_window = new RenderWindow(window, renderer);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    srand((unsigned)time(0));
}

SdlApplication::~SdlApplication()
{
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
