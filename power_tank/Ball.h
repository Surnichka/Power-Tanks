#pragma once
#include <glm/glm.hpp>
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/CircleShape.hpp"

class Ball
{
public:
    Ball() = default;
    Ball(glm::vec2 pos, glm::vec2 vel, float r, float m);
    void Init(glm::vec2 pos, glm::vec2 vel, float r, float m);
    void Update();
    void Draw(sf::RenderWindow& window);
    bool IsCollide(const Ball& ball);
    void ResolveCollision(Ball& ball);

public:
    bool enteringScreen = true;
    size_t id = 0;
    glm::vec2 position;
    glm::vec2 velocity;
    float radius;
    float mass;

    sf::CircleShape circle;
};
