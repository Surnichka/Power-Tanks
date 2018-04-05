#include "Player.h"
#include "Window.h"
#include "DebugMenu.h"
#include "SignalSystem.h"
#include "SFML/Window/Mouse.hpp"
#include "cmath"

void Player::Init()
{
    DebugMenu();

    gun.Init(300.0f, 10.5f, 1);
    float radius = 20;
    glm::vec2 centerPos = {Window::width / 2.0f, Window::height / 2.0f};

    player.Init(centerPos, {}, radius, radius);
    player.m_circle.setFillColor({92, 97, 112});
    player.SetMaxHealth(10);
    GetSignals().Dispatch("health_change", player.GetCurrentHealth());
    player.OnCollideOtherBall([&](Ball& self, Ball& other)
    {
        if( IsInvulnarable() )
        {
            return;
        }

        elapsed_invulnaraibility_time = 0.0f;
        other.Destroy();
        self.TakeLife(1);
        GetSignals().Dispatch("health_change", self.GetCurrentHealth());
        if( false == self.IsAlive() )
        {
            self.m_circle.setFillColor(sf::Color::Red);
        }
    });

    sf::Vector2f playerCenter;
    playerCenter.x = centerPos.x + radius;
    playerCenter.y = centerPos.y + radius;

    sf::Vector2f barrelSize = {7, 30};
    barrel.setSize(barrelSize);
    barrel.setOrigin(barrelSize.x / 2, barrelSize.y);
    barrel.setPosition(centerPos.x, centerPos.y);
    barrel.setFillColor({121, 131, 134});
    barrel.setOutlineThickness(1.0f);
    barrel.setOutlineColor(sf::Color::Black);

    GetSignals().Dispatch("move_speed",int(speed));
    GetSignals().ConnectSlot("player_move_right", [this]() { direction |= Direction::Right; });
    GetSignals().ConnectSlot("player_move_left", [this]() { direction |= Direction::Left; });
    GetSignals().ConnectSlot("player_move_up", [this]() { direction |= Direction::Up; });
    GetSignals().ConnectSlot("player_move_down", [this]() { direction |= Direction::Down; });
    GetSignals().ConnectSlot("player_shoot", [this]()
    {
        glm::vec2 srcPos = {barrel.getPosition().x,barrel.getPosition().y};
        glm::vec2 dstPos = {mousePos.x, mousePos.y};
        gun.Shoot(srcPos, dstPos);
    });
    GetSignals().ConnectSlot("player_shoot_supernova", [this]()
    {
        glm::vec2 srcPos = {barrel.getPosition().x,barrel.getPosition().y};
        gun.Ultimate(srcPos);
    });
}

void Player::Update(float dt)
{
    player.m_velocity = Move();
    direction = Direction::None;
    elapsed_invulnaraibility_time += dt;

    auto color = player.m_circle.getFillColor();
    color.a = IsInvulnarable() ? 30 : 255;
    player.m_circle.setFillColor(color);

    gun.Update(dt);
    player.Update(dt);
    barrel.setPosition(player.m_position.x, player.m_position.y);
}

void Player::Draw(sf::RenderWindow &window)
{
    mousePos = sf::Mouse::getPosition(window);
    LookAtMousePos();

    gun.Draw(window);
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

bool Player::IsInvulnarable()
{
    return elapsed_invulnaraibility_time < invulnarabilityDuration;
}

void Player::DebugMenu()
{
    auto& debugMenu = GetDebugMenu();
    debugMenu.AddButton("add_speed", [this]()
    {
        Player::speed++;
        GetSignals().Dispatch("move_speed",int(speed));
    });

    debugMenu.AddButton("decrease_speed", [this]()
    {
        Player::speed--;
        GetSignals().Dispatch("move_speed",int(speed));
    });
}

Gun &Player::GetGun()
{
    return gun;
}

void Player::LookAtMousePos()
{
     double dx = double(mousePos.x - barrel.getPosition().x);
     double dy = double(mousePos.y - barrel.getPosition().y);
     float angle = float(atan2(dy, dx) * 180 / M_PI - 270);
     barrel.setRotation(angle);
}



