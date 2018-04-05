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
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
    glm::vec2 Move();

    // GETTERS
    Gun& GetGun();
    Ball& GetPlayer();
private:
    bool IsInvulnarable();
    void DebugMenu();

    void LookAtMousePos();
    enum Direction
    {
        None = 0, Left = 1, Right = 2, Up = 4, Down = 8,
    };

    float speed = 7.0f;
    sf::Vector2i mousePos;

    Gun gun;
    Ball player;
    sf::RectangleShape barrel;
    int direction = Direction::None;

    static constexpr float invulnarabilityDuration = 1000.0f;
    float elapsed_invulnaraibility_time = invulnarabilityDuration;
};
