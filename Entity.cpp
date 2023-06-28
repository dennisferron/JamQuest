#include "Entity.hpp"

using namespace jq;

Entity::Entity(
        MapObject* view_obj,
        btRigidBody* phys_obj
    ) :
        view_obj(view_obj),
        phys_obj(phys_obj)
{
}
