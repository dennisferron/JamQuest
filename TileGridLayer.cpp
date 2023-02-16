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
            unsigned int tile_id = layer->content.gids[cell_index];
            unsigned int gid = tile_id & TMX_FLIP_BITS_REMOVAL;
            unsigned int flags = tile_id & ~TMX_FLIP_BITS_REMOVAL;

            tmx_tile const* tile = map->tiles[gid];
            if (tile != NULL)
            {
                tmx_tileset const* tileset = tile->tileset;

                AnimationFrame frame;

                frame.source_image =
                    reinterpret_cast<SDL_Texture*>(
                        tile->image?
                        tile->image->resource_image :
                        tileset->image->resource_image);

                frame.source_region =
                {
                    static_cast<int>(tile->ul_x),
                    static_cast<int>(tile->ul_y),
                    static_cast<int>(tileset->tile_width),
                    static_cast<int>(tileset->tile_height)
                };

                Tile* out_tile = new Tile();
                out_tile->frames.push_back(frame);
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
                AnimationFrame const& frame = tile->frames[0];

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

                SDL_RenderCopy(renderer, frame.source_image, &frame.source_region, &dest_rect);
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
