#pragma once

#include "MapObject.hpp"
#include "MotionState.hpp"

#include "btBulletDynamicsCommon.h"

namespace jq
{

    class Entity : public btMotionState
    {
    private:
        MapObject* view_obj;
        btRigidBody* phys_obj;
        btTransform transform;
        btTransform centerOfMassOffset;

    public:

        // Bullet stuff
        virtual void setWorldTransform(btTransform const& worldTrans);
        virtual void getWorldTransform(btTransform& worldTrans) const;

        Entity(
            MapObject* view_obj,
            btRigidBody* phys_obj,
            btTransform const& transform,
            btTransform const& centerOfMassOffset);

    };

}
