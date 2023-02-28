#pragma once

#include <tmx.h>
#include <SDL2/SDL.h>
#include <vector>

struct AnimationFrame
{
    SDL_Texture* source_image;
    SDL_Rect source_region;
    int duration_ms;

    AnimationFrame(tmx_tile const* tile, int duration_ms);
};

std::vector<AnimationFrame> get_animation_frames(tmx_tile const* tile);