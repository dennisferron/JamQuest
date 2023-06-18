#include "ObjectLayer.hpp"

void ObjectLayer::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    tmx_col_bytes col = tmx_col_to_bytes(layer_color);
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);

    for (MapObject const* map_obj : objects)
        map_obj->render(renderer, camera);
}

ObjectLayer::ObjectLayer(std::string name, uint32_t layer_color, std::vector<MapObject*> objects)
    : CompositionLayer(name), layer_color(layer_color), objects(objects)
{

}
