#include "TmxExample.hpp"


SDL_Renderer* TmxExample::texture_loader_renderer = nullptr;

void* TmxExample::SDL_tex_loader(const char *path)
{
    return IMG_LoadTexture(texture_loader_renderer, path);
}

TmxExample::TmxExample()
{
    texture_loader_renderer = renderer;

    /* Set the callback globs in the main function */
    tmx_img_load_func = SDL_tex_loader;
    tmx_img_free_func = (void (*)(void*))SDL_DestroyTexture;

    map = tmx_load("res/maps/objecttemplates.tmx");
    if (!map)
    {
        tmx_perror("Cannot load map");
    }

    root_layer_group = new LayerGroup(map, map->ly_head);
}

TmxExample::~TmxExample()
{

}

void TmxExample::gameLoop()
{
    SDL_Event ev;

    while (SDL_PollEvent(&ev))
    {
        switch (ev.type)
        {
            case SDL_QUIT:
                gameRunning = false;
                break;
            default:
                break;
        }
    };

    tmx_col_bytes col = tmx_col_to_bytes(map->backgroundcolor);
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    SDL_RenderClear(renderer);

    int viewport_w;
    int viewport_h;
    SDL_RenderGetLogicalSize(renderer, &viewport_w, &viewport_h);

    Camera2D camera(viewport_w, viewport_h);

    camera.set_extent({1600, 960});
    camera.set_center({800, 480});

    root_layer_group->render(renderer, camera);

    SDL_RenderPresent(renderer);
}
