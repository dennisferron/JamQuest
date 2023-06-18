#pragma once

#include "Camera2D.hpp"

#include <SDL2/SDL.h>

#include <string>

class CompositionLayer
{
private:
    std::string name;

public:
    CompositionLayer(std::string name);
    virtual ~CompositionLayer() = 0;
    virtual void render(SDL_Renderer* renderer, Camera2D const& camera) const = 0;
    virtual void update(uint32_t delta_ms);
    virtual void debug_print(int indent) const;
};
