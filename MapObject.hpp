#pragma once

#include "Camera2D.hpp"

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
class TileMapObject : public MapObject
{
private:
    // TODO: implement this

public:
    TileMapObject(tmx_object const* obj);
    void render(SDL_Renderer* renderer, Camera2D const& camera) const override;
};
