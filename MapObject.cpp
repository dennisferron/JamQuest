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
    SDL_RenderDrawRect(renderer, &rect);
}

RectangleMapObject::RectangleMapObject(tmx_object const* obj)
    : rect({(int)obj->x, (int)obj->y, (int)obj->width, (int)obj->height})
{
}

PolyMapObject::PolyMapObject(SDL_Point const& pos, const std::vector<SDL_Point>& points, bool closed)
    : pos(pos), points(points), closed(closed)
{
}

std::vector<SDL_Point> PolyMapObject::conv_points(const tmx_shape* shape)
{
    std::vector<SDL_Point> result;

    for (int i=0; i<shape->points_len; i++)
    {
        result.push_back(
            {
                (int)shape->points[i][0],
                (int)shape->points[i][1]
            });
    }

    return result;
}

PolyMapObject::PolyMapObject(const tmx_object* obj) :
    pos({(int)obj->x, (int)obj->y}),
    points(conv_points(obj->content.shape)),
    closed(obj->obj_type == OT_POLYGON)
{
}

void PolyMapObject::draw_line(SDL_Renderer* renderer, const Camera2D& camera, const SDL_Point& p0, const SDL_Point& p1) const
{
    int x0 = pos.x + p0.x;
    int y0 = pos.y + p0.y;
    int x1 = pos.x + p1.x;
    int y1 = pos.y + p1.y;
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
