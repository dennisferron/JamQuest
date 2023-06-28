//#pragma once
//
//#include "btBulletDynamicsCommon.h"
//
//namespace jq
//{
//    class Entity;
//
//    class MotionState : public btDefaultMotionState
//    {
//    private:
//        btTransform m_graphicsWorldTrans;
//        btTransform m_centerOfMassOffset;
//        btTransform m_startWorldTrans;
//        void* m_userPointer;
//
//        MotionState(const btTransform& startTrans = btTransform::getIdentity(), const btTransform& centerOfMassOffset = btTransform::getIdentity())
//                : m_graphicsWorldTrans(startTrans),
//                  m_centerOfMassOffset(centerOfMassOffset),
//                  m_startWorldTrans(startTrans),
//                  m_userPointer(0)
//
//        {
//        }
//
//        ///synchronizes world transform from user to physics
//        virtual void getWorldTransform(btTransform & centerOfMassWorldTrans) const
//        {
//            centerOfMassWorldTrans = m_graphicsWorldTrans * m_centerOfMassOffset.inverse();
//        }
//
//        ///synchronizes world transform from physics to user
//        ///Bullet only calls the update of worldtransform for active objects
//        virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans)
//        {
//            m_graphicsWorldTrans = centerOfMassWorldTrans * m_centerOfMassOffset;
//        }
//
//    private:
//        btTransform trans;
//        btTransform centerOfMassOffset;
//
//        void sync();
//        void sync_to_irrlicht() const;
//
//    public:
//
//        // Bullet stuff
//        virtual void setWorldTransform(btTransform const& worldTrans);
//        virtual void getWorldTransform(btTransform& worldTrans) const;
//
//        MotionState(btTransform const& trans_, btTransform const& centerOfMassOffset_);
//
//    };
//}