#include "CollisionManager.h"
#include "../Window.h"

namespace
{
    static const sf::FloatRect collisionWindow = {0, 0, Window::width, Window::height};
}

////////////////////////////////////////////////////////////////////////////////
CollisionManager::WallHitInfo CollisionManager::IsWallHit(const IBoid::WeakPtr weakMe) const
{
    auto me = weakMe.lock();
    if( nullptr == me )
    {
        return WallHitInfo::NoCollision;
    }

    glm::vec2 ahead = me->location + me->velocity;
    WallHitInfo wallHitInfo = collisionWindow.contains(ahead.x, ahead.y) ?
                WallHitInfo::FromOutside : WallHitInfo::FromInside;

    if( (me->location.x < collisionWindow.left)   ||
        (me->location.x > collisionWindow.width)  ||
        (me->location.y < collisionWindow.top)    ||
        (me->location.y > collisionWindow.height) )
    {
        return wallHitInfo;
    }
    return WallHitInfo::NoCollision;
}

////////////////////////////////////////////////////////////////////////////////
bool CollisionManager::IsCollding(const IBoid::WeakPtr weakMe, const IBoid::WeakPtr weakOther) const
{
    auto me = weakMe.lock();
    auto other = weakOther.lock();
    if( nullptr == me || nullptr == other)
    {
        return false;
    }

    float distance = glm::distance(me->location, other->location);
    float sumRadius = me->radius + other->radius;
    return distance <= sumRadius;
}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::Process(IBoid::WeakPtr weakMe, IBoid::WeakPtr weakOther)
{
    auto me = weakMe.lock();
    auto other = weakOther.lock();
    if( nullptr == me || nullptr == other)
    {
        return;
    }

    if( me == other )
    {
        return;
    }

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
void CollisionManager::Preset::BounceFromWall(IBoid::WeakPtr weakMe, CollisionManager::WallHitInfo wallHitInfo)
{
    auto me = weakMe.lock();
    if( nullptr == me )
    {
        return;
    }

    if( WallHitInfo::FromInside == wallHitInfo )
    {
        if(me->location.x< collisionWindow.left) { me->velocity.x *= (-1); }
        if(me->location.y< collisionWindow.top) { me->velocity.y *= (-1); }
        if(me->location.x> collisionWindow.width)  { me->velocity.x *= (-1); }
        if(me->location.y> collisionWindow.height) { me->velocity.y *= (-1); }
    }
}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::Preset::MoveThroughWall(IBoid::WeakPtr weakMe, CollisionManager::WallHitInfo wallHitInfo)
{
    auto me = weakMe.lock();
    if( nullptr == me )
    {
        return;
    }

    if( WallHitInfo::FromInside == wallHitInfo )
    {
        if (me->location.x < 0) me->location.x = Window::width;
        if (me->location.y < 0) me->location.y = Window::height;
        if (me->location.x > Window::width) me->location.x = 0;
        if (me->location.y > Window::height) me->location.y = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::Preset::ResolveBallCollision(IBoid::WeakPtr weakMe, IBoid::WeakPtr weakOther)
{
    auto me = weakMe.lock();
    auto other = weakOther.lock();
    if( nullptr == me || nullptr == other)
    {
        return;
    }

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
