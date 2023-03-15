#pragma once

#include <cmath>

template <typename T>
struct Vector2D
{
    T x;
    T y;
};

using Vector2Df = Vector2D<double>;
using Vector2Di = Vector2D<int>;

inline Vector2Df floor(Vector2Df const& a)
{
    return { std::floor(a.x), std::floor(a.y) };
}

inline Vector2Df ceil(Vector2Df const& a)
{
    return { std::ceil(a.x), std::ceil(a.y) };
}

template <typename T>
Vector2D<T> operator +(Vector2D<T> const& a, Vector2D<T> const& b)
{
    return { a.x + b.x, a.y + b.y };
}

template <typename T>
Vector2D<T> operator -(Vector2D<T> const& a, Vector2D<T> const& b)
{
    return { a.x - b.x, a.y - b.y };
}

template <typename T>
Vector2D<T> operator *(Vector2D<T> const& a, Vector2D<T> const& b)
{
    return { a.x * b.x, a.y * b.y };
}

template <typename T>
Vector2D<T> operator /(Vector2D<T> const& a, Vector2D<T> const& b)
{
    return { a.x / b.x, a.y / b.y };
}

template <typename T>
Vector2D<T> operator /(Vector2D<T> const& a, T const& b)
{
    return { a.x / b, a.y / b };
}

template <typename T>
Vector2D<T> operator *(T const& a, Vector2D<T> const& b)
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

inline Vector2Df sqrt_sign(Vector2Df const& a)
{
    return { sqrt_sign(a.x), sqrt_sign(a.y) };
}

inline Vector2Df abs(Vector2Df const& a)
{
    return { fabs(a.x), fabs(a.y) };
}

template <typename T>
struct Rect2D
{
    T left;
    T top;
    T right;
    T bottom;
};

using Rect2Di = Rect2D<int>;
using Rect2Df = Rect2D<double>;

template <typename T>
Rect2D<T> operator +(Rect2D<T> const& a, Vector2D<T> const& b)
{
    return { a.left + b.x, a.top + b.y,
             a.right + b.x, a.bottom + b.y};
}

template <typename T>
Rect2D<T> operator -(Rect2D<T> const& a, Vector2D<T> const& b)
{
    return { a.left - b.x, a.top - b.y,
             a.right - b.x, a.bottom - b.y};
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
    Vector2Df view_size;

    /*
     * Camera center and extent are
     * given in world coordinates. This makes it easy to
     * visualize what will be visible regardless of viewport.
     * Visible map area is -0.5*extent+center to 0.5*extent+center.
     */
    Vector2Df extent;
    Vector2Df center;

public:
    // View size should be logical size of the SDL viewport.
    // Extent and center will be derived from view size to
    // achieve a default 1:1 mapping of world to viewport.
    Camera2D(Vector2Df const& view_size)
            : view_size(view_size), extent(view_size), center(0.5*view_size)
    {
    }

    Camera2D(int viewport_w, int viewport_h) :
            Camera2D(Vector2Df {
                    static_cast<double>(viewport_w),
                    static_cast<double>(viewport_h)
            }) {}

    Vector2Df world_to_viewport(const Vector2Df& w) const
    {
        return view_size * (w - center) / extent + 0.5*view_size;
    }

    Vector2Df viewport_to_world(const Vector2Df& v) const
    {
        return extent * (v - 0.5*view_size) / view_size + center;
    }

    Vector2Df const& get_view_size() const
        { return view_size; }

    Vector2Df const& get_extent() const
        { return extent; }

    // Set camera view extent in world coordinates.
    void set_view_size(Vector2Df const& new_view_size)
        { view_size = new_view_size; }

    // Set camera view extent in world coordinates.
    void set_extent(Vector2Df const& new_extent)
        { extent = new_extent; }

    void set_center(Vector2Df const& new_center)
        { center = new_center; }

    Rect2Df world_rect() const
    {
        Vector2Df p0 = center - 0.5*extent;
        Vector2Df p1 = center + 0.5*extent;
        return
        {
            p0.x,
            p0.y,
            p1.x,
            p1.y
        };
    }
};
