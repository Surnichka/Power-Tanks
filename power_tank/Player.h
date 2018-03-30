#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "Ball.h"
#include "Gun.h"

class Player
{
public:
    void Init();
    void Update();
    void Draw(sf::RenderWindow& window);
    glm::vec2 Move();
    Ball& GetPlayer();
private:
    void LookAtMousePos();
    Gun gun;
    enum Direction
    {
        None = 0, Left = 1, Right = 2, Up = 4, Down = 8,
    };

    float speed = 5.0f;
    sf::Vector2i mousePos;

    Ball player;
    sf::RectangleShape barrel;
    int direction = Direction::None;
};
