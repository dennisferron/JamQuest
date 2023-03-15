#pragma once

#include "Camera2D.hpp"
#include "CompositionLayer.hpp"

#include <SDL2/SDL.h>
#include <vector>

class ZoomableView
{
private:
    SDL_Texture* canvas;
    int width;
    int height;

public:
    ZoomableView(
        SDL_Renderer* renderer,
        int width,
        int height);

    ~ZoomableView();

    // We're going to change CompositionLayer to take
    // an integer zoom, and we need this to take a Camera2D.
    void render(SDL_Renderer* renderer,
                CompositionLayer const* composition,
                Camera2D camera);
};
