#include "Enemies.h"
#include "Window.h"
#include "SignalSystem.h"
#include "SFML/Window/Mouse.hpp"
#include "DebugMenu.h"
#include <random>
#include <cmath>

namespace
{
    static std::random_device rd;
    static std::default_random_engine rng(rd());
    using Random = std::uniform_real_distribution<float>;
}

void Enemies::Init()
{
//    SpawnEnemy({250, 250}, 50);
}

void Enemies::Update(float dt)
{
    for(const auto& ball : m_enemies)
    {
        if( false == ball.IsAlive() )
        {
            GetSignals().Dispatch("add_points");
            HandleSpawn(ball);
        }
    }
    cleanUpDeadEnemies();

    spawn_elapsed += dt;
    if(spawn_elapsed > spawn_rate)
    {
        if( int(m_enemies.size()) < max_enemies )
        {
            SpawnEnemy();
        }
        spawn_elapsed = 0.0f;
    }

    for(auto& b : m_enemies)
    {
        b.Update(dt);
    }

    for(auto& b1 : m_enemies)
    {
        for(auto& b2 : m_enemies)
        {
            if( b1.m_id == b2.m_id ||
                b1.m_enteringScreen ||
                b2.m_enteringScreen )
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

void Enemies::cleanUpDeadEnemies()
{
    auto iter = remove_if(m_enemies.begin(), m_enemies.end(), [this](const Ball& self)
    {
        return (false == self.IsAlive());
    });
    m_enemies.erase(iter, m_enemies.end());
}

bool Enemies::HandleSpawn(const Ball& self)
{
    //TODO - When Borko Becomes Imba - Make This Awesome
    if( self.m_radius <= max_split_radius )
    {
        return true;
    }

    float maxR = max_split_radius;
    float r = std::max(maxR, self.m_radius - 8.0f);

    SpawnEnemy(self.m_position - glm::vec2(self.m_radius/2, 0), r);
    SpawnEnemy(self.m_position + glm::vec2(self.m_radius/2, 0), r);

    return false;
}

void Enemies::SpawnEnemy(glm::vec2 pos, float r)
{
    float speed = Random(3.0f, 6.0f)(rng);
    glm::vec2 centerPos = {Window::Playable::width / 2.0f, Window::Playable::height / 2.0f};
    glm::vec2 disp = centerPos - pos;
    float distance = std::hypot(disp.x, disp.y);
    glm::vec2 velocity = disp * (speed / distance);

    float radius = r;

    Ball ball(pos, velocity, radius, radius * 10.0f);
    ball.SetMaxHealth(health);

    m_enemies.push_back(ball);
}

void Enemies::SpawnEnemy()
{
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
    float radius = Random(15.0f, 30.0f)(rng);
    SpawnEnemy(pos, radius);
}

