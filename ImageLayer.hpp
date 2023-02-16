#pragma once

#include "CompositionLayer.hpp"

#include <tmx.h>

class ImageLayer : public CompositionLayer
{
private:
    SDL_Texture* texture;
    Vector2D pos_w;
    Vector2D size_w;

public:
    ImageLayer(tmx_map const* map, tmx_layer const* layer);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
};
