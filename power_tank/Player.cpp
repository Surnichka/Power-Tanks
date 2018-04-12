#include "Player.h"
#include "Window.h"
#include "menus/DebugMenu.h"
#include "utils/SignalSystem.h"
#include "cmath"

#include "utils/ResourceMgr.h"

void Player::Init()
{
    DebugMenu();

    gun.Init(300.0f, 10.5f, 1);
    float radius = 20;
    glm::vec2 centerPos = {Window::width / 2.0f, Window::height / 2.0f};

    player.Init(centerPos, {}, radius, radius);
    gun.setBarrelPositions(player.getCurrentPosition());
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

    GetSignals().Dispatch("move_speed",int(speed));
    GetSignals().ConnectSlot("player_move_right", [this]() { direction |= Direction::Right; });
    GetSignals().ConnectSlot("player_move_left", [this]() { direction |= Direction::Left; });
    GetSignals().ConnectSlot("player_move_up", [this]() { direction |= Direction::Up; });
    GetSignals().ConnectSlot("player_move_down", [this]() { direction |= Direction::Down; });
    GetSignals().ConnectSlot("player_shoot", [this]()
    {
        gun.Shoot();
    });
    GetSignals().ConnectSlot("player_shoot_supernova", [this]()
    {
        gun.Ultimate();
    });
    GetSignals().ConnectSlot("gain_exp", [this]()
    {
        lvlCount.GainExp();
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
    gun.setBarrelPositions(player.getCurrentPosition());
}

void Player::Draw(sf::RenderWindow &window)
{
    player.Draw(window);
    gun.Draw(window);
    lvlCount.Draw(window);
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
    debugMenu.AddButton("MOVE SPEED +", [this]()
    {
        Player::speed++;
        GetSignals().Dispatch("move_speed",int(speed));
    });

    debugMenu.AddButton("MOVE SPEED -", [this]()
    {
        Player::speed--;
        GetSignals().Dispatch("move_speed",int(speed));
    });
}

Gun &Player::GetGun()
{
    return gun;
}



