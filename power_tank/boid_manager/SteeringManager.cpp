#include "SteeringManager.h"
#include <random>

void SteeringManager::Seek(IBoid::Ptr me, const glm::vec2& target)
{
    float slowingRadius = 100.0f;

    glm::vec2 desired_velocity = target - me->location;
    float distance = glm::length(desired_velocity);

    desired_velocity = glm::normalize(desired_velocity) * me->maxSpeed;
    if (distance <= slowingRadius)
    {
        desired_velocity *= (distance / slowingRadius);
    }

    glm::vec2 force = desired_velocity - me->velocity;
    me->ApplyForce(force);
}

void SteeringManager::Flee(IBoid::Ptr me, const glm::vec2& target)
{
    glm::vec2 desired_velocity = target - me->location;
    desired_velocity = glm::normalize(desired_velocity) * me->maxSpeed;
    desired_velocity = -desired_velocity;

    glm::vec2 force = desired_velocity - me->velocity;
    me->ApplyForce(force);
}

void SteeringManager::Wander(IBoid::Ptr me)
{
    static float wanderAngle = 0.0f;
    float circleDistance = 5.0f;
    float angleChange = 15.0f;

    glm::vec2 circleCenter = glm::normalize(me->velocity);
    circleCenter *= circleDistance;

    glm::vec2 displacement = glm::vec2(0, -1);
    displacement *= circleDistance;

    std::random_device rd;
    std::default_random_engine rng(rd());
    float randomNumber = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);

    wanderAngle += (randomNumber * angleChange) - (angleChange * 0.5f);

    auto length = glm::length(displacement);
    displacement.x = std::cos(wanderAngle) * length;
    displacement.y = std::sin(wanderAngle) * length;

    glm::vec2 force = circleCenter + displacement;
    me->ApplyForce(force);
}

void SteeringManager::Pursuit(IBoid::Ptr me, const IBoid::Ptr target)
{
    auto distance = target->location - me->location;
    float updatesNeeded = glm::length(distance) / me->maxSpeed;

    glm::vec2 targetFuturePos = target->location + (target->velocity * updatesNeeded);

    glm::vec2 desired_velocity = targetFuturePos - me->location;
    desired_velocity = glm::normalize(desired_velocity) * me->maxSpeed;
    glm::vec2 force = desired_velocity - me->velocity;

    me->ApplyForce(force);
}

void SteeringManager::Evade(IBoid::Ptr me, const IBoid::Ptr target)
{
    auto distance = target->location - me->location;
    float updatesNeeded = glm::length(distance) / me->maxSpeed;

    glm::vec2 targetFuturePos = target->location + (target->velocity * updatesNeeded);
    Flee(me, targetFuturePos);
}
