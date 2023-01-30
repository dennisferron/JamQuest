#pragma once

#include "CompositionLayer.hpp"

#include <tmx.h>

class ImageLayer : public CompositionLayer
{
private:
    SDL_Texture* texture;
    SDL_Rect region;

public:
    ImageLayer(tmx_map const* map, tmx_layer const* layer);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
};
