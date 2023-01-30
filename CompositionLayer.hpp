#pragma once

#include "Camera2D.hpp"

#include <SDL2/SDL.h>

class CompositionLayer
{
public:
    virtual ~CompositionLayer() = 0;
    virtual void render(SDL_Renderer* renderer, Camera2D const& camera) const = 0;
};
