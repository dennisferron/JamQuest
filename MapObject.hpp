#pragma once

#include "Camera2D.hpp"
#include "Animation.hpp"

#include <tmx.h>
#include <SDL2/SDL.h>

#include <vector>

class MapObject
{
public:
    virtual ~MapObject() = 0;
    virtual void render(SDL_Renderer* renderer, Camera2D const& camera) const = 0;
};

// Used for OT_SQUARE
class RectangleMapObject : public MapObject
{
private:
    SDL_Rect rect;

public:
    RectangleMapObject(tmx_object const* obj);
    RectangleMapObject(SDL_Rect const& rect);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
};

// Used for OT_POLYGON and OT_POLYLINE
class PolyMapObject : public MapObject
{
private:
    Vector2D pos;
    std::vector<Vector2D> points;
    bool closed;

    void draw_line(SDL_Renderer* renderer, Camera2D const& camera,
                   Vector2D const& p0, Vector2D const& p1) const;

    static std::vector<Vector2D> conv_points(tmx_shape const* shape);
public:
    PolyMapObject(Vector2D const& pos, std::vector<Vector2D> const& points, bool closed);
    PolyMapObject(tmx_object const* obj);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
};

// Used for OT_ELLIPSE
class EllipseMapObject : public MapObject
{
private:
    // TODO: implement this

public:
    EllipseMapObject(tmx_object const* obj);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
};

// Used for tiles displayed as map objects
class TileSpriteMapObject : public MapObject
{
private:
    std::vector<AnimationFrame> frames;
    Vector2D ctr_pos_w;
    Vector2D obj_size_w;
    Vector2D ofs_ctr_obj;
    double angle_degrees = 0;
    SDL_RendererFlip flip_flags = SDL_FLIP_NONE;

    static Vector2D calc_alignment_ofs(tmx_map_orient map_orient, tmx_obj_alignment obj_align, double obj_w, double obj_h);

public:
    TileSpriteMapObject(tmx_map const* map, tmx_object const* obj);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
};
