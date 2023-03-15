#include "TileGridLayer.hpp"

TileGridLayer::TileGridLayer(tmx_map const* map, tmx_layer const* layer)
    : opacity(layer->opacity), map_width(map->width), map_height(map->height),
      map_tile_size({(int)map->tile_width, (int)map->tile_height})
{
    cells.resize(map_height*map_width);

    for (unsigned int row = 0; row < map_height; row++)
    {
        for (unsigned int col = 0; col < map_width; col++)
        {
            unsigned int cell_index = (row * map_width) + col;
            unsigned int raw_gid = layer->content.gids[cell_index];
            unsigned int gid = raw_gid & TMX_FLIP_BITS_REMOVAL;

            unsigned int flags = raw_gid & ~TMX_FLIP_BITS_REMOVAL;
            cells[cell_index].flip_flags = static_cast<SDL_RendererFlip>(
                    (flags & TMX_FLIPPED_HORIZONTALLY ? (int)SDL_FLIP_HORIZONTAL : 0)
                    |   (flags & TMX_FLIPPED_VERTICALLY ? (int)SDL_FLIP_VERTICAL : 0));

            tmx_tile const* tile = map->tiles[gid];
            if (tile != NULL)
            {
                Tile* out_tile = &tiles[gid];

                if (out_tile->frames.empty())
                    out_tile->frames = get_animation_frames(tile);

                cells[cell_index].tile = out_tile;
            }
        }
    }
}

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
        kv.second.update(delta_ms);
    }
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
