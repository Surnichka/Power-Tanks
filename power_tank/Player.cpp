#include "Player.h"
#include "Window.h"
#include "menus/DebugMenu.h"
#include "libs/Binder/Binder.h"
#include "cmath"
#include "SignalDefinitions.h"
#include "utils/ResourceMgr.h"
#include "PanelContext.h"
#include "SharedContext.h"

void Player::Init()
{
    m_playerKillSound.Init();

    DebugMenu();

    gun.Init();
    float radius = 20;
    glm::vec2 centerPos = {Window::width / 2.0f, Window::height / 2.0f};

    player.Init(centerPos, {}, radius, radius);
    gun.setBarrelPositions(player.getCurrentPosition());
    player.m_circle.setFillColor({92, 97, 112});
    player.SetMaxHealth(10.0f);

    player.OnCollideOtherBall([&](Ball& self, Ball& other)
    {
        if( IsInvulnarable() )
        {
            return;
        }

        elapsed_invulnaraibility_time = 0.0f;
        other.Destroy();

        self.TakeLife(1);
        refreshContext();

        if( false == self.IsAlive() )
        {
            self.m_circle.setFillColor(sf::Color::Red);
        }
    });

    sf::Vector2f playerCenter;
    playerCenter.x = centerPos.x + radius;
    playerCenter.y = centerPos.y + radius;

    connectSignals();
    refreshContext();
}

void Player::Update(float dt)
{
    m_playerKillSound.Update(dt);
    player.m_velocity = Move();
    direction = Direction::None;
    elapsed_invulnaraibility_time += dt;

    auto color = player.m_circle.getFillColor();
    color.a = IsInvulnarable() ? 30 : 255;
    player.m_circle.setFillColor(color);

    gun.Update(dt);
    player.Update(dt);
    gun.setBarrelPositions(player.getCurrentPosition());
    InputEvents();
}

void Player::Draw(sf::RenderWindow &window)
{
    player.Draw(window);
    gun.Draw(window);
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

void Player::connectSignals()
{
    GetBinder().ConnectSlot("player_move_right", [this]() { direction |= Direction::Right; });
    GetBinder().ConnectSlot("player_move_left", [this]() { direction |= Direction::Left; });
    GetBinder().ConnectSlot("player_move_up", [this]() { direction |= Direction::Up; });
    GetBinder().ConnectSlot("player_move_down", [this]() { direction |= Direction::Down; });
    GetBinder().ConnectSlot("player_shoot", [this]() { gun.Shoot(); });
    GetBinder().ConnectSlot("player_shoot_supernova", [this](){ gun.Ultimate(); });
    GetBinder().ConnectSlot(Signal::Enemy::Died, [this]()
    {
        m_highScorePoints++;
        refreshContext();
        player.AddLife(lifeStealAmount);
    });
    GetBinder().ConnectSlot(Signal::Player::MoveSpeed, [this](float value)
    {
        speed += value;
        refreshContext();
    });
    GetBinder().ConnectSlot(Signal::Bullet::LifeSteal, [this](float lifesteal)
    {
        lifeStealAmount += lifesteal;
    });
}

void Player::refreshContext()
{
    GetSharedContext().Add(Property::Health, player.GetCurrentHealth());
    GetSharedContext().Add(Property::MovementSpeed, speed);
    GetSharedContext().Add(Property::Highscore, m_highScorePoints);
}

bool Player::IsInvulnarable()
{
    return elapsed_invulnaraibility_time < invulnarabilityDuration;
}

void Player::InputEvents()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { GetBinder().DispatchSignal(Signal::Player::MoveLeft); }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { GetBinder().DispatchSignal(Signal::Player::MoveRight); }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { GetBinder().DispatchSignal(Signal::Player::MoveUp); }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { GetBinder().DispatchSignal(Signal::Player::MoveDown); }

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) { GetBinder().DispatchSignal(Signal::Player::Shoot); }
    if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){ GetBinder().DispatchSignal(Signal::Player::ShootUltimate); }
}

Gun &Player::GetGun()
{
    return gun;
}



