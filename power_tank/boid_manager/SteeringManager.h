#pragma once
#include "glm/glm.hpp"
#include "../boids/IBoid.h"
#include <vector>

class SteeringManager
{
public:
    static void Seek(IBoid::WeakPtr weakMe, const glm::vec2& target);
    static void Flee(IBoid::WeakPtr weakMe, const glm::vec2& target);
    static void Wander(IBoid::WeakPtr weakMe);
    static void Pursuit(IBoid::WeakPtr weakMe, const IBoid::WeakPtr weakTarget);
    static void Evade(IBoid::WeakPtr weakMe, const IBoid::WeakPtr weakTarget);
private:
};
