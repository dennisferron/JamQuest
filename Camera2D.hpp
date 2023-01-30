#pragma once

struct Vector2D
{
    double x;
    double y;
};

struct Camera2D
{
    /*
     * For user convenience, camera center and extent are
     * given in world coordinates. This makes it easy to
     * visualize what will be visible regardless of viewport.
     * Visible map area is -0.5*extent+center to 0.5*extent+center.
     */
    Vector2D center;
    Vector2D extent;

    /*
     * For user convenience, output scaling is determined by just
     * communicating the viewport logical size to the camera.
     * The drawing area is from 0,0 to view_size (using <, not <=).
     * The camera center draws at view_size / 2 by definition.
     * We intentionally do not provide a variable to change the
     * location (in screen pixels) where the camera center or topleft
     * corner starts, because an SDL viewport should be used instead
     * if you need to change where view 0,0 maps to screen or window.
     */
    Vector2D view_size;
};
