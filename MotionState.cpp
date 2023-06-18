#include "MotionState.hpp"

using namespace jq;

MotionState::MotionState(btTransform const& trans_, btTransform const& centerOfMassOffset_)
        : trans(trans_), centerOfMassOffset(centerOfMassOffset_)
{
}

void MotionState::setWorldTransform(btTransform const& worldTrans)
{
    trans = worldTrans;
}

void MotionState::getWorldTransform(btTransform& worldTrans) const
{
    worldTrans = trans;
}
