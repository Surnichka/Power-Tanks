#include "Boid.h"
#include <random>
#include <cmath>
#include "../Window.h"

namespace
{
    void setAngle(glm::vec2& vec, float angle)
    {
        auto length = glm::length(vec);
        vec.x = std::cos(angle) * length;
        vec.y = std::sin(angle) * length;
    }
    glm::vec2 truncate(glm::vec2 vec, float limit)
    {
        float delta = limit / glm::length(vec);
        delta = std::min(delta, 1.0f);
        return (vec * delta);
    }
}

///////////////////////////////////////////////////////////////////////////////////////
Boid::Boid(glm::vec2 pos)
    : m_position(pos) {}

void Boid::seek(glm::vec2 target, float slowingRadius)
{
    m_steering += doSeek(target, slowingRadius);
}

void Boid::flee(glm::vec2 target)
{
    m_steering += doFlee(target);
}

void Boid::wander()
{
    m_steering += doWander();
}

void Boid::evade(glm::vec2 target_pos, glm::vec2 target_velocity)
{
    m_steering += doEvade(target_pos, target_velocity);
}

void Boid::pursuit(glm::vec2 target_pos, glm::vec2 target_velocity)
{
    m_steering += doPursuit(target_pos, target_velocity);
}

void Boid::update()
{
    m_steering = truncate(m_steering, max_force);
    m_steering *= 1.0f / mass;

    m_velocity += m_steering;
    m_velocity = truncate(m_velocity, max_speed);

    m_position += m_velocity;

    if( m_position.x < 0 ) { m_position.x = Window::width; }
    if( m_position.y < 0 ) { m_position.y = Window::height; }
    if( m_position.x > Window::width ) { m_position.x = 0; }
    if( m_position.y > Window::height ) { m_position.y = 0; }
}

void Boid::reset()
{

}

///////////////////////////////////////////////////////////////////////////////////////
glm::vec2 Boid::doSeek(glm::vec2 target, float slowingRadius)
{
    glm::vec2 desired_velocity = target - m_position;
    float distance = glm::length(desired_velocity);

    desired_velocity = glm::normalize(desired_velocity) * max_speed;
    if (distance <= slowingRadius)
    {
        desired_velocity *= (distance / slowingRadius);
    }

    glm::vec2 force = desired_velocity - m_velocity;
    return force;
}

glm::vec2 Boid::doFlee(glm::vec2 target)
{
    glm::vec2 desired_velocity = target - m_position;
    desired_velocity = glm::normalize(desired_velocity) * max_speed;
    desired_velocity = -desired_velocity;
    glm::vec2 force = desired_velocity - m_velocity;
    return force;
}

glm::vec2 Boid::doWander()
{
    glm::vec2 circleCenter = glm::normalize(m_velocity);
    circleCenter *= wander_circle_distance;

    glm::vec2 displacement = glm::vec2(0, -1);
    displacement *= wander_circle_distance;

    std::random_device rd;
    std::default_random_engine rng(rd());
    float random = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);

    static float wanderAngle = 0;
    wanderAngle += (random * wander_angle_change) - (wander_angle_change * 0.5f);
    setAngle(displacement, wanderAngle);

    glm::vec2 wanderForce = circleCenter + displacement;
    return wanderForce;
}

glm::vec2 Boid::doEvade(glm::vec2 target_pos, glm::vec2 target_velocity)
{
    auto distance = target_pos - m_position;
    float updatesNeeded = glm::length(distance) / max_speed;

    glm::vec2 target_future_position = target_pos + (target_velocity * updatesNeeded);
    return doFlee(target_future_position);
}

glm::vec2 Boid::doPursuit(glm::vec2 target_pos, glm::vec2 target_velocity)
{
    auto distance = target_pos - m_position;
    float updatesNeeded = glm::length(distance) / max_speed;

    glm::vec2 target_future_position = target_pos + (target_velocity * updatesNeeded);
    return doSeek(target_future_position);
}
