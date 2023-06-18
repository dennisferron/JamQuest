#pragma once

#include "btBulletDynamicsCommon.h"

namespace jq
{
    class MotionState : public btMotionState
    {
    private:
        btTransform trans;
        btTransform centerOfMassOffset;

        void sync();
        void sync_to_irrlicht() const;

    public:

        // Bullet stuff
        virtual void setWorldTransform(btTransform const& worldTrans);
        virtual void getWorldTransform(btTransform& worldTrans) const;

        MotionState(btTransform const& trans_, btTransform const& centerOfMassOffset_);

    };
}