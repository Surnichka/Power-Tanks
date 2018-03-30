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
    id = getFreeId();

    position = pos;
    velocity = vel;
    radius = r;
    mass = m;

    circle.setPosition(pos.x, pos.y);
    circle.setRadius(r);
    circle.setOrigin(r, r);
    circle.setFillColor(sf::Color::White);
    circle.setOutlineThickness(1);
    circle.setOutlineColor(sf::Color::Black);
}

void Ball::Update()
{
    position += velocity;
    if( enteringScreen )
    {
        static const sf::FloatRect windowRect = {0, 0, Window::width, Window::height};
        if(windowRect.contains(position.x, position.y))
        {
            enteringScreen = false;
        }
    }
    else
    {
        velocity *= 0.999f;
        float w = Window::width - radius;
        float h = Window::height - radius;

        if( position.x <= 0 || position.x >= w) velocity.x *= (-1);
        if( position.y <= 0 || position.y >= h) velocity.y *= (-1);

        position.x = glm::clamp<float>(position.x, 0, w);
        position.y = glm::clamp<float>(position.y, 0, h);
    }

    //Update the circle for drawing
    circle.setPosition(position.x, position.y);
}

void Ball::Draw(sf::RenderWindow &window)
{
    window.draw(circle);
}

bool Ball::IsCollide(const Ball &other)
{
    auto distance = glm::distance(position, other.position);

    if ( distance <= (radius + other.radius) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Ball::ResolveCollision(Ball &other)
{
    float minimal = radius + other.radius;
    position -= velocity;
    other.position -= other.velocity;
    glm::vec2 line = other.position - position;
    glm::vec2 un = glm::normalize(line);
    glm::vec2 ut = glm::vec2(-un.y, un.x);
    float distance = glm::dot(line, un);
    float v1n = glm::dot(velocity, un);
    float v2n = glm::dot(other.velocity, un);
    float v1t = glm::dot(ut, velocity);
    float v2t = glm::dot(ut, other.velocity);

    if(v1n == v2n)
    {
        return;
    }

    float t = (minimal - distance) / (v2n - v1n);
    position += t * velocity;
    other.position += t * other.velocity;
    float newV1n = ((v1n*(mass-other.mass))+(2*other.mass*v2n))/(mass + other.mass);
    float newV2n = ((v2n*(other.mass-mass))+(2*mass*v1n))/(mass + other.mass);
    velocity = (newV1n*un) + (v1t*ut);
    other.velocity = (newV2n*un) + (v2t*ut);

    position += (1.0f - t) * velocity;
    other.position += (1.0f -t) * other.velocity;
}
