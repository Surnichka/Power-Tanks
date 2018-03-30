#pragma once
#include "Ball.h"

class Gun
{
public:
    void Init(float frameRate, int damage);
    void Shoot(const glm::vec2& srcPos, const glm::vec2& destPos);
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
    std::vector<Ball>& GetBullets();
private:
    float bullet_frame_rate = 500.0f;
    float last_shoot = 0.0f;

    std::vector<Ball> bullets;
    int bullet_damage = 1;
    static constexpr float bullet_radius = 4.0f;
    static constexpr float bullet_speed = 10.0f;
    static constexpr float bullet_mass = 4.0f;
};
