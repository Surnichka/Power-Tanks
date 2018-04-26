#pragma once
#include "Ball.h"
#include "SFML/Graphics/RectangleShape.hpp"

class Gun
{
public:
    void Init();
    void Shoot();
    void Ultimate();
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
    void setBarrelPositions(glm::vec2 pos);
    std::vector<Ball>& GetBullets();
private:
    void connectSignals();
    void Shoot(glm::vec2 destPos);
    void LookAtMousePos();

    float bullet_damage = 1;
    float bullet_radius = 4.0f;

    float bullet_speed = 7.0f;
    float bullet_fire_rate = 600.0f;
    float critical_chance = 10.0f;
    float crit_damage = 2.1f;

    float last_shoot = 0.0f;
    float ultimate_cooldown = 30000.0f;
//    float ultimate_cooldown = 1000.0f;
    float last_ultimate = ultimate_cooldown;

    sf::Vector2i mousePos;
    std::vector<Ball> bullets;
    sf::RectangleShape barrel;
    static constexpr float bullet_mass = 4.0f;
};
