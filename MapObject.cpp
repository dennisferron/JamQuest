#include "MapObject.hpp"

#include <stdexcept>

constexpr double PI = 3.14159265359;

MapObject::~MapObject()
{
}

RectangleMapObject::RectangleMapObject(const SDL_Rect& rect)
    : rect(rect)
{
}

void RectangleMapObject::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    Vector2Df p0_vp = camera.world_to_viewport({(double)rect.x, (double)rect.y});
    Vector2Df p1_vp = camera.world_to_viewport({(double)rect.x + rect.w, (double)rect.y + rect.h });

    SDL_Rect rect_vp = {
        (int)p0_vp.x,
        (int)p0_vp.y,
        (int)(p1_vp.x - p0_vp.x),
        (int)(p1_vp.y - p0_vp.y)
    };
    SDL_RenderDrawRect(renderer, &rect_vp);
}

RectangleMapObject::RectangleMapObject(tmx_object const* obj)
    : rect({(int)obj->x, (int)obj->y, (int)obj->width, (int)obj->height})
{
}

void RectangleMapObject::set_pos(const Vector2Df& new_pos)
{
    rect.x = (int)new_pos.x;
    rect.y = (int)new_pos.y;
}

PolyMapObject::PolyMapObject(Vector2Df const& pos, const std::vector<Vector2Df>& points, bool closed)
    : pos(pos), points(points), closed(closed)
{
}

std::vector<Vector2Df> PolyMapObject::conv_points(const tmx_shape* shape)
{
    std::vector<Vector2Df> result;

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

void PolyMapObject::draw_line(SDL_Renderer* renderer, const Camera2D& camera, const Vector2Df& p0, const Vector2Df& p1) const
{
    Vector2Df p0_vp = camera.world_to_viewport(pos + p0);
    Vector2Df p1_vp = camera.world_to_viewport(pos + p1);

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

void PolyMapObject::set_pos(const Vector2Df& new_pos)
{
    pos = new_pos;
}

EllipseMapObject::EllipseMapObject(const tmx_object* obj)
{

}

void EllipseMapObject::render(SDL_Renderer* renderer, const Camera2D& camera) const
{

}

void EllipseMapObject::set_pos(const Vector2Df& new_pos)
{

}

void TileSpriteMapObject::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    Vector2Df ctr_pos_w_ = ctr_pos_w;
    double angle_degrees_ = angle_degrees;

    if (motion_state)
    {
        btTransform trans;
        motion_state->getWorldTransform(trans);
        ctr_pos_w_ = { trans.getOrigin().x(), trans.getOrigin().y() };
        btScalar yawZ, pitchY, rollX;
        trans.getRotation().getEulerZYX(yawZ, pitchY, rollX);
        angle_degrees_ = 180 * yawZ / PI;
    }

    Vector2Df tile_ul_w = ctr_pos_w_ - ofs_ctr_obj;
    Vector2Df tile_br_w = tile_ul_w + obj_size_w;

    Vector2Df ctr_pos_vp = camera.world_to_viewport(ctr_pos_w_);
    Vector2Df tile_ul_vp = camera.world_to_viewport(tile_ul_w);
    Vector2Df tile_br_vp = camera.world_to_viewport(tile_br_w);
    Vector2Df dest_sz_vp = tile_br_vp - tile_ul_vp;

    SDL_Rect dest_rect = {
            (int)tile_ul_vp.x,
            (int)tile_ul_vp.y,
            (int)dest_sz_vp.x,
            (int)dest_sz_vp.y
    };

    // Determine which animation frame to display.
    AnimationFrame const& frame = frames[0];

    SDL_RenderCopyEx(
            renderer,
            frame.source_image,
            &frame.source_region,
            &dest_rect,
            angle_degrees_,
            nullptr, /*rotate around dest rect center*/
            flip_flags );
}

void TileSpriteMapObject::set_pos(const Vector2Df& new_pos)
{
    ctr_pos_w = new_pos;
}

TileSpriteMapObject::TileSpriteMapObject(const TileSpriteMapObject::ConstructionInfo& info)
        :
    frames(info.frames),
    ctr_pos_w(info.ctr_pos_w),
    obj_size_w(info.obj_size_w),
    ofs_ctr_obj(info.ofs_ctr_obj),
    angle_degrees(info.angle_degrees),
    flip_flags(info.flip_flags)
{
}

void TileSpriteMapObject::set_motion_state(btMotionState* motion_state)
{
    this->motion_state = motion_state;

    if (motion_state)
    {
        btTransform trans;
        trans.setIdentity();
        trans.setOrigin({(btScalar)ctr_pos_w.x, (btScalar)ctr_pos_w.y, 0});
        btQuaternion quat;
        quat.setRotation({0,0,1},
             (btScalar)(angle_degrees * PI / 180));
        trans.setRotation(quat);
        motion_state->setWorldTransform(trans);
    }
}