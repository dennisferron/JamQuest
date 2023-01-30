#include "ObjectLayer.hpp"

ObjectLayer::ObjectLayer(const tmx_map* map, const tmx_layer* layer)
    : layer_color(layer->content.objgr->color)
{
    tmx_object_group const* objgr = layer->content.objgr;

    tmx_object *head = objgr->head;
    while (head) {
        if (head->visible) {
            switch (head->obj_type)
            {
                case OT_SQUARE:
                    objects.push_back(new RectangleMapObject(head));
                    break;
                case OT_POLYGON:
                case OT_POLYLINE:
                    objects.push_back(new PolyMapObject(head));
                    break;
//                case OT_ELLIPSE:
//                    objects.push_back(new EllipseMapObject(head));
//                    break;
//                case OT_TILE:
//                    objects.push_back(new TileMapObject(head));
//                    break;
//                case OT_POINT:
//                    break;
//                case OT_TEXT:
//                    break;
//                case OT_NONE:
//                    break;
                default:
                    break;
            }
        }
        head = head->next;
    }
}

void ObjectLayer::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    tmx_col_bytes col = tmx_col_to_bytes(layer_color);
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);

    for (MapObject const* map_obj : objects)
        map_obj->render(renderer, camera);
}
