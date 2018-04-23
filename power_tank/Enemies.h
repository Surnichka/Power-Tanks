#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "Ball.h"
#include "EnemyGotHit.h"

struct Enemy
{
    Ball ball;
    float chance_to_split = 0.0f;
    bool chasePlayer = false;
    float max_speed = 2.5f;
};

class Enemies
{
public:
    void Init();
    void Update(float dt, const Ball& player);
    std::vector<Enemy>& GetEnemies();
    void Draw(sf::RenderWindow& window);
private:
    void Seek(const Ball& player);
    EnemyGotHit m_enemyGotHit;

    int health = 1;
    int max_enemies_in_screen = 10;
    float spawn_elapsed = 0.0f;
    float spawn_rate = 300.0f;
    float chance_to_split = 0.0f;

    void cleanUpDeadEnemies();
    bool HandleSpawn(Enemy& self);
    void SpawnEnemy(glm::vec2 pos, float r, int health, float chanceToSplit = 0.0f);
    void SpawnEnemy();

    std::vector<Enemy> m_enemies;
};
