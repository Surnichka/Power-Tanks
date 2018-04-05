#pragma once
#include "Ball.h"

class Gun
{
public:
    void Init(float frameRate, float bulletSpeed, int damage);
    void Shoot(const glm::vec2& srcPos, const glm::vec2& destPos);
    void Ultimate(const glm::vec2& srcPos);
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
    std::vector<Ball>& GetBullets();
private:
    int bullet_damage = 1;
    float last_shoot = 0.0f;
    float bullet_speed = 10.5f;
    float bullet_frame_rate = 200.0f;

    float ultimate_cooldown = 10000.0f;
    float last_ultimate = ultimate_cooldown;

    std::vector<Ball> bullets;
    static constexpr float bullet_radius = 4.0f;
    static constexpr float bullet_mass = 4.0f;
};
