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

    void render(SDL_Renderer* renderer,
                Camera2D camera,
                CompositionLayer const* composition);
};
