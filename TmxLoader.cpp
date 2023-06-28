#include "TmxLoader.hpp"
using namespace jq;

#include "LayerGroup.hpp"
#include "ObjectLayer.hpp"
#include "ImageLayer.hpp"
#include "TileGridLayer.hpp"

#include <SDL2/SDL_image.h>

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

SDL_Renderer* TmxLoader::texture_loader_renderer = nullptr;

void* TmxLoader::SDL_tex_loader(const char *path)
{
    void* result = IMG_LoadTexture(texture_loader_renderer, path);
    if (!result)
    {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    return result;
}

jq::TmxLoader::TmxLoader(std::string map_file,
                         SDL_Renderer* sdl_renderer,
                         btDiscreteDynamicsWorld* dynamicsWorld)
     : world(dynamicsWorld)
{
    texture_loader_renderer = sdl_renderer;

    /* Set the callback globs in the main function */
    tmx_img_load_func = SDL_tex_loader;
    tmx_img_free_func = (void (*)(void*))SDL_DestroyTexture;

    map = tmx_load(map_file.c_str());
    if (!map)
    {
        std::stringstream msg;
        msg << "Unable to load TMX map file "
            << map_file;
        tmx_perror(msg.str().c_str());

        std::string path = "res/maps";
        std::cout << "Files in directory: " << path << std::endl;
        for (const auto & entry : fs::directory_iterator(path))
            std::cout << entry.path() << std::endl;

        throw std::runtime_error(msg.str());
    }

    auto root_layer_group = load_layer_group("root", map->ly_head);
    tmx_col_bytes background_color = tmx_col_to_bytes(map->backgroundcolor);

    map_renderer = new TiledMapRenderer(root_layer_group, background_color);
}

TiledMapRenderer* jq::TmxLoader::get_map_renderer() const
{
    return map_renderer;
}

LayerGroup* TmxLoader::load_layer_group(std::string name, const tmx_layer* list_head)
{
    std::vector<CompositionLayer*> children;

    tmx_layer const* cur_layer = list_head;

    while (cur_layer != nullptr)
    {
        if (cur_layer->visible)
        {
            switch (cur_layer->type)
            {
                case L_GROUP:
                    children.push_back(
                            load_layer_group(
                                    cur_layer->name,
                                    cur_layer->content.group_head));
                    break;
                case L_OBJGR:
                    children.push_back(load_object_layer(cur_layer));
                    break;
                case L_IMAGE:
                    children.push_back(load_image_layer(cur_layer));
                    break;
                case L_LAYER:
                    children.push_back(load_tile_grid_layer(cur_layer));
                    break;
                default:
                    break;
            }
        }
        cur_layer = cur_layer->next;
    }

    return new LayerGroup(name, children);
}

ObjectLayer* TmxLoader::load_object_layer(const tmx_layer* layer)
{
    std::vector<MapObject*> objects;

    tmx_object_group const* objgr = layer->content.objgr;

    tmx_object *head = objgr->head;
    while (head)
    {
        if (head->visible)
        {
            if (auto map_obj = load_object(head))
            {
                objects.push_back(map_obj);
            }
        }
        head = head->next;
    }

    return new ObjectLayer(
            layer->name,
            layer->content.objgr->color,
            objects);
}


MapObject* TmxLoader::load_object(tmx_object const* obj)
{
    MapObject* map_obj = nullptr;

    switch (obj->obj_type)
    {
        case OT_SQUARE:
            map_obj = new RectangleMapObject(obj);
            break;
        case OT_POLYGON:
        case OT_POLYLINE:
            map_obj = new PolyMapObject(obj);
            break;
//                case OT_ELLIPSE:
//                    objects.push_back(new EllipseMapObject(head));
//                    break;
        case OT_TILE:
            map_obj = load_tile_sprite_map_object(obj);

            break;
//                case OT_POINT:
//                    break;
//                case OT_TEXT:
//                    break;
//                case OT_NONE:
//                    break;
        default:
            break;
    }

    return map_obj;
}

TileSpriteMapObject* TmxLoader::load_tile_sprite_map_object(const tmx_object* obj)
{
    // World coordinates are defined in unit tiles, however
    // object coordinates are in pixels, so we need a
    // correction factor to recast pixel coords to tile units.
    Vector2Df px_to_w = {1.0 / map->tile_width, 1.0 / map->tile_height };

    Vector2Df obj_size_px = {0, 0};

    if (obj->width != 0)
        obj_size_px.x = obj->width;
    else if (obj->template_ref != nullptr)
        obj_size_px.x = obj->template_ref->object->width;

    if (obj_size_px.x == 0)
        throw std::runtime_error("Neither object nor template defines width.");

    if (obj->height != 0)
        obj_size_px.y = obj->height;
    else if (obj->template_ref != nullptr)
        obj_size_px.y = obj->template_ref->object->height;

    if (obj_size_px.y == 0)
        throw std::runtime_error("Neither object nor template defines height.");

    Vector2Df obj_size_w = px_to_w * obj_size_px;

    double angle_degrees = 0;

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

    SDL_RendererFlip flip_flags = static_cast<SDL_RendererFlip>(
            (flags & TMX_FLIPPED_HORIZONTALLY ? (int)SDL_FLIP_HORIZONTAL : 0)
            |   (flags & TMX_FLIPPED_VERTICALLY ? (int)SDL_FLIP_VERTICAL : 0));

    unsigned int gid = raw_gid & TMX_FLIP_BITS_REMOVAL;
    tmx_tile const* tile = map->tiles[gid];
    std::vector<AnimationFrame> frames = get_animation_frames(tile);

    // The object position may be relative to any of nine
    // possible alignment locations within the tile image.
    // We compute an alignment vector that tells how you
    // would need to move to get from the center of the
    // tile to the location of the point of alignment.
    Vector2Df alignment_vector;

    constexpr double A_LFT = -0.5;
    constexpr double A_RHT = 0.5;
    constexpr double A_CTR = 0.0;
    constexpr double A_TOP = -0.5;
    constexpr double A_BOT = 0.5;

    switch (tile->tileset->objectalignment)
    {
        case OA_TOPLEFT:
            alignment_vector = { A_LFT, A_TOP };
            break;
        case OA_TOP:
            alignment_vector = { A_CTR, A_TOP };
            break;
        case OA_TOPRIGHT:
            alignment_vector = { A_RHT, A_TOP };
            break;
        case OA_RIGHT:
            alignment_vector = { A_RHT, A_CTR };
            break;
        case OA_BOTTOMRIGHT:
            alignment_vector = { A_RHT, A_BOT };
            break;
        case OA_BOTTOM:
            alignment_vector = { A_CTR, A_BOT };
            break;
        case OA_BOTTOMLEFT:
            alignment_vector = { A_LFT, A_BOT };
            break;
        case OA_LEFT:
            alignment_vector = { A_LFT, A_CTR };
            break;
        case OA_CENTER:
            alignment_vector = { A_CTR, A_CTR };
            break;
        case OA_NONE:
        default:
            // Default is bottom left, except in ISO mode.
            if (map->orient == O_ISO)
                alignment_vector = { A_CTR, A_BOT };
            else
                alignment_vector = { A_LFT, A_BOT };
    }

    // The TMX file gives the object position in pixels, which
    // we must convert to unit tiles to get world coordinates.
    // We subtract the alignment vector (scaled to object size)
    // so that obj_pos_w refers to the center of the object.
    Vector2Df obj_pos_w = px_to_w * Vector2Df {obj->x, obj->y}
                          - alignment_vector * obj_size_w;

    // If the graphical center of mass of the tile sprite is not
    // also centered within the tile rectangle, this offset can
    // also be adjusted to be different than (1/2) * size.
    // The MapObject will subtract this value from the physics
    // position to obtain the upper left SDL rect coordinate.
    Vector2Df ofs_ctr_w = 0.5 * obj_size_w;
    // TODO: check custom object properties for different center.

    auto map_object = new TileSpriteMapObject(
        {
            frames,
            obj_pos_w,
            obj_size_w,
            ofs_ctr_w,
            angle_degrees,
            flip_flags
        }
    );

    auto shape = //new btBoxShape({0.5, 0.5, 0.5});
            new btSphereShape(0.5);
    shapes.push_back(shape);

    btScalar mass = 1.0f;

    btVector3 localInertia(0, 0, 0);
    shape->calculateLocalInertia(mass, localInertia);

    auto motion_state = new btDefaultMotionState();
    map_object->set_motion_state(motion_state);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motion_state, shape, localInertia);
    btRigidBody* phys_obj = new btRigidBody(rbInfo);

    //add the body to the dynamics world
    world->addRigidBody(phys_obj);

    auto entity = new Entity(map_object, phys_obj);
    entities.push_back(entity);

    return map_object;
}


ImageLayer* TmxLoader::load_image_layer(const tmx_layer* layer)
{
    Vector2Df pos_w = { 0, 0 };
    Vector2Df size_w = {
            (double)map->width * map->tile_width,
            (double)map->height * map->tile_height
    };

    auto texture = (SDL_Texture*)layer->content.image->resource_image;

    return new ImageLayer(layer->name, texture, pos_w, size_w);
}

TileGridLayer* TmxLoader::load_tile_grid_layer(const tmx_layer* layer)
{
    TileGridLayer::ConstructionInfo info;

    info.parallax_offset = {0,0};
    info.opacity = (float)layer->opacity;
    info.map_width = map->width;
    info.map_height = map->height;
    info.map_tile_size = {
            (int)map->tile_width,
            (int)map->tile_height
    };

    std::map<uint32_t, Tile*> tiles;
    std::vector<Cell> cells;
    cells.resize(info.map_height * info.map_width);

    for (unsigned int row = 0; row < info.map_height; row++)
    {
        for (unsigned int col = 0; col < info.map_width; col++)
        {
            unsigned int cell_index = (row * info.map_width) + col;
            unsigned int raw_gid = layer->content.gids[cell_index];
            unsigned int gid = raw_gid & TMX_FLIP_BITS_REMOVAL;

            unsigned int flags = raw_gid & ~TMX_FLIP_BITS_REMOVAL;
            cells[cell_index].flip_flags = static_cast<SDL_RendererFlip>(
                    (flags & TMX_FLIPPED_HORIZONTALLY ? (int)SDL_FLIP_HORIZONTAL : 0)
                    |   (flags & TMX_FLIPPED_VERTICALLY ? (int)SDL_FLIP_VERTICAL : 0));

            if (tmx_tile const* map_tile = map->tiles[gid])
            {
                Tile* our_tile;

                auto tile_kv = tiles.find(gid);
                if (tile_kv != tiles.end())
                {
                    our_tile = tile_kv->second;
                }
                else
                {
                    our_tile = tiles[gid] = new Tile();
                    our_tile->frames = get_animation_frames(map_tile);
                    our_tile->phys_shape = new btBoxShape(
                            btVector3(0.5f, 0.5f, 0.5f));
                    // TODO: check map_tile properties for friction, etc.
                }

                cells[cell_index].tile = our_tile;

                if (our_tile->phys_shape)
                {
                    btTransform cell_trans;
                    cell_trans.setIdentity();
                    cell_trans.setOrigin(
                            btVector3(
                                    static_cast<btScalar>(col+0.5),
                                    static_cast<btScalar>(row+0.5),
                                    0.0f));

                    auto* co = new btCollisionObject();
                            //= new btRigidBody(1.,nullptr,our_tile->phys_shape);
                    cells[cell_index].phys_obj = co;
                    co->setCollisionShape(our_tile->phys_shape);
                    co->setWorldTransform(cell_trans);
                    co->setActivationState(ISLAND_SLEEPING); // very important (otherwise bodies on it might not fall asleep!)
//                    world->addRigidBody(
                    world->addCollisionObject(
                        co,
                        btBroadphaseProxy::StaticFilter,
                        btBroadphaseProxy::AllFilter &
                        ~(btBroadphaseProxy::StaticFilter
                         | btBroadphaseProxy::KinematicFilter));
                    co->setCollisionFlags(co->getCollisionFlags()
                                          | btCollisionObject::CF_STATIC_OBJECT
                    ); // This is an additional 'static' flag
                    co->setFriction(our_tile->friction);
                    co->setRestitution(our_tile->restitution);
                }
            }
        }
    }

    return new TileGridLayer(layer->name,
                             cells,
                             tiles,
                             info);
}

