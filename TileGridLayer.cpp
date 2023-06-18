#include "TileGridLayer.hpp"

void TileGridLayer::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    Rect2Df rect = camera.world_rect() - parallax_offset;

    for (double y = floor(rect.top); y <= ceil(rect.bottom); y++)
    {
        if (y >= 0.0 && y < map_height)
        {
            for (double x = floor(rect.left); x <= ceil(rect.right); x++)
            {
                if (x >= 0.0 && x < map_width)
                {
                    unsigned int cell_index =
                        static_cast<unsigned int>(y * map_width + x);
                    Tile const* tile = cells[cell_index].tile;
                    if (tile != nullptr)
                    {
                        Vector2Df center = { x+0.5, y+0.5 };
                        SDL_RendererFlip flip_flags = cells[cell_index].flip_flags;
                        draw_tile(renderer, camera, tile, center, flip_flags);
                    }
                }
            }
        }
    }
}

void TileGridLayer::draw_tile(SDL_Renderer* renderer,
                              const Camera2D& camera,
                              Tile const* tile,
                              Vector2Df const& center,
                              SDL_RendererFlip flip_flags) const
{
    // Determine which animation frame to display.
    AnimationFrame const& frame = tile->frames[tile->cur_frame];

    // The size of the animation frame image could in theory
    // be different from the map tile size (tile spacing);
    // this also affects where the tile upper left should go.
    Vector2Df size_ratio =
    {
            static_cast<double>(frame.source_region.w)
                / map_tile_size.x,
            static_cast<double>(frame.source_region.h)
                / map_tile_size.y
    };

    // Calculate the top left and bottom right points
    // of the tile, scaling size_ratio about the center.
    Vector2Df world_ul = center - 0.5*size_ratio;
    Vector2Df world_br = center + 0.5*size_ratio;

    // And then convert these to viewport for the dest rect.
    Vector2Df p0 = camera.world_to_viewport(world_ul);
    Vector2Df p1 = camera.world_to_viewport(world_br);

    Vector2Df size_in_view = floor(p1) - floor(p0);

    SDL_Rect dest_rect = {
            (int)p0.x,
            (int)p0.y,
            (int)size_in_view.x,
            (int)size_in_view.y
    };

    SDL_RenderCopyEx(
            renderer,
            frame.source_image,
            &frame.source_region,
            &dest_rect,
            0,
            nullptr,
            flip_flags );
}

void TileGridLayer::update(uint32_t delta_ms)
{
    for (auto& kv : tiles)
    {
        kv.second->update(delta_ms);
    }
}

TileGridLayer::TileGridLayer(
        std::string name,
        std::vector<Cell> cells,
        std::map<uint32_t, Tile*> tiles,
        TileGridLayer::ConstructionInfo const& info
    ) :
        CompositionLayer(name),
        cells(cells),
        tiles(tiles),
        map_width(info.map_width),
        map_height(info.map_height),
        opacity(info.opacity),
        parallax_offset(info.parallax_offset),
        map_tile_size(info.map_tile_size)
{
}

void Tile::update(uint32_t delta_ms)
{
    uint32_t all_frames_ms = 0;
    for (auto const& fr : frames)
        all_frames_ms += fr.duration_ms;

    if (all_frames_ms > 0)
    {
        fractional_ms += delta_ms;
        while (fractional_ms >= frames[cur_frame].duration_ms)
        {
            fractional_ms -= frames[cur_frame].duration_ms;
            cur_frame = (cur_frame + 1) % frames.size();
        }
    }
}
