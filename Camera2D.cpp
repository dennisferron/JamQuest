#include "Camera2D.hpp"

Camera2D::Camera2D(Vector2D const& view_size)
    : view_size(view_size), extent(view_size), center(view_size/2)
{

}

Vector2D Camera2D::world_to_viewport(const Vector2D& w) const
{
    return view_size * (w - center) / extent + view_size/2;
}

Vector2D Camera2D::viewport_to_world(const Vector2D& v) const
{
    return extent * (v - view_size/2) / view_size + center;
}
