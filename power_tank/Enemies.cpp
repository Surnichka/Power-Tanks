#include "Enemies.h"
#include "Window.h"
#include "libs/Binder/Binder.h"
#include "SFML/Window/Mouse.hpp"
#include "menus/DebugMenu.h"
#include <random>
#include <cmath>
#include "SignalDefinitions.h"

namespace
{
    static std::random_device rd;
    static std::default_random_engine rng(rd());
    using Random = std::uniform_real_distribution<float>;
}

void Enemies::Init()
{
    GetBinder().ConnectSlot(Signal::Player::LevelUp, [this](int currentLevel)
    {
        max_enemies_in_screen += 2;
        spawn_rate = std::max(250.0f, spawn_rate - 250);
        health += 1;
        if( currentLevel == 3 )
        {
            chance_to_split = 25.0f;
        }
        if ( currentLevel == 5)
        {
            max_enemies_in_screen += 1;
            health += 1;
            chance_to_split = 50.0f;
        }
        if ( currentLevel == 8)
        {
            max_enemies_in_screen += 1;
            health += 1;
            chase_player--;
            chance_to_split = 75.0f;
        }
        if ( currentLevel == 10)
        {
            max_enemies_in_screen += 2;
            health += 1;
            chase_player--;
            chance_to_split = 100.0f;
        }
    });
}

void Enemies::Update(float dt, const Ball& player)
{
    for(auto& enemy : m_enemies)
    {
        if( false == enemy.ball.IsAlive() )
        {
            HandleSpawn(enemy);
        }
    }
    cleanUpDeadEnemies();

    spawn_elapsed += dt;
    if(spawn_elapsed > spawn_rate)
    {
        if( int(m_enemies.size()) < max_enemies_in_screen )
        {
            SpawnEnemy();
        }
        spawn_elapsed = 0.0f;
    }

    for(auto& b : m_enemies)
    {
        b.ball.Update(dt);
    }

    for(auto& b1 : m_enemies)
    {
        for(auto& b2 : m_enemies)
        {
            if( b1.ball.m_id == b2.ball.m_id ||
                b1.ball.m_enteringScreen ||
                b2.ball.m_enteringScreen )
            {
                continue;
            }

            if( b1.ball.IsCollide(b2.ball) )
            {
                b1.ball.ResolveCollision(b2.ball);
            }
        }
    }
    Seek(player);
    m_enemyGotHit.Update(dt);
}

std::vector<Enemy> &Enemies::GetEnemies()
{
    return m_enemies;
}

void Enemies::Draw(sf::RenderWindow &window)
{
    for(auto& b : m_enemies)
    {
        b.ball.Draw(window);
    }
    m_enemyGotHit.Draw(window);
}

void Enemies::Seek(const Ball& player)
{
    for(auto& b : m_enemies)
    {
       if(b.chasePlayer)
       {
           glm::vec2 desired_velocity = player.m_position - b.ball.m_position;
           b.ball.m_velocity = glm::normalize(desired_velocity) * b.max_speed;
       }
    }
}

void Enemies::cleanUpDeadEnemies()
{
    auto iter = remove_if(m_enemies.begin(), m_enemies.end(), [this](const Enemy& self)
    {
        if(false == self.ball.IsAlive())
        {
            GetBinder().DispatchSignal(Signal::Enemy::Died, self.ball.m_position.x, self.ball.m_position.y);
            return true;
        }
        return false;
    });
    m_enemies.erase(iter, m_enemies.end());
}

bool Enemies::HandleSpawn(Enemy& enemy)
{
    float chance = Random(0.0f, 100.0f)(rng);
    if( chance <= enemy.chance_to_split && enemy.ball.m_radius >= 12.0f)
    {

        SpawnEnemy(enemy.ball.m_position - glm::vec2(enemy.ball.m_radius/2, 0),
                   enemy.ball.m_radius / 1.2f, enemy.ball.m_maxHealth / 2, 0.0f);

        SpawnEnemy(enemy.ball.m_position + glm::vec2(enemy.ball.m_radius/2, 0),
                   enemy.ball.m_radius / 1.2f, enemy.ball.m_maxHealth / 2, 0.0f);
    }

    return false;
}

void Enemies::SpawnEnemy(glm::vec2 pos, float r, int health, float chanceToSplit)
{
    float speed = Random(3.0f, 6.0f)(rng);
    glm::vec2 centerPos = {Window::Playable::width / 2.0f, Window::Playable::height / 2.0f};
    glm::vec2 disp = centerPos - pos;
    float distance = std::hypot(disp.x, disp.y);
    glm::vec2 velocity = disp * (speed / distance);

    float radius = r;

    Enemy enemy;
    enemy.chance_to_split = chanceToSplit;
    enemy.ball.Init(pos, velocity, radius, radius * 10.0f);
    enemy.ball.SetMaxHealth(health);
    if( r >= chase_player )
    {
        enemy.ball.m_circle.setOutlineThickness(3.0f);
        enemy.chasePlayer = true;
    }
    m_enemies.push_back(enemy);
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
    float radius = Random(26.0f, 30.0f)(rng);
    SpawnEnemy(pos, radius, health, chance_to_split);
}

