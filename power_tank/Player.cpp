#include "Player.h"
#include "Window.h"
#include "SignalSystem.h"
#include "SFML/Window/Mouse.hpp"
#include "cmath"

void Player::Init()
{
    float playeRadius = 20;
    player.setRadius(playeRadius);
    sf::Vector2f centerPos = {Window::width / 2.0f, Window::height / 2.0f};
    player.setOrigin(playeRadius, playeRadius);
    player.setPosition(centerPos);
    player.setFillColor({192,192,192});
    player.setOutlineThickness(1.0f);
    player.setOutlineColor(sf::Color::Black);

    sf::Vector2f playerCenter;
    playerCenter.x = centerPos.x + player.getRadius();
    playerCenter.y = centerPos.y + player.getRadius();

    sf::Vector2f barrelSize = {7, 30};
    barrel.setSize(barrelSize);
    barrel.setOrigin(barrelSize.x / 2, barrelSize.y);
    barrel.setPosition(centerPos);
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
    auto velocity = Move();
    auto nextMove = player.getPosition() + velocity;
    float radius = player.getRadius();

    if(nextMove.x + radius >= Window::width) { nextMove.x = int(Window::width - radius); }
    if(nextMove.x <= radius) { nextMove.x = radius; }
    if(nextMove.y + radius >= Window::height) { nextMove.y = int(Window::height - radius);}
    if(nextMove.y <= radius) { nextMove.y = radius; }

    player.setPosition(nextMove);
    barrel.setPosition(nextMove);
    direction = Direction::None;
}

void Player::Draw(sf::RenderWindow &window)
{
    mousePos = sf::Mouse::getPosition(window);
    LookAtMousePos();
    window.draw(player);
    window.draw(barrel);
}

sf::Vector2f Player::Move()
{
    //TODO - Fix diagonal speed
    sf::Vector2f returnValue = {0.0f, 0.0f};
    if( Direction::Up == (direction & Direction::Up)){ returnValue.y -= speed; }
    if ( Direction::Right == (direction & Direction::Right)){ returnValue.x += speed; }
    if ( Direction::Left == (direction & Direction::Left)){ returnValue.x -= speed; }
    if ( Direction::Down== (direction & Direction::Down)){ returnValue.y += speed; }
    return returnValue;
}

void Player::LookAtMousePos()
{
     double dx = double(mousePos.x - barrel.getPosition().x);
     double dy = double(mousePos.y - barrel.getPosition().y);
     double rotation = (atan2(dy, dx)) * 180 / M_PI;
     barrel.setRotation(float(rotation-270));
}



