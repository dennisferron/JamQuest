#pragma once

#include "SdlApplication.hpp"
#include "TileGridLayer.hpp"
#include "LayerGroup.hpp"

#include <tmx.h>

class TmxExample : public SdlApplication
{
private:
    static SDL_Renderer* texture_loader_renderer;
    tmx_map *map = nullptr;

    LayerGroup* root_layer_group = nullptr;

    static void* SDL_tex_loader(const char *path);

public:
    TmxExample();
    ~TmxExample();
    void gameLoop() override;
};
