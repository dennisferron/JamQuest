#include "TileGridLayer.hpp"

TileGridLayer::TileGridLayer(tmx_map const* map, tmx_layer const* layer)
    : opacity(layer->opacity), map_width(map->width), map_height(map->height)
{
    cells.resize(map_height*map_width);

    for (unsigned int i=0; i<map_height; i++)
    {
        for (unsigned int j=0; j<map_width; j++)
        {
            unsigned int cell_index = (i*map_width)+j;
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
    for (unsigned int i=0; i<map_height; i++)
    {
        for (unsigned int j=0; j<map_width; j++)
        {
            unsigned int cell_index = (i*map_width)+j;
            Tile const* tile = cells[cell_index].tile;

            if (tile != nullptr)
            {
                AnimationFrame const& frame = tile->frames[0];

                // TODO: actually calculate size
                auto tile_width = frame.source_region.w;
                auto tile_height = frame.source_region.h;

                SDL_Rect dest_rect;
                dest_rect.x = j*tile_width;
                dest_rect.y = i*tile_height;
                dest_rect.w = tile_width;
                dest_rect.h = tile_height;

                SDL_RenderCopy(renderer, frame.source_image, &frame.source_region, &dest_rect);
            }
        }
    }
}