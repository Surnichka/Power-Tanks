#pragma once
#include "glm/glm.hpp"
#include "SFML/Graphics/Rect.hpp"
#include <functional>
#include "IBoid.h"

class CollisionManager
{
public:
    enum class WallHitInfo { FromInside, FromOutside, NoCollision };

    struct Preset
    {
        static void BounceFromWall(IBoid::Ptr me, CollisionManager::WallHitInfo wallHitInfo);
        static void MoveThroughWall(IBoid::Ptr me, CollisionManager::WallHitInfo wallHitInfo);
        static void ResolveBallCollision(IBoid::Ptr me, IBoid::Ptr other);
    };

    using ResolveWallHitFunc = std::function<void(IBoid::Ptr, WallHitInfo)>;
    using ResolveCollisionFunc = std::function<void(IBoid::Ptr, IBoid::Ptr)>;

    using OnWallHitCB = std::function<void(IBoid::Ptr)>;
    using OnCollisionCB = std::function<void(IBoid::Ptr, IBoid::Ptr)>;

    void Process(IBoid::Ptr me, IBoid::Ptr other);

    void SetResolveWallHitFunc(ResolveWallHitFunc resolveWallHit);
    void SetResolveCollisionFunc(ResolveCollisionFunc resolveCollision);

    void SetOnWallHitCB(OnWallHitCB onWallHitCB);
    void SetOnCollisionCB(OnCollisionCB onCollisionCB);
private:
    WallHitInfo IsWallHit(const IBoid::Ptr me) const;
    bool IsCollding(const IBoid::Ptr me, const IBoid::Ptr other) const;

    OnWallHitCB m_onWallHitCB;
    OnCollisionCB m_onCollisionCB;

    ResolveWallHitFunc m_resolveWallHit = Preset::BounceFromWall;
    ResolveCollisionFunc m_resolveCollision = Preset::ResolveBallCollision;
};


