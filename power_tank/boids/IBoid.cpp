#include "IBoid.h"

////////////////////////////////////////////////////////////////////
void IBoid::ApplyForce(glm::vec2 force)
{
    acceleration += force;
}

////////////////////////////////////////////////////////////////////
void IBoid::Update()
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
