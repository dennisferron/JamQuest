#include "TileGridLayer.hpp"

TileGridLayer::TileGridLayer(tmx_map const* map, tmx_layer const* layer)
    : opacity(layer->opacity), map_width(map->width), map_height(map->height),
      tile_size_in_world({(double)map->tile_width, (double)map->tile_height})
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
    Vector2D p0 = floor(viewport_to_tile(camera, {0,0}));
    Vector2D p1 = ceil(viewport_to_tile(camera, camera.get_view_size()));

    std::size_t col_min = clip_index_dimension(p0.x, 0, map_width);
    std::size_t col_max = clip_index_dimension(p1.x, 0, map_width);
    std::size_t row_min = clip_index_dimension(p0.y, 0, map_height);
    std::size_t row_max = clip_index_dimension(p1.y, 0, map_height);

    for (std::size_t row = row_min; row < row_max; row++)
    {
        for (std::size_t col = col_min; col < col_max; col++)
        {
            unsigned int cell_index = (row * map_width) + col;
            Tile const* tile = cells[cell_index].tile;

            if (tile != nullptr)
            {
                // Determine which animation frame to display.
                AnimationFrame const& frame = tile->frames[tile->cur_frame];

                // The size of the frame image could in theory
                // be different from the map tile size (spacing);
                // this also affects where the tile upper left should go.
                // These aliases make the equations for tile_pos_w
                // (see below) fit on one line.
                Vector2D tl_sz = tile_size_in_world;
                double src_w = frame.source_region.w;
                double src_h = frame.source_region.h;

                // Calculate upper left of tile col,row in world coordinates.
                // plus a correction factor if image size different from tiles.
                // (The correction assumes we want to keep tile center in same place.)
                Vector2D tile_ul_w = {
                        col * tl_sz.x + (tl_sz.x-src_w)/2,
                        row * tl_sz.y + (tl_sz.y-src_h)/2
                };

                // Calculate the bottom right from upper left, since camera
                // is based on points not sizes.
                Vector2D tile_br_w = tile_ul_w + Vector2D {src_w, src_h};

                // And then convert these to viewport for the dest rect.
                Vector2D tile_ul_vp = camera.world_to_viewport(tile_ul_w);
                Vector2D tile_br_vp = camera.world_to_viewport(tile_br_w);
                Vector2D dest_sz_vp = tile_br_vp - tile_ul_vp;

                SDL_Rect dest_rect = {
                        (int)tile_ul_vp.x,
                        (int)tile_ul_vp.y,
                        (int)dest_sz_vp.x,
                        (int)dest_sz_vp.y
                };

                // It's possible the camera transform could have flipped
                // the left/right or upper/lower designations, so
                // normalize the rect if width or height are negative.
                if (dest_rect.w < 0)
                {
                    dest_rect.w = -dest_rect.w;
                    dest_rect.x -= dest_rect.w;
                }
                if (dest_rect.h < 0) // not an else if
                {
                    dest_rect.h = -dest_rect.h;
                    dest_rect.y -= dest_rect.h;
                }

                SDL_RenderCopyEx(
                        renderer,
                        frame.source_image,
                        &frame.source_region,
                        &dest_rect,
                        0,
                        nullptr,
                        cells[cell_index].flip_flags );
            }
        }
    }
}

std::size_t TileGridLayer::clip_index_dimension(double value, unsigned int dim_min, unsigned int dim_max)
{
    if (value <= dim_min)
    {
        return dim_min;
    }
    else if (value >= dim_max)
    {
        return dim_max;
    }
    else
    {
        return static_cast<std::size_t>(value);
    }
}

Vector2D TileGridLayer::viewport_to_tile(const Camera2D& camera, const Vector2D& vp) const
{
    Vector2D vp_in_world = camera.viewport_to_world(vp);
    Vector2D world_to_tile = vp_in_world / tile_size_in_world;
    return world_to_tile;
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
