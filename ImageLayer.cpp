#include "ImageLayer.hpp"

void ImageLayer::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    Vector2Df p0_vp = camera.world_to_viewport(pos_w);
    Vector2Df p1_vp = camera.world_to_viewport(size_w);

    SDL_Rect dst_rect = {
            (int)p0_vp.x,
            (int)p0_vp.y,
            (int)(p1_vp.x - p0_vp.x),
            (int)(p1_vp.y - p0_vp.y)
    };

    SDL_RenderCopy(renderer, texture, nullptr, &dst_rect);
}

ImageLayer::ImageLayer(std::string name, SDL_Texture* texture, Vector2Df pos_w, Vector2Df size_w)
    : CompositionLayer(name), texture(texture), pos_w(pos_w), size_w(size_w)
{
}
