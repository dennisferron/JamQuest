#include "LayerGroup.hpp"
#include "ObjectLayer.hpp"
#include "ImageLayer.hpp"
#include "TileGridLayer.hpp"

LayerGroup::LayerGroup(std::string name, std::vector<CompositionLayer*> children)
    : CompositionLayer(name), children(children)
{
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

void LayerGroup::debug_print(int indent) const
{
    CompositionLayer::debug_print(indent);

    for (auto layer : children)
        layer->debug_print(indent+1);
}
