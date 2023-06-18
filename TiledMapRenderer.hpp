#pragma once

#include "LayerGroup.hpp"
#include "ZoomableView.hpp"

class TiledMapRenderer : public CompositionLayer
{
private:
    LayerGroup* layers;
    tmx_col_bytes background_color;

public:
    TiledMapRenderer(
        LayerGroup* layers,
        tmx_col_bytes const& background_color);

    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
    void update(uint32_t delta_ms) override;
    void debug_print(int indent) const override;
};
