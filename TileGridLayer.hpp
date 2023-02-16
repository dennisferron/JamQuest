#pragma once

#include "CompositionLayer.hpp"
#include "Animation.hpp"

#include <vector>
#include <memory>
#include <tmx.h>

struct TileSet
{
    Vector2D tile_size;
    SDL_Texture* image;
};

// A tile doesn't know its size because all tiles in the
// layer have the same dimensions.
struct Tile
{
    std::vector<AnimationFrame> frames;
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

    // A layer can be offset from (0,0) world, e.g. for parallax.
    Vector2D origin;

    /*
     * Width and height in pixels of a tile on the tileset image.
     * The tile will be scaled by the time it reaches the screen.
     * TODO: allow a source tile to be larger than its nominal box.
     */
    Vector2D source_tile_size;

    /*
     * How large a tile is in world coordinates.
     * This by itself isn't enough to determine how large it
     * will appear on screen; that's determined by the camera.
     */
    Vector2D tile_size_in_world;

    unsigned int map_width;
    unsigned int map_height;

    float opacity = 1.0f;

    // Convert double value to size_t while ensuring result is not
    // less than dim_min and not greater than dim_max (inclusive, both).
    static std::size_t clip_index_dimension(double value, unsigned int dim_min, unsigned int dim_max);

    // Map a viewport logical coordinate to a (fractional!) tile coordinate.
    Vector2D viewport_to_tile(Camera2D const& camera, Vector2D const& vp) const;

public:
    TileGridLayer(tmx_map const* map, tmx_layer const* layer);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
};
