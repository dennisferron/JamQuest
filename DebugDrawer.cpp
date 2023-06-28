#include "DebugDrawer.hpp"

using namespace jq;

DebugDrawer::DebugDrawer(SDL_Renderer* renderer
                         //, Vector2Df tile_size
                         )
    : renderer(renderer)
        //, tile_size(tile_size)
        , camera(1,1)
{
}

void DebugDrawer::update_camera(const Camera2D& camera)
{
    this->camera = camera;
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    SDL_SetRenderDrawColor(renderer,
                           (Uint8)color.x(),
                           (Uint8)color.y(),
                           (Uint8)color.z(),
                           (Uint8)color.w());

    Vector2Df w0 = { to.x(), to.y() };
    Vector2Df w1 = { from.x(), from.y() };

    Vector2Df v0 = camera.world_to_viewport(/* tile_size * */ w0);
    Vector2Df v1 = camera.world_to_viewport(/* tile_size * */ w1);

    SDL_RenderDrawLineF(renderer,
                        (float)v0.x,
                        (float)v0.y,
                        (float)v1.x,
                        (float)v1.y);
}

void
DebugDrawer::drawContactPoint(const btVector3& PointOnB,
                              const btVector3& normalOnB,
                              btScalar distance,
                              int lifeTime,
                              const btVector3& color)
{

}

void DebugDrawer::reportErrorWarning(const char* warningString)
{

}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{

}

void DebugDrawer::setDebugMode(int debugMode)
{
    this->debug_mode = debugMode;
}

int DebugDrawer::getDebugMode() const
{
    return debug_mode;
}
