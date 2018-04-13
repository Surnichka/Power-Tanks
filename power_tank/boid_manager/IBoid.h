#pragma once
#include "glm/glm.hpp"
#include <memory>

class IBoid
{
public:
    using Ptr = std::shared_ptr<IBoid>;

    glm::vec2 location;
    glm::vec2 velocity;
    glm::vec2 acceleration;

    float radius = 10.0f;
    float maxSpeed = 10.0f;
    float maxForce = 5.0f;
    float mass = 3.0f;

    void ApplyForce(glm::vec2 force)
    {
        acceleration += force;
    }

    void Update()
    {
        float accTruncate = maxForce / glm::length(acceleration);
        accTruncate = glm::min(accTruncate, 1.0f);
        acceleration *= accTruncate;
        acceleration *= 1.0f / mass;

        velocity += acceleration;

        float velTruncate = maxForce / glm::length(velocity);
        velTruncate = glm::min(velTruncate, 1.0f);
        velocity *= velTruncate;

        location += velocity;

        acceleration = glm::vec2(0, 0);
    }
};
