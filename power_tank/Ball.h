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
    void TakeLife(float damage);

    void OnCollideOtherBall(OnCollisionFunc onColFunc);
    void OnWallCollide(OnWallHitFunc onWallHitFunc);

    void SetMaxHealth(float health);
    void AddLife(float health);

    float GetCurrentHealth();
    glm::vec2 getCurrentPosition();
public:
    size_t m_id = 0;
    glm::vec2 m_position;
    glm::vec2 m_velocity;
    OnWallHitFunc m_onWallHit;
    OnCollisionFunc m_onCollision;

    float m_mass;
    float m_radius;
    float m_maxHealth = 1.0f;
    float m_currentHealth = 1.0f;
    bool m_enteringScreen = true;

    sf::CircleShape m_circle;
};
