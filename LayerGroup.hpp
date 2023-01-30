#pragma once

#include "CompositionLayer.hpp"

#include <tmx.h>

#include <vector>

class LayerGroup : public CompositionLayer
{
private:
    std::vector<CompositionLayer*> children;

public:
    LayerGroup(tmx_map const* map, tmx_layer const* list_head);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
};
