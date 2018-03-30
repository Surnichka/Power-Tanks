#pragma once
#include "Ball.h"

class Gun
{
public:
    void Init(int frameRate, int damage);
    void Shoot(const glm::vec2& srcPos, const glm::vec2& destPos);
    void Update();
    void Draw(sf::RenderWindow& window);
    std::vector<Ball>& GetBullets();
private:
    std::vector<Ball> bullets;
    static constexpr float bullet_radius = 4.0f;
    static constexpr float bullet_speed = 10.0f;
    static constexpr float bullet_mass = 4.0f;
};
