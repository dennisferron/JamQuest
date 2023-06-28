#pragma once

#include "MapObject.hpp"
#include "MotionState.hpp"

#include "btBulletDynamicsCommon.h"

namespace jq
{

    class Entity
    {
    private:
        MapObject* view_obj;
        btRigidBody* phys_obj;

    public:

        Entity(
            MapObject* view_obj,
            btRigidBody* phys_obj);
    };

}
