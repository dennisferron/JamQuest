#include "TiledMapRenderer.hpp"

TiledMapRenderer::TiledMapRenderer(const tmx_map* map) :
    layers(map, map->ly_head)
{
    tmx_col_bytes col = tmx_col_to_bytes(map->backgroundcolor);
    background_color[0] = col.r;
    background_color[1] = col.g;
    background_color[2] = col.b;
    background_color[3] = col.a;
}

void TiledMapRenderer::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    SDL_SetRenderDrawColor(renderer,
        background_color[0],
        background_color[1],
        background_color[2],
        background_color[3]);
    SDL_RenderClear(renderer);
    layers.render(renderer, camera);
}

void TiledMapRenderer::update(uint32_t delta_ms)
{
    layers.update(delta_ms);
}
