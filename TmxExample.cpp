#include "TmxExample.hpp"

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

SDL_Renderer* TmxExample::texture_loader_renderer = nullptr;

void* TmxExample::SDL_tex_loader(const char *path)
{
    void* result = IMG_LoadTexture(texture_loader_renderer, path);
    if (!result)
    {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    return result;
}

TmxExample::TmxExample()
{
    texture_loader_renderer = renderer;

    /* Set the callback globs in the main function */
    tmx_img_load_func = SDL_tex_loader;
    tmx_img_free_func = (void (*)(void*))SDL_DestroyTexture;

    std::string map_file = "res/maps/gameart2d-desert.tmx";
    map = tmx_load(map_file.c_str());
    if (!map)
    {
        std::stringstream msg;
        msg << "Unable to load TMX map file "
            << map_file;
        tmx_perror(msg.str().c_str());

        std::string path = "res/maps";
        std::cout << "Files in directory: " << path << std::endl;
        for (const auto & entry : fs::directory_iterator(path))
            std::cout << entry.path() << std::endl;

        throw std::runtime_error(msg.str());
    }

    map_renderer = new TiledMapRenderer(map);
    zoom_view = new ZoomableView(renderer, 160, 144);
}

TmxExample::~TmxExample()
{

}

void TmxExample::gameLoop()
{
    static double total_time = 0;

    uint32_t elapsed_time_ms = get_elapsed_time();
    total_time += elapsed_time_ms;

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

    map_renderer->update(elapsed_time_ms);

    int viewport_w;
    int viewport_h;
    SDL_RenderGetLogicalSize(renderer, &viewport_w, &viewport_h);

    Camera2D camera(viewport_w, viewport_h);

    camera.set_extent({32, 20});
    camera.set_center({0.001 * total_time + 10,
                       6 + 12 * sin(0.00001 * total_time)});

    //map_renderer->render(renderer, camera);
    zoom_view->render(renderer, camera, map_renderer);

    SDL_RenderPresent(renderer);
}
