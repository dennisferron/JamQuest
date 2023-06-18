#include "Entity.hpp"

using namespace jq;

Entity::Entity(
        MapObject* view_obj,
        btRigidBody* phys_obj,
        const btTransform& transform,
        const btTransform& centerOfMassOffset
    ) :
        view_obj(view_obj),
        phys_obj(phys_obj),
        transform(transform),
        centerOfMassOffset(centerOfMassOffset)
{
}

void Entity::setWorldTransform(btTransform const& worldTrans)
{
    transform = worldTrans;

    if (view_obj)
    {
        auto p = transform.getOrigin();
        view_obj->set_pos({p.getX(), p.getY()});
    }
}

void Entity::getWorldTransform(btTransform& worldTrans) const
{
    worldTrans = transform;
}
