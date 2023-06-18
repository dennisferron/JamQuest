#pragma once

#include "SdlApplication.hpp"
#include "TiledMapRenderer.hpp"
#include "ZoomableView.hpp"

#include "btBulletDynamicsCommon.h"
#include <tmx.h>

class TmxExample : public SdlApplication
{
private:
    TiledMapRenderer* map_renderer = nullptr;
    ZoomableView* zoom_view = nullptr;

    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;

    btDiscreteDynamicsWorld* dynamicsWorld;

    int test_bullet();

public:
    TmxExample();
    ~TmxExample();
    void gameLoop() override;
};
