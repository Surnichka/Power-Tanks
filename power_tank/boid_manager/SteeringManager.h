#pragma once
#include "glm/glm.hpp"
#include "IBoid.h"
#include <vector>

class SteeringManager
{
public:
    static void Seek(IBoid::Ptr me, const glm::vec2& target);
    static void Flee(IBoid::Ptr me, const glm::vec2& target);
    static void Wander(IBoid::Ptr me);
    static void Pursuit(IBoid::Ptr me, const IBoid::Ptr target);
    static void Evade(IBoid::Ptr me, const IBoid::Ptr target);
private:
};
