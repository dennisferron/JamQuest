#include "ImageLayer.hpp"

ImageLayer::ImageLayer(const tmx_map* map, const tmx_layer* layer)
{
    texture = (SDL_Texture*)layer->content.image->resource_image;;
    region = {0,0,0,0};
    SDL_QueryTexture(texture, NULL, NULL, &(region.w), &(region.h));
}

void ImageLayer::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}
