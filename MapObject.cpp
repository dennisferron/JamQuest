#include "MapObject.hpp"

#include <stdexcept>

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

EllipseMapObject::EllipseMapObject(const tmx_object* obj)
{

}

void EllipseMapObject::render(SDL_Renderer* renderer, const Camera2D& camera) const
{

}

TileSpriteMapObject::TileSpriteMapObject(tmx_map const* map, const tmx_object* obj) 
{
    ctr_pos_w = {obj->x, obj->y};
    
    obj_size_w = {0,0};
    
    if (obj->width != 0)
        obj_size_w.x = obj->width;
    else if (obj->template_ref != nullptr)
        obj_size_w.x = obj->template_ref->object->width;

    if (obj_size_w.x == 0)
        throw std::runtime_error("Neither object nor template defines width.");

    if (obj->height != 0)
        obj_size_w.y = obj->height;
    else if (obj->template_ref != nullptr)
        obj_size_w.y = obj->template_ref->object->height;

    if (obj_size_w.y == 0)
        throw std::runtime_error("Neither object nor template defines height.");

    angle_degrees = 0;

    if (obj->rotation != 0)
        angle_degrees = obj->rotation;
    else if (obj->template_ref != nullptr)
        angle_degrees = obj->template_ref->object->rotation;

    unsigned int raw_gid = obj->content.gid;

    if (raw_gid == 0 && obj->template_ref != nullptr)
    {
        raw_gid = obj->template_ref->object->content.gid;
    }

    if (raw_gid == 0)
    {
        throw std::runtime_error("Neither tile/sprite map object nor its template defines a GID.");
    }

    unsigned int flags = raw_gid & ~TMX_FLIP_BITS_REMOVAL;

    flip_flags = static_cast<SDL_RendererFlip>(
        (flags & TMX_FLIPPED_HORIZONTALLY ? (int)SDL_FLIP_HORIZONTAL : 0)
    |   (flags & TMX_FLIPPED_VERTICALLY ? (int)SDL_FLIP_VERTICAL : 0));

    unsigned int gid = raw_gid & TMX_FLIP_BITS_REMOVAL;
    tmx_tile const* tile = map->tiles[gid];

    tmx_obj_alignment alignment = tile->tileset->objectalignment;

    // Default is bottom left except in ISO mode.
    if (alignment == OA_NONE && map->orient == O_ISO)
        alignment = OA_BOTTOM;

    // Factoring out the object alignment offset into separate
    // switch-cases for the x and y coordinates means we only
    // have to handle N+M cases instead of N*M cases.
    switch (alignment)
    {
        case OA_RIGHT:
        case OA_TOPRIGHT:
        case OA_BOTTOMRIGHT:
            ofs_ctr_obj.x = obj_size_w.x;
        case OA_TOP:
        case OA_BOTTOM:
        case OA_CENTER:
            ofs_ctr_obj.x = obj_size_w.x / 2.0;
        default:
            ofs_ctr_obj.x = 0.0;
    }
    switch (alignment)
    {
        case OA_BOTTOM:
        case OA_BOTTOMLEFT:
        case OA_BOTTOMRIGHT:
            ofs_ctr_obj.y = obj_size_w.y;
        case OA_LEFT:
        case OA_RIGHT:
        case OA_CENTER:
            ofs_ctr_obj.y = obj_size_w.y / 2.0;
        default:
            ofs_ctr_obj.y = 0.0;
    }

    frames = get_animation_frames(tile);
}

void TileSpriteMapObject::render(SDL_Renderer* renderer, const Camera2D& camera) const
{
    Vector2Df tile_ul_w = ctr_pos_w - ofs_ctr_obj;
    Vector2Df tile_br_w = tile_ul_w + obj_size_w;

    Vector2Df ctr_pos_vp = camera.world_to_viewport(ctr_pos_w);
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
            angle_degrees,
            nullptr, /*rotate around dest rect center*/
            flip_flags );
}
