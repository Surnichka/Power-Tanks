#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/CircleShape.hpp"

class Enemies
{
public:
    void Init();
    void Update();
    void Draw(sf::RenderWindow& window);
private:
    struct Enemy
    {
        sf::Vector2f velocity = {3.5f, 5.0f};
        float radius = 15.0f;
        sf::CircleShape shape;
        bool inWindow = false;
        Enemy()
        {
            shape.setPosition(500, -100);
            shape.setFillColor(sf::Color::White);
            shape.setOutlineColor(sf::Color::Black);
            shape.setOutlineThickness(1.0f);
            shape.setRadius(radius);
        }
    };
    void SpawnEnemy();
    std::vector<Enemy> m_enemies;
};
