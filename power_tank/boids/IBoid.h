#pragma once
#include "glm/glm.hpp"
#include <memory>
#include "SFML/Graphics/RenderWindow.hpp"

class IBoid
{
public:
    IBoid() = default;
    virtual ~IBoid() = default;

    using Ptr = std::shared_ptr<IBoid>;
    using WeakPtr = std::weak_ptr<IBoid>;

    void ApplyForce(glm::vec2 force);
    virtual void Update();
    virtual void Draw(sf::RenderWindow&) = 0;
public:
    glm::vec2 location;
    glm::vec2 velocity;
    glm::vec2 acceleration;

    float radius = 10.0f;
    float maxSpeed = 10.0f;
    float maxForce = 5.0f;
    float mass = 3.0f;   
};
