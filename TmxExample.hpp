#pragma once

#include "SdlApplication.hpp"
#include "TiledMapRenderer.hpp"
#include "ZoomableView.hpp"

#include <tmx.h>

class TmxExample : public SdlApplication
{
private:
    static SDL_Renderer* texture_loader_renderer;
    tmx_map *map = nullptr;

    TiledMapRenderer* map_renderer = nullptr;
    ZoomableView* zoom_view = nullptr;

    static void* SDL_tex_loader(const char *path);

public:
    TmxExample();
    ~TmxExample();
    void gameLoop() override;
};
