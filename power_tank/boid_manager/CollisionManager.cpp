#include "CollisionManager.h"
#include "../Window.h"

namespace
{
    static const sf::FloatRect collisionWindow =
        {200, 200, Window::Playable::width-200, Window::Playable::height-200};
}

////////////////////////////////////////////////////////////////////////////////
CollisionManager::WallHitInfo CollisionManager::IsWallHit(const IBoid::Ptr me) const
{
    glm::vec2 ahead = me->location + me->velocity + me->radius;
    WallHitInfo wallHitInfo = collisionWindow.contains(ahead.x, ahead.y) ?
                WallHitInfo::FromOutside : WallHitInfo::FromInside;

    if( (me->location.x + me->radius < collisionWindow.left)   ||
        (me->location.x + me->radius > collisionWindow.width)  ||
        (me->location.y + me->radius < collisionWindow.top)    ||
        (me->location.y + me->radius > collisionWindow.height) )
    {
        return wallHitInfo;
    }
    return WallHitInfo::NoCollision;
}

////////////////////////////////////////////////////////////////////////////////
bool CollisionManager::IsCollding(const IBoid::Ptr me, const IBoid::Ptr other) const
{
    float distance = glm::distance(me->location, other->location);
    float sumRadius = me->radius + other->radius;
    return distance <= sumRadius;
}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::Process(IBoid::Ptr me, IBoid::Ptr other)
{
    auto wallHitInfo = IsWallHit(me);
    if( WallHitInfo::NoCollision != wallHitInfo )
    {
        if(nullptr != m_onWallHitCB)
        {
            m_onWallHitCB(me);
        }

        if(nullptr != m_resolveWallHit)
        {
            m_resolveWallHit(me, wallHitInfo);
        }
    }

    if( IsCollding(me, other) )
    {
        if( nullptr != m_onCollisionCB )
        {
            m_onCollisionCB(me, other);
        }
        if( nullptr != m_resolveCollision )
        {
            m_resolveCollision(me, other);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::SetResolveWallHitFunc(ResolveWallHitFunc resolveWallHit)
{
    m_resolveWallHit = resolveWallHit;
}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::SetResolveCollisionFunc(ResolveCollisionFunc resolveCollision)
{
    m_resolveCollision = resolveCollision;
}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::SetOnWallHitCB(CollisionManager::OnWallHitCB onWallHitCB)
{
    m_onWallHitCB = onWallHitCB;
}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::SetOnCollisionCB(CollisionManager::OnCollisionCB onCollisionCB)
{
    m_onCollisionCB = onCollisionCB;
}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::Preset::BounceFromWall(IBoid::Ptr me, CollisionManager::WallHitInfo wallHitInfo)
{
    if( WallHitInfo::FromInside == wallHitInfo )
    {
        if(me->location.x + me->radius < collisionWindow.left) { me->velocity.x *= (-1); }
        if(me->location.y + me->radius < collisionWindow.top) { me->velocity.y *= (-1); }
        if(me->location.x + me->radius > collisionWindow.width)  { me->velocity.x *= (-1); }
        if(me->location.y + me->radius > collisionWindow.height) { me->velocity.y *= (-1); }
    }
}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::Preset::MoveThroughWall(IBoid::Ptr me, CollisionManager::WallHitInfo wallHitInfo)
{
    if( WallHitInfo::FromInside == wallHitInfo )
    {
        if (me->location.x < -me->radius) me->location.x = Window::width + me->radius;
        if (me->location.y < -me->radius) me->location.y = Window::height + me->radius;
        if (me->location.x > Window::width + me->radius) me->location.x = -me->radius;
        if (me->location.y > Window::height + me->radius) me->location.y = -me->radius;
    }
}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::Preset::ResolveBallCollision(IBoid::Ptr me, IBoid::Ptr other)
{
    float minimal = me->radius + other->radius;
    me->location -= me->velocity;
    other->location -= other->velocity;

    glm::vec2 line = other->location - me->location;
    glm::vec2 un = glm::normalize(line);
    glm::vec2 ut = glm::vec2(-un.y, un.x);

    float distance = glm::dot(line, un);
    float v1n = glm::dot(me->velocity, un);
    float v2n = glm::dot(other->velocity, un);
    float v1t = glm::dot(ut, me->velocity);
    float v2t = glm::dot(ut, other->velocity);

    if(v1n == v2n)
    {
        return;
    }

    float t = (minimal - distance) / (v2n - v1n);
    me->location += t * me->velocity;
    other->location += t * other->velocity;

    float newV1n = ((v1n*(me->mass-other->mass))+(2*other->mass*v2n))/(me->mass + other->mass);
    float newV2n = ((v2n*(other->mass-me->mass))+(2*me->mass*v1n))/(me->mass + other->mass);

    me->velocity = (newV1n*un) + (v1t*ut);
    other->velocity = (newV2n*un) + (v2t*ut);

    me->location += (1.0f - t) * me->velocity;
    other->location += (1.0f -t) * other->velocity;
}
