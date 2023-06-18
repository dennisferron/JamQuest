#include "TiledMapRenderer.hpp"

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