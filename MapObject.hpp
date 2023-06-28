#pragma once

#include "Camera2D.hpp"
#include "Animation.hpp"

#include "btBulletDynamicsCommon.h"
#include <tmx.h>
#include <SDL2/SDL.h>

#include <vector>

class MapObject
{
public:
    virtual ~MapObject() = 0;
    virtual void render(SDL_Renderer* renderer, Camera2D const& camera) const = 0;
    virtual void set_pos(Vector2Df const& new_pos) = 0;
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
    virtual void set_pos(Vector2Df const& new_pos) override;
};

// Used for OT_POLYGON and OT_POLYLINE
class PolyMapObject : public MapObject
{
private:
    Vector2Df pos;
    std::vector<Vector2Df> points;
    bool closed;

    void draw_line(SDL_Renderer* renderer, Camera2D const& camera,
                   Vector2Df const& p0, Vector2Df const& p1) const;

    static std::vector<Vector2Df> conv_points(tmx_shape const* shape);
public:
    PolyMapObject(Vector2Df const& pos, std::vector<Vector2Df> const& points, bool closed);
    PolyMapObject(tmx_object const* obj);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
    virtual void set_pos(Vector2Df const& new_pos) override;
};

// Used for OT_ELLIPSE
class EllipseMapObject : public MapObject
{
private:
    // TODO: implement this

public:
    EllipseMapObject(tmx_object const* obj);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
    virtual void set_pos(Vector2Df const& new_pos) override;
};

// Used for tiles displayed as map objects
class TileSpriteMapObject : public MapObject
{
public:
    struct ConstructionInfo
    {
        std::vector<AnimationFrame> frames;

        Vector2Df ctr_pos_w;
        Vector2Df obj_size_w;
        Vector2Df ofs_ctr_obj;
        double angle_degrees = 0;
        SDL_RendererFlip flip_flags = SDL_FLIP_NONE;
    };

private:
    btMotionState* motion_state = nullptr;

    std::vector<AnimationFrame> frames;
    Vector2Df ctr_pos_w;
    Vector2Df obj_size_w;
    Vector2Df ofs_ctr_obj;
    double angle_degrees = 0;
    SDL_RendererFlip flip_flags = SDL_FLIP_NONE;

public:
    TileSpriteMapObject(ConstructionInfo const& info);
    TileSpriteMapObject(tmx_map const* map, tmx_object const* obj);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
    virtual void set_pos(Vector2Df const& new_pos) override;
    void set_motion_state(btMotionState* motion_state);
};
