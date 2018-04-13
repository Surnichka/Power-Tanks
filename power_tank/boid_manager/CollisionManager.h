#pragma once
#include "glm/glm.hpp"
#include "SFML/Graphics/Rect.hpp"
#include <functional>
#include "../boids/IBoid.h"

class CollisionManager
{
public:
    enum class WallHitInfo { FromInside, FromOutside, NoCollision };

    struct Preset
    {
        static void BounceFromWall(IBoid::WeakPtr weakMe, CollisionManager::WallHitInfo wallHitInfo);
        static void MoveThroughWall(IBoid::WeakPtr weakMe, CollisionManager::WallHitInfo wallHitInfo);
        static void ResolveBallCollision(IBoid::WeakPtr weakMe, IBoid::WeakPtr weakOther);
    };

    using ResolveWallHitFunc = std::function<void(IBoid::WeakPtr, WallHitInfo)>;
    using ResolveCollisionFunc = std::function<void(IBoid::WeakPtr, IBoid::WeakPtr)>;

    using OnWallHitCB = std::function<void(IBoid::WeakPtr)>;
    using OnCollisionCB = std::function<void(IBoid::WeakPtr, IBoid::WeakPtr)>;

    void Process(IBoid::WeakPtr weakMe, IBoid::WeakPtr weakOther);

    void SetResolveWallHitFunc(ResolveWallHitFunc resolveWallHit);
    void SetResolveCollisionFunc(ResolveCollisionFunc resolveCollision);

    void SetOnWallHitCB(OnWallHitCB onWallHitCB);
    void SetOnCollisionCB(OnCollisionCB onCollisionCB);
private:
    WallHitInfo IsWallHit(const IBoid::WeakPtr me) const;
    bool IsCollding(const IBoid::WeakPtr weakMe, const IBoid::WeakPtr weakOther) const;

    OnWallHitCB m_onWallHitCB;
    OnCollisionCB m_onCollisionCB;

    ResolveWallHitFunc m_resolveWallHit = Preset::BounceFromWall;
    ResolveCollisionFunc m_resolveCollision = Preset::ResolveBallCollision;
};


