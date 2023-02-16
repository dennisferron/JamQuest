#pragma once

#include <cmath>

struct Vector2D
{
    double x;
    double y;
};

inline Vector2D floor(Vector2D const& a)
{
    return { std::floor(a.x), std::floor(a.y) };
}

inline Vector2D ceil(Vector2D const& a)
{
    return { std::ceil(a.x), std::ceil(a.y) };
}

inline Vector2D operator +(Vector2D const& a, Vector2D const& b)
{
    return { a.x + b.x, a.y + b.y };
}

inline Vector2D operator -(Vector2D const& a, Vector2D const& b)
{
    return { a.x - b.x, a.y - b.y };
}

inline Vector2D operator *(Vector2D const& a, Vector2D const& b)
{
    return { a.x * b.x, a.y * b.y };
}

inline Vector2D operator /(Vector2D const& a, Vector2D const& b)
{
    return { a.x / b.x, a.y / b.y };
}

inline Vector2D operator /(Vector2D const& a, double b)
{
    return { a.x / b, a.y / b };
}

inline Vector2D operator *(double a, Vector2D const& b)
{
    return { a * b.x, a * b.y };
}

inline double sqrt_sign(double x)
{
    if (x < 0)
        return -std::sqrt(-x);
    else
        return std::sqrt(x);
}

inline Vector2D sqrt_sign(Vector2D const& a)
{
    return { sqrt_sign(a.x), sqrt_sign(a.y) };
}

inline Vector2D abs(Vector2D const& a)
{
    return { fabs(a.x), fabs(a.y) };
}

class Camera2D
{
private:
    /*
     * Output scaling is determined by letting the Camera2D know
     * the logical viewport height and width as set via SDL.
     * Draw area is {0,0} (inclusive) to view_size (exclusive).
     * Camera2D::center maps to center of viewport (view_size / 2).
     * We intentionally do not provide a variable to change the
     * offset where the camera center or topleft corner maps to a
     * screen location, because SDL viewport should be used instead
     * if you need to change where the camera view maps onscreen.
     * (Using SDL viewport ensures drawing clips to viewport boundaries;
     * Camera2D settings alone do not prevent out of bounds drawing.)
     */
    Vector2D view_size;

    /*
     * Camera center and extent are
     * given in world coordinates. This makes it easy to
     * visualize what will be visible regardless of viewport.
     * Visible map area is -0.5*extent+center to 0.5*extent+center.
     */
    Vector2D extent;
    Vector2D center;

public:
    // View size should be logical size of the SDL viewport.
    // Extent and center will be derived from view size to
    // achieve a default 1:1 mapping of world to viewport.
    Camera2D(Vector2D const& view_size);

    Camera2D(int viewport_w, int viewport_h) :
        Camera2D(Vector2D {
            static_cast<double>(viewport_w),
            static_cast<double>(viewport_h)
        }) {}

    Vector2D world_to_viewport(Vector2D const& w) const;
    Vector2D viewport_to_world(Vector2D const& v) const;

    Vector2D const& get_view_size() const
        { return view_size; }

    // Set camera view extent in world coordinates.
    void set_extent(Vector2D const& new_extent)
        { extent = new_extent; }

    void set_center(Vector2D const& new_center)
        { center = new_center; }
};
