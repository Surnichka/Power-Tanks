#include "Enemies.h"
#include "Window.h"
#include "SignalSystem.h"
#include "SFML/Window/Mouse.hpp"
#include <random>
#include <cmath>

void Enemies::Init()
{

}

void Enemies::Update(float dt)
{
    auto iter = remove_if(m_enemies.begin(), m_enemies.end(), [](const Ball& bullet)
    {
       return bullet.IsAlive() == false;
    });
    m_enemies.erase(iter, m_enemies.end());

    static int counterTimer = 0;
    counterTimer++;
    if(counterTimer > 100)
    {
        SpawnEnemy();
        counterTimer = 0;
    }

    for(auto& b : m_enemies)
    {
        b.Update(dt);
    }

    for(auto& b1 : m_enemies)
    {
        for(auto& b2 : m_enemies)
        {
            if( b1.m_id == b2.m_id )
            {
                continue;
            }

            if( b1.IsCollide(b2) )
            {
                b1.ResolveCollision(b2);
            }
        }
    }
}

std::vector<Ball> &Enemies::GetEnemies()
{
    return m_enemies;
}

void Enemies::Draw(sf::RenderWindow &window)
{
    for(auto& b : m_enemies)
    {
        b.Draw(window);
    }
}

void Enemies::SpawnEnemy()
{
    std::random_device rd;
    std::default_random_engine rng(rd());
    using Random = std::uniform_real_distribution<float>;
    int placeSide = std::uniform_int_distribution<int>(1, 3)(rng);

    glm::vec2 pos;
    switch( placeSide )
    {
        case 1: //Left Side
        {
            pos.x = Random(-50, -150)(rng);
            pos.y = Random(50, Window::Playable::height - 50)(rng);
        }break;
        case 2: //Right Side
        {
            pos.x = Random(Window::Playable::width + 50, Window::Playable::width + 150)(rng);
            pos.y = Random(50, Window::Playable::height - 50)(rng);
        }break;
        case 3: //Top Side
        {
             pos.x = Random(50, Window::Playable::width - 50)(rng);
             pos.y = Random(-50, - 150)(rng);
        }break;
    }

    float speed = Random(5.f, 5.f)(rng);
    glm::vec2 centerPos = {Window::Playable::width / 2.0f, Window::Playable::height / 2.0f};
    glm::vec2 disp = centerPos - pos;
    float distance = std::hypot(disp.x, disp.y);
    glm::vec2 velocity = disp * (speed / distance);

    float radius = Random(10.0f, 15.0f)(rng);

    Ball ball(pos, velocity, radius, radius * 10.0f);
    ball.m_health = 5;

    m_enemies.push_back(ball);
}

