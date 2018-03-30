#pragma once
#include <glm/glm.hpp>
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include <functional>

class Ball
{
public:
    using OnCollisionFunc = std::function<void(Ball& self, Ball& other)>;
    using OnWallHitFunc = std::function<void(Ball& self)>;

    Ball() = default;
    Ball(glm::vec2 pos, glm::vec2 vel, float r, float m);

    void Init(glm::vec2 pos, glm::vec2 vel, float r, float m);

    void Update(float dt);
    void Draw(sf::RenderWindow& window);

    bool IsCollide(const Ball& other) const;
    void ResolveCollision(Ball& other);

    bool IsAlive() const;
    void Destroy();
    void TakeLife(int damage);

    void OnCollideOtherBall(OnCollisionFunc onColFunc);
    void OnWallCollide(OnWallHitFunc onWallHitFunc);

    void SetMaxHealth(int health);
public:
    OnWallHitFunc m_onWallHit;
    OnCollisionFunc m_onCollision;
    int m_maxHealth = 1;
    int m_currentHealth = 1;
    bool m_enteringScreen = true;
    size_t m_id = 0;
    glm::vec2 m_position;
    glm::vec2 m_velocity;
    float m_radius;
    float m_mass;

    sf::CircleShape m_circle;
};
