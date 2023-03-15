#pragma once

#include "CompositionLayer.hpp"
#include "Animation.hpp"

#include <tmx.h>
#include <vector>
#include <memory>
#include <map>

// A tile doesn't know its size because all tiles in the
// layer have the same dimensions.
struct Tile
{
    std::vector<AnimationFrame> frames;
    int cur_frame = 0;
    int fractional_ms = 0;

    void update(uint32_t delta_ms);
};

struct Cell
{
    Tile const* tile = nullptr;
    SDL_RendererFlip flip_flags = SDL_FLIP_NONE;
};

class TileGridLayer : public CompositionLayer
{
private:
    std::vector<Cell> cells;
    std::map<uint32_t, Tile> tiles;

    // A layer can be offset from (0,0) world, e.g. for parallax.
    Vector2Df parallax_offset;

    /*
     * Width and height in pixels of a tile on the tileset image.
     */
    Vector2Di map_tile_size;

    unsigned int map_width;
    unsigned int map_height;

    float opacity = 1.0f;

    void draw_tile(SDL_Renderer* renderer, const Camera2D& camera, Tile const* tile, Vector2Df const& center, SDL_RendererFlip flip_flags) const;

public:
    TileGridLayer(tmx_map const* map, tmx_layer const* layer);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
    void update(uint32_t delta_ms) override;
};
