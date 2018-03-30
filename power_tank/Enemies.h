#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "Ball.h"

class Enemies
{
public:
    void Init();
    void Update(float dt);
    std::vector<Ball>& GetEnemies();
    void Draw(sf::RenderWindow& window);
private:
    void SpawnEnemy();
    std::vector<Ball> m_enemies;
};
