#include "ParticleSystem.h"
#include <random>
#include <cmath>
#include "../utils/Utils.hpp"

namespace
{
    static std::random_device rd;
    static std::default_random_engine rng(rd());
    using Random = std::uniform_real_distribution<float>;
}

void ParticleSystem::SetFading(bool fading)     { m_fading = fading;     m_dirty = true; }
void ParticleSystem::SetDuration(float duration){ m_duration = duration; m_dirty = true; }
void ParticleSystem::SetSpeed(float maxSpeed)   { m_maxSpeed = maxSpeed; m_dirty = true; }
void ParticleSystem::SetFuel(uint32_t fuel)     { m_fuel = fuel;         m_dirty = true; }
void ParticleSystem::SetParticleRadius(float radius)
{
    m_particleRadius = radius;
    m_dirty = true;
}

void ParticleSystem::Emit(sf::Vector2i pos)
{
    Refresh();

//    bool m_fading = true;
    uint32_t fuel = m_fuel;
    for(uint32_t i = 0; i < maxParticles; i++)
    {
        if(false == m_particles[i].alive)
        {
            CreateParticle(pos, i);
            fuel--;
        }
        if( fuel == 0 )
        {
            break;
        }
    }
}

void ParticleSystem::Update(float dt)
{
    for(uint32_t i = 0; i < maxParticles; i++)
    {
        if(false == m_particles[i].alive)
        {
            continue;
        }

        m_particles[i].elapsed += dt;
        m_particles[i].position += m_particles[i].velocity;

        if(m_particles[i].elapsed > m_duration)
        {
            m_particles[i].alive = false;
        }

//        float x2 = m_particles[i].initialPos.x;
//        float y2 = m_particles[i].initialPos.y;
//        float x1 = m_particles[i].position.x;
//        float y1 = m_particles[i].position.y;

//        float distanceX = (x2 - x1) * (x2 - x1);
//        float distanceY = (y2 - y1) * (y2 - y1);
//        float distance = sqrtf(distanceX - distanceY);
//        if(distance > m_area)
//        {
//            m_particles[i].alive = false;
//        }
    }
}

void ParticleSystem::Draw(sf::RenderWindow& window)
{
    for(uint32_t i = 0; i < maxParticles; i++)
    {
        if( false == m_particles[i].alive )
        {
            continue;
        }

        if( m_fading && m_particles[i].elapsed > (m_duration / 2))
        {
            uint8_t alpha = uint8_t(utils::map<float>
                            (m_particles[i].elapsed,
                             m_duration / 2, m_duration,
                             0, 255));
            m_particles[i].color.a = alpha;
        }
        else
        {
            m_particles[i].color.a = 255;
        }

        sfCircle.setFillColor(m_particles[i].color);
        sfCircle.setPosition(m_particles[i].position);
        window.draw(sfCircle);
    }
}

void ParticleSystem::CreateParticle(sf::Vector2i pos, uint32_t idxPos)
{
    m_particles[idxPos].position.x = pos.x;
    m_particles[idxPos].position.y = pos.y;

    auto imba_random = [this]()
    {
        if( Random(-m_maxSpeed, m_maxSpeed)(rng) < Random(-m_maxSpeed, m_maxSpeed)(rng) )
        {
            return Random(-m_maxSpeed/Random(-m_maxSpeed, m_maxSpeed)(rng),
                           m_maxSpeed/Random(-m_maxSpeed, m_maxSpeed)(rng))(rng);
        }
        else
        {
            return (Random(-m_maxSpeed, m_maxSpeed)(rng) + Random(-m_maxSpeed, m_maxSpeed)(rng)) / 2;
        }
    };

    auto some_random = [this, idxPos]()
    {
        return Random(-m_maxSpeed/Random(-m_maxSpeed, m_maxSpeed)(rng), m_maxSpeed/-m_maxSpeed)(rng);
    };

    m_particles[idxPos].velocity.x = imba_random();
    m_particles[idxPos].velocity.y = some_random();


    m_particles[idxPos].alive = true;
    m_particles[idxPos].elapsed = 0.0f;

    auto r = Random(0, 1)(rng);
    if( r < 0.10f)
    {
        m_particles[idxPos].color = sf::Color::Red;
    }
    else
    {
        m_particles[idxPos].color = sf::Color::Black;
    }
}

void ParticleSystem::Refresh()
{
    if(false == m_dirty)
    {
        return;
    }

    sfCircle.setRadius(m_particleRadius);
    sfCircle.setOrigin(m_particleRadius, m_particleRadius);

    m_dirty = false;
}
