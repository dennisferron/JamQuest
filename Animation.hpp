#pragma once

#include <SDL2/SDL.h>

struct AnimationFrame
{
    SDL_Texture* source_image;
    SDL_Rect source_region;
    int duration;  // in milliseconds
};
