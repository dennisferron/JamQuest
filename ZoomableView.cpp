#include "ZoomableView.hpp"

#include <stdexcept>
#include <sstream>

ZoomableView::ZoomableView(
        SDL_Renderer* renderer,
        int width,
        int height)
    : canvas(nullptr), width(width), height(height)
{
    canvas = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                width,
                                height);

    if (canvas == nullptr)
    {
        std::stringstream msg;
        msg << "Unable to create streamable blank texture! SDL Error: "
            << SDL_GetError() << "\n";
        throw std::logic_error(msg.str());
    }
}

ZoomableView::~ZoomableView()
{
    SDL_DestroyTexture(canvas);
}

void ZoomableView::render(SDL_Renderer* renderer, const CompositionLayer* composition, Camera2D camera)
{
    Vector2Df canvas_size = {(double)width, (double)height };
    Vector2Df tile_size_in_canvas =
            floor(canvas_size / camera.get_extent());
    Vector2Df view_in_canvas =
            tile_size_in_canvas * camera.get_extent();

    Camera2D adj_camera = camera;
    adj_camera.set_view_size(view_in_canvas);

    SDL_Texture* old_target = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, canvas);
    composition->render(renderer, adj_camera);
    SDL_SetRenderTarget(renderer, old_target);

    SDL_Rect source_region = {
            0, 0, (int)view_in_canvas.x, (int)view_in_canvas.y
    };

    SDL_RenderCopyEx(
            renderer,
            canvas,
            &source_region,
            nullptr, // dest_rect
            0,
            nullptr,
            SDL_RendererFlip::SDL_FLIP_NONE );
}
