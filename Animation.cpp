#include "Animation.hpp"

AnimationFrame::AnimationFrame(tmx_tile const* tile, int duration_ms)
    : duration_ms(duration_ms)
{
    source_image =
            reinterpret_cast<SDL_Texture*>(
                    tile->image?
                    tile->image->resource_image :
                    tile->tileset->image->resource_image);

    source_region =
            {
                    static_cast<int>(tile->ul_x),
                    static_cast<int>(tile->ul_y),
                    static_cast<int>(tile->tileset->tile_width),
                    static_cast<int>(tile->tileset->tile_height)
            };
}

std::vector<AnimationFrame> get_animation_frames(tmx_tile const* tile)
{
    std::vector<AnimationFrame> result;

    if (tile->animation == nullptr)
    {
        result.push_back(AnimationFrame(tile, 0));
    }
    else
    {
        for (int frame_num=0; frame_num < tile->animation_len; frame_num++)
        {
            tmx_anim_frame const& frame = tile->animation[frame_num];
            tmx_tile const* frame_tile = &(tile->tileset->tiles[frame.tile_id]);
            result.push_back(AnimationFrame(frame_tile, frame.duration));
        }
    }

    return result;
}
