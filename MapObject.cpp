#include "MapObject.hpp"

MapObject::~MapObject()
{
}

RectangleMapObject::RectangleMapObject(const SDL_Rect& rect)
    : rect(rect)
{
}

void RectangleMapObject::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    // And then convert to viewport for dest rect.
    Vector2D pos_vp = camera.world_to_viewport({
        (double)rect.x, (double)rect.y
    });

    // We also need to map source size to destination size.
    Vector2D sz_vp = camera.size_in_viewport({
        (double)rect.w, (double)rect.h
    });

    SDL_Rect rect_vp =
    {
        (int)pos_vp.x, (int)pos_vp.y,
        (int)sz_vp.x, (int)sz_vp.y
    };

    SDL_RenderDrawRect(renderer, &rect_vp);
}

RectangleMapObject::RectangleMapObject(tmx_object const* obj)
    : rect({(int)obj->x, (int)obj->y, (int)obj->width, (int)obj->height})
{
}

PolyMapObject::PolyMapObject(Vector2D const& pos, const std::vector<Vector2D>& points, bool closed)
    : pos(pos), points(points), closed(closed)
{
}

std::vector<Vector2D> PolyMapObject::conv_points(const tmx_shape* shape)
{
    std::vector<Vector2D> result;

    for (int i=0; i<shape->points_len; i++)
    {
        result.push_back(
            {
                (double)shape->points[i][0],
                (double)shape->points[i][1]
            });
    }

    return result;
}

PolyMapObject::PolyMapObject(const tmx_object* obj) :
    pos({(double)obj->x, (double)obj->y}),
    points(conv_points(obj->content.shape)),
    closed(obj->obj_type == OT_POLYGON)
{
}

void PolyMapObject::draw_line(SDL_Renderer* renderer, const Camera2D& camera, const Vector2D& p0, const Vector2D& p1) const
{
    Vector2D p0_vp = camera.world_to_viewport(pos + p0);
    Vector2D p1_vp = camera.world_to_viewport(pos + p1);

    int x0 = (int)p0_vp.x;
    int y0 = (int)p0_vp.y;
    int x1 = (int)p1_vp.x;
    int y1 = (int)p1_vp.y;
    SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
}

void PolyMapObject::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    for (std::size_t i=1; i<points.size(); i++)
    {
        draw_line(renderer, camera, points[i-1], points[i]);
    }

    if (points.size() > 2 && closed)
    {
        draw_line(renderer, camera, points[points.size()-1], points[0]);
    }
}

EllipseMapObject::EllipseMapObject(const tmx_object* obj)
{

}

void EllipseMapObject::render(SDL_Renderer* renderer, const Camera2D& camera) const
{

}

TileMapObject::TileMapObject(const tmx_object* obj)
{

}

void TileMapObject::render(SDL_Renderer* renderer, const Camera2D& camera) const
{

}
