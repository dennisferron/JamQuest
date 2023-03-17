#pragma once

#include "LayerGroup.hpp"
#include "ZoomableView.hpp"

class TiledMapRenderer : public CompositionLayer
{
private:
    LayerGroup layers;
    Uint8 background_color[4];

public:
    TiledMapRenderer(tmx_map const* map);

    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
    void update(uint32_t delta_ms) override;
};
