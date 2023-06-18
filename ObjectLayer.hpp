#pragma once

#include "CompositionLayer.hpp"
#include "MapObject.hpp"

#include <tmx.h>

#include <string>
#include <vector>

class ObjectLayer : public CompositionLayer
{
private:
    uint32_t layer_color = 0;
    std::vector<MapObject*> objects;

public:
    ObjectLayer(std::string name, uint32_t layer_color, std::vector<MapObject*> objects);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
};
