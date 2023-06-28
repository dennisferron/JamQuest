#pragma once

#include "CompositionLayer.hpp"

#include "btBulletDynamicsCommon.h"
#include <tmx.h>

#include <vector>

class LayerGroup : public CompositionLayer
{
private:
    std::vector<CompositionLayer*> children;

public:
    LayerGroup(std::string name, std::vector<CompositionLayer*> children);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
    void update(uint32_t delta_ms) override;
    void debug_print(int indent) const override;
    std::vector<CompositionLayer*> const& get_children() const
        { return children; }
};
