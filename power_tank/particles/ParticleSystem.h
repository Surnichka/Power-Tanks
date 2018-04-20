#pragma once
#include <array>
#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/CircleShape.hpp"

class ParticleSystem
{
public:
    void SetFading(bool fading);
    void SetDuration(float duration);
    void SetSpeed(float maxSpeed);
    void SetFuel(uint32_t fuel);
    void SetParticleRadius(float radius);

    void Emit(sf::Vector2i pos);
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
private:
    void CreateParticle(sf::Vector2i pos, uint32_t idxPos);
    void Refresh();
    bool m_dirty = true;

    bool m_fading = false;

    float m_particleRadius = 1.0f;
    float m_duration = 400.0f;
    float m_maxSpeed = 0.1f;

    uint32_t m_fuel = 200;

    static const uint32_t maxParticles = 4000;
    sf::CircleShape sfCircle;

    struct Particle
    {
        float elapsed = 0.0f;
        bool alive = false;
        sf::Vector2f velocity;
        sf::Vector2f position;
        sf::Color color;
    };
    std::array<Particle, maxParticles> m_particles;
};
