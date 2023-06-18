#pragma once

#include "TiledMapRenderer.hpp"

class LayerGroup;
class ObjectLayer;
class TileGridLayer;
class ImageLayer;

namespace jq
{
    class TmxLoader
    {
    private:
        static SDL_Renderer* texture_loader_renderer;
        static void* SDL_tex_loader(const char *path);

        tmx_map *map = nullptr;
        btDynamicsWorld* world = nullptr;

        TiledMapRenderer* map_renderer;

        LayerGroup* load_layer_group(std::string name, tmx_layer const* list_head);
        ObjectLayer* load_object_layer(const tmx_layer* layer);
        ImageLayer* load_image_layer(tmx_layer const* layer);
        TileGridLayer* load_tile_grid_layer(tmx_layer const* layer);

    public:
        TmxLoader(std::string map_file,
                  SDL_Renderer* sdl_renderer,
                  btDiscreteDynamicsWorld* dynamicsWorld);
        TiledMapRenderer* get_map_renderer() const;
    };
}
