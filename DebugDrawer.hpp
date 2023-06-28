#pragma once

#include "Camera2D.hpp"

#include "btBulletDynamicsCommon.h"
#include <SDL2/SDL.h>

namespace jq
{

    class DebugDrawer : public btIDebugDraw
    {
    private:
        SDL_Renderer* renderer;
        //Vector2Df tile_size;
        Camera2D camera;
        int debug_mode = DBG_DrawWireframe;

    public:
        DebugDrawer(SDL_Renderer* renderer
                    //, Vector2Df tile_size
                    );
        void update_camera(Camera2D const& camera);
        void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

        void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime,
                              const btVector3& color) override;

        void reportErrorWarning(const char* warningString) override;

        void draw3dText(const btVector3& location, const char* textString) override;

        void setDebugMode(int debugMode) override;

        int getDebugMode() const override;
    };

}
