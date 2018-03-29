#include "Player.h"
#include "Window.h"
#include "SignalSystem.h"
#include "SFML/Window/Mouse.hpp"
#include "cmath"

void Player::Init()
{
    float radius = 20;
    glm::vec2 centerPos = {Window::width / 2.0f, Window::height / 2.0f};

    player.Init(centerPos, {}, radius, radius);
    player.circle.setFillColor({192,192,192});

    sf::Vector2f playerCenter;
    playerCenter.x = centerPos.x + radius;
    playerCenter.y = centerPos.y + radius;

    sf::Vector2f barrelSize = {7, 30};
    barrel.setSize(barrelSize);
    barrel.setOrigin(barrelSize.x / 2, barrelSize.y);
    barrel.setPosition(centerPos.x, centerPos.y);
    barrel.setFillColor({192,192,192});
    barrel.setOutlineThickness(1.0f);
    barrel.setOutlineColor(sf::Color::Black);

    GetSignals().ConnectSlot("player_move_right", [this]() { direction |= Direction::Right; });
    GetSignals().ConnectSlot("player_move_left", [this]() { direction |= Direction::Left; });
    GetSignals().ConnectSlot("player_move_up", [this]() { direction |= Direction::Up; });
    GetSignals().ConnectSlot("player_move_down", [this]() { direction |= Direction::Down; });
}

void Player::Update()
{
    player.velocity = Move();
    direction = Direction::None;

    player.Update();
    barrel.setPosition(player.position.x, player.position.y);
}

void Player::Draw(sf::RenderWindow &window)
{
    mousePos = sf::Mouse::getPosition(window);
    LookAtMousePos();
    player.Draw(window);
    window.draw(barrel);
}

glm::vec2 Player::Move()
{
    //TODO - Fix diagonal speed
    glm::vec2 returnValue = {0.0f, 0.0f};
    if( Direction::Up == (direction & Direction::Up)){ returnValue.y -= speed; }
    if ( Direction::Right == (direction & Direction::Right)){ returnValue.x += speed; }
    if ( Direction::Left == (direction & Direction::Left)){ returnValue.x -= speed; }
    if ( Direction::Down == (direction & Direction::Down)){ returnValue.y += speed; }
    return returnValue;
}

Ball &Player::GetPlayer()
{
    return player;
}

void Player::LookAtMousePos()
{
     double dx = double(mousePos.x - barrel.getPosition().x);
     double dy = double(mousePos.y - barrel.getPosition().y);
     double rotation = (atan2(dy, dx)) * 180 / M_PI;
     barrel.setRotation(float(rotation-270));
}



