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
    while (head) {
        if (head->visible) {
            switch (head->obj_type)
            {
                case OT_SQUARE:
                    objects.push_back(new RectangleMapObject(head));
                    break;
                case OT_POLYGON:
                case OT_POLYLINE:
                    objects.push_back(new PolyMapObject(head));
                    break;
//                case OT_ELLIPSE:
//                    objects.push_back(new EllipseMapObject(head));
//                    break;
                case OT_TILE:
                    objects.push_back(new TileSpriteMapObject(map, head));
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
        }
        head = head->next;
    }

    return new ObjectLayer(
            layer->name,
            layer->content.objgr->color,
            objects);
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
                    cell_trans.setOrigin(
                            btVector3(
                                    static_cast<btScalar>(col),
                                    static_cast<btScalar>(row),
                                    0.0f));

                    auto* co = cells[cell_index].phys_obj = new btCollisionObject();
                    co->setCollisionShape(our_tile->phys_shape);
                    co->setWorldTransform(cell_trans);
                    co->setActivationState(ISLAND_SLEEPING); // very important (otherwise bodies on it might not fall asleep!)
                    world->addCollisionObject(co,
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

