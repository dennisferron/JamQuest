#pragma once

#include "LayerGroup.hpp"
#include "ZoomableView.hpp"

#include <optional>

class TiledMapRenderer : public CompositionLayer
{
private:
    LayerGroup* layers;
    tmx_col_bytes background_color;

    static std::optional<Vector2Di> get_tile_size(
            std::vector<CompositionLayer*> layer_group_children);

public:
    TiledMapRenderer(
        LayerGroup* layers,
        tmx_col_bytes const& background_color);

    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
    void update(uint32_t delta_ms) override;
    void debug_print(int indent) const override;
    Vector2Di get_tile_size() const;
};
