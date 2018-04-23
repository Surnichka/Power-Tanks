#pragma once
#include "Ball.h"
#include "SFML/Graphics/RectangleShape.hpp"

class Gun
{
public:
    void Init(/*float frameRate, float bulletSpeed, int damage*/);
    void Shoot();
    void Ultimate();
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
    void setBarrelPositions(glm::vec2 pos);
    std::vector<Ball>& GetBullets();
private:
    void Shoot(glm::vec2 destPos);
    void DebugMenu();
    void LookAtMousePos();

    int bullet_damage = 1;
    float bullet_radius = 4.0f;

    float last_shoot = 0.0f;
    float bullet_speed = 7.0f;
    float bullet_frame_rate = 700.0f;

    float ultimate_cooldown = 100.0f;
    float last_ultimate = ultimate_cooldown;

    sf::Vector2i mousePos;
    std::vector<Ball> bullets;
    sf::RectangleShape barrel;
    static constexpr float bullet_mass = 4.0f;
};
