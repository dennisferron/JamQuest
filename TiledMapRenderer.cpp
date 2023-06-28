#include "TiledMapRenderer.hpp"
#include "TileGridLayer.hpp"

#include <stdexcept>

TiledMapRenderer::TiledMapRenderer(
    LayerGroup* layers,
    tmx_col_bytes const& background_color) :
        CompositionLayer("map renderer"),
        layers(layers), background_color(background_color)
{
}

void TiledMapRenderer::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    SDL_SetRenderDrawColor(
            renderer,
        background_color.r,
        background_color.g,
        background_color.b,
        background_color.a);

    SDL_RenderClear(renderer);

    layers->render(renderer, camera);
}

void TiledMapRenderer::update(uint32_t delta_ms)
{
    layers->update(delta_ms);
}

void TiledMapRenderer::debug_print(int indent) const
{
    CompositionLayer::debug_print(indent);
    layers->debug_print(indent+1);
}

Vector2Di TiledMapRenderer::get_tile_size() const
{
    if (auto result = get_tile_size(layers->get_children()))
    {
        return *result;
    }
    else
    {
        throw std::logic_error("Unable to get tile size for layers.");
    }
}

std::optional<Vector2Di> TiledMapRenderer::get_tile_size(
        std::vector<CompositionLayer*> layer_group_children)
{
    for (auto layer : layer_group_children)
    {
        if (auto tile_layer =
                dynamic_cast<TileGridLayer const*>(layer))
        {
            return tile_layer->get_tile_size();
        }
        else if (auto layer_group =
                dynamic_cast<LayerGroup const*>(layer))
        {
            if (auto found_size =
                    get_tile_size(layer_group->get_children()))
                return found_size;
        }
    }

    return {};
}
