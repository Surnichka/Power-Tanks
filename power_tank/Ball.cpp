#include "Ball.h"
#include "Window.h"
#include <iostream>
#include <random>

namespace
{
size_t getFreeId()
{
    static size_t id = 0;
    return id++;
}
}

Ball::Ball(glm::vec2 pos, glm::vec2 vel, float r, float m)
{
    Init(pos, vel, r, m);
}

void Ball::Init(glm::vec2 pos, glm::vec2 vel, float r, float m)
{
    m_id = getFreeId();

    m_position = pos;
    m_velocity = vel;
    m_radius = r;
    m_mass = m;

    m_circle.setPosition(pos.x, pos.y);
    m_circle.setRadius(r);
    m_circle.setOrigin(r, r);
    m_circle.setFillColor(sf::Color::White);
    m_circle.setOutlineThickness(1);
    m_circle.setOutlineColor(sf::Color::Black);
}

void Ball::Destroy()
{
    m_currentHealth = 0;
}

void Ball::TakeLife(int damage)
{
    m_currentHealth = std::max(0, m_currentHealth - damage);
}

void Ball::Update(float dt)
{
    m_position += m_velocity;
    if( m_enteringScreen )
    {
        static const sf::FloatRect windowRect = {0, 0, Window::Playable::width,
                                                       Window::Playable::height};
        if(windowRect.contains(m_position.x, m_position.y))
        {
            m_enteringScreen = false;
        }
    }
    else
    {
        float w = Window::Playable::width - m_radius;
        float h = Window::Playable::height - m_radius;

        bool wallHitted = false;
        if( m_position.x <= 0 || m_position.x >= w)
        {
            m_velocity.x *= (-1);
            wallHitted = true;
        }
        if( m_position.y <= 0 || m_position.y >= h)
        {
            m_velocity.y *= (-1);
            wallHitted = true;
        }

        if( wallHitted )
        {
            if(nullptr != m_onWallHit)
            {
                m_onWallHit(*this);
            }
        }
        m_position.x = glm::clamp<float>(m_position.x, 0, w);
        m_position.y = glm::clamp<float>(m_position.y, 0, h);
    }

    //Update the circle for drawing
    m_circle.setPosition(m_position.x, m_position.y);
}

void Ball::Draw(sf::RenderWindow &window)
{
    window.draw(m_circle);
}

bool Ball::IsCollide(const Ball &other) const
{
    auto distance = glm::distance(m_position, other.m_position);
    return distance <= (m_radius + other.m_radius);
}

void Ball::ResolveCollision(Ball &other)
{
    if( nullptr != m_onCollision )
    {
        m_onCollision(*this, other);
    }

    float minimal = m_radius + other.m_radius;
    m_position -= m_velocity;
    other.m_position -= other.m_velocity;
    glm::vec2 line = other.m_position - m_position;
    glm::vec2 un = glm::normalize(line);
    glm::vec2 ut = glm::vec2(-un.y, un.x);
    float distance = glm::dot(line, un);
    float v1n = glm::dot(m_velocity, un);
    float v2n = glm::dot(other.m_velocity, un);
    float v1t = glm::dot(ut, m_velocity);
    float v2t = glm::dot(ut, other.m_velocity);

    if(v1n == v2n)
    {
        return;
    }

    float t = (minimal - distance) / (v2n - v1n);
    m_position += t * m_velocity;
    other.m_position += t * other.m_velocity;
    float newV1n = ((v1n*(m_mass-other.m_mass))+(2*other.m_mass*v2n))/(m_mass + other.m_mass);
    float newV2n = ((v2n*(other.m_mass-m_mass))+(2*m_mass*v1n))/(m_mass + other.m_mass);
    m_velocity = (newV1n*un) + (v1t*ut);
    other.m_velocity = (newV2n*un) + (v2t*ut);

    m_position += (1.0f - t) * m_velocity;
    other.m_position += (1.0f -t) * other.m_velocity;
}

bool Ball::IsAlive() const
{
    return m_currentHealth != 0;
}

void Ball::OnWallCollide(Ball::OnWallHitFunc onWallHitFunc)
{
    m_onWallHit = onWallHitFunc;
}

void Ball::SetMaxHealth(int health)
{
    m_maxHealth = health;
    m_currentHealth = health;
}

void Ball::OnCollideOtherBall(Ball::OnCollisionFunc onColFunc)
{
    m_onCollision = onColFunc;
}
