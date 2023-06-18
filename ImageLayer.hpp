#pragma once

#include "CompositionLayer.hpp"

#include <tmx.h>

#include <string>

class ImageLayer : public CompositionLayer
{
private:
    SDL_Texture* texture;
    Vector2Df pos_w;
    Vector2Df size_w;

public:
    ImageLayer(std::string name, SDL_Texture* texture,
               Vector2Df pos_w, Vector2Df size_w);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
};
