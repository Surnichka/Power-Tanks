#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "Ball.h"
#include "Gun.h"
#include "utils/LevelCounter.h"
#include "PlayerKillSound.h"

class Player
{
public:
    void Init();
    void Update(float dt);
    void Draw(sf::RenderWindow& window);

    // GETTERS
    Gun& GetGun();
    Ball& GetPlayer();
private:
    bool IsInvulnarable();
    void DebugMenu();
    void InputEvents();
    glm::vec2 Move();

    enum Direction
    {
        None = 0, Left = 1, Right = 2, Up = 4, Down = 8,
    };

    float speed = 4.0f;

    Gun gun;
    Ball player;
    LevelCounter lvlCount;

    int direction = Direction::None;

    static constexpr float invulnarabilityDuration = 1000.0f;
    float elapsed_invulnaraibility_time = invulnarabilityDuration;

    PlayerKillSound m_playerKillSound;
};
