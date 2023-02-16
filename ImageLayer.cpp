#include "ImageLayer.hpp"

ImageLayer::ImageLayer(const tmx_map* map, const tmx_layer* layer)
{
    pos_w = { 0, 0 };
    size_w = {
            (double)map->width * map->tile_width,
            (double)map->height * map->tile_height
    };

    texture = (SDL_Texture*)layer->content.image->resource_image;;
}

void ImageLayer::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    Vector2D p0_vp = camera.world_to_viewport(pos_w);
    Vector2D p1_vp = camera.world_to_viewport(size_w);

    SDL_Rect dst_rect = {
            (int)p0_vp.x,
            (int)p0_vp.y,
            (int)(p1_vp.x - p0_vp.x),
            (int)(p1_vp.y - p0_vp.y)
    };

    SDL_RenderCopy(renderer, texture, nullptr, &dst_rect);
}
