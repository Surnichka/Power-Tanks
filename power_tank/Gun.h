#pragma once

#include "Ball.h"

class Gun
{
public:
    void Init(const glm::vec2& playerPos, const sf::Vector2i& mousePos);
    void Shoot(const sf::Vector2i& mousePos);
    void Update();
    void Draw(sf::RenderWindow &window);

private:
    std::vector<Ball> bullets;
};
