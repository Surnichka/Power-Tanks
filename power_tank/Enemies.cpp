#include "Enemies.h"
#include "Window.h"
#include "SignalSystem.h"
#include "SFML/Window/Mouse.hpp"
#include <random>
#include <cmath>

void Enemies::Init()
{
    GetSignals().ConnectSlot("SpawnEnemy", [this]()
    {
        SpawnEnemy();
    });
}

void Enemies::Update()
{
    static int counterTimer = 0;
    counterTimer++;
    if(counterTimer > 200)
    {
        SpawnEnemy();
        counterTimer = 0;
    }

    for(size_t i = 0; i < m_enemies.size(); i++)
    {
        for(size_t j = (i+1); j < m_enemies.size(); j++)
        {
            auto& lhsVel = m_enemies[i].velocity;
            auto& lhsShape = m_enemies[i].shape;

            auto& rhsVel = m_enemies[j].velocity;
            auto& rhsShape = m_enemies[j].shape;
        }
    }

    for(auto& c : m_enemies)
    {
        if( false == c.inWindow )
        {
            c.shape.move(c.velocity);
            const auto& pos = c.shape.getPosition();
            if( pos.x > c.radius &&
                pos.x + c.radius < Window::width &&
                pos.y > c.radius &&
                pos.y + c.radius < Window::height)
            {
                c.inWindow = true;
            }
        }
        else
        {
            auto nextMove = c.shape.getPosition() + c.velocity;
            if(nextMove.x + c.radius >= Window::width)
            {
                nextMove.x = int(Window::width - c.radius);
                c.velocity.x = -(c.velocity.x);
            }
            if(nextMove.x <= c.radius)
            {
                nextMove.x = c.radius;
                c.velocity.x = -(c.velocity.x);
            }
            if(nextMove.y + c.radius >= Window::height)
            {
                nextMove.y = int(Window::height - c.radius);
                c.velocity.y = -(c.velocity.y);
            }
            if(nextMove.y <= c.radius)
            {
                nextMove.y = c.radius;
                c.velocity.y = -(c.velocity.y);
            }
            c.shape.setPosition(nextMove);
        }
    }
}

void Enemies::Draw(sf::RenderWindow &window)
{
    for(const auto& c : m_enemies)
    {
        window.draw(c.shape);
    }
}

void Enemies::SpawnEnemy()
{
    std::random_device rd;
    std::default_random_engine rng(rd());
    using Random = std::uniform_real_distribution<float>;
    int placeSide = std::uniform_int_distribution<int>(1, 3)(rng);

    sf::Vector2f pos;
    switch( placeSide )
    {
        case 1: //Left Side
        {
            pos.x = Random(-50, -150)(rng);
            pos.y = Random(50, Window::height - 50)(rng);
        }break;
        case 2: //Right Side
        {
            pos.x = Random(Window::width + 50, Window::width + 150)(rng);
            pos.y = Random(50, Window::height - 50)(rng);
        }break;
        case 3: //Top Side
        {
             pos.x = Random(50, Window::width - 50)(rng);
             pos.y = Random(-50, - 150)(rng);
        }break;
    }

    float speed = Random(1.5f, 2.7f)(rng);
    sf::Vector2f centerPos = {Window::width / 2.0f, Window::height / 2.0f};
    sf::Vector2f disp = centerPos - pos;
    float distance = std::hypot(disp.x, disp.y);
    sf::Vector2f velocity = disp * (speed / distance);

    Enemy enemy;
    enemy.radius = Random(10.0f, 15.0f)(rng);
    enemy.velocity = velocity;
    enemy.shape.setPosition(pos);
    enemy.shape.setOrigin(enemy.radius, enemy.radius);
    m_enemies.push_back(enemy);
}

