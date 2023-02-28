#include "LayerGroup.hpp"
#include "ObjectLayer.hpp"
#include "ImageLayer.hpp"
#include "TileGridLayer.hpp"

LayerGroup::LayerGroup(const tmx_map* map, const tmx_layer* list_head)
{
    tmx_layer const* cur_layer = list_head;

    while (cur_layer != nullptr)
    {
        if (cur_layer->visible)
        {
            switch (cur_layer->type)
            {
                case L_GROUP:
                    children.push_back(new LayerGroup(map, cur_layer->content.group_head));
                    break;
                case L_OBJGR:
                    children.push_back(new ObjectLayer(map, cur_layer));
                    break;
                case L_IMAGE:
                    children.push_back(new ImageLayer(map, cur_layer));
                    break;
                case L_LAYER:
                    children.push_back(new TileGridLayer(map, cur_layer));
                    break;
                default:
                    break;
            }
        }
        cur_layer = cur_layer->next;
    }
}

void LayerGroup::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    for (CompositionLayer const* layer : children)
        layer->render(renderer, camera);
}

void LayerGroup::update(uint32_t delta_ms)
{
    for (CompositionLayer* layer : children)
        layer->update(delta_ms);
}
