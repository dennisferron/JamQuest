#pragma once

#include "CompositionLayer.hpp"
#include "MapObject.hpp"

#include <tmx.h>

#include <vector>

class ObjectLayer : public CompositionLayer
{
private:
    uint32_t layer_color = 0;
    std::vector<MapObject*> objects;

public:
    ObjectLayer(tmx_map const* map, tmx_layer const* layer);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
};
