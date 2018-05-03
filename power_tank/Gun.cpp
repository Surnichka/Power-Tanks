#include "Gun.h"
#include "Window.h"
#include "utils/Utils.hpp"
#include "menus/DebugMenu.h"
#include "SignalDefinitions.h"
#include "libs/Binder/Binder.h"
#include "SFML/Window/Mouse.hpp"
#include "PanelContext.h"
#include "SharedContext.h"
#include <random>

namespace
{
    static std::random_device rd;
    static std::default_random_engine rng(rd());
    using Random = std::uniform_real_distribution<float>;
}

void Gun::Init()
{
    sf::Vector2f barrelSize = {7, 30};
    barrel.setSize(barrelSize);
    barrel.setOrigin(barrelSize.x / 2, barrelSize.y);
    barrel.setFillColor({121, 131, 134});
    barrel.setOutlineThickness(1.0f);
    barrel.setOutlineColor(sf::Color::Black);

    connectSignals();
    refreshContext();
}


void Gun::connectSignals()
{
    GetBinder().ConnectSlot(Signal::Bullet::Damage, [this](float dmg, float criticalDmg)
    {
        bullet_damage += dmg;
        crit_damage += criticalDmg;
        refreshContext();
    });

    GetBinder().ConnectSlot(Signal::Bullet::FireRate, [this](float fireRate, float bulletSpeed)
    {
        bullet_speed += bulletSpeed;
        bullet_fire_rate = std::max(100.0f, bullet_fire_rate + fireRate);
        refreshContext();
    });

    GetBinder().ConnectSlot(Signal::Bullet::Critical, [this](float criticalChance, float ultimateCD)
    {
        critical_chance += criticalChance;
        ultimate_cooldown += ultimateCD;
        refreshContext();
    });
}

void Gun::Ultimate()
{
    if(last_ultimate < ultimate_cooldown)
    {
        return;
    }
    last_ultimate = 0.0f;

    float step = 1.0f;
    for(float theta = 0; theta < 36; theta += step)
    {
        last_shoot = bullet_fire_rate;
        glm::vec2 dstPos;
        dstPos.x = barrel.getPosition().x + std::cos(theta);
        dstPos.y = barrel.getPosition().y + std::sin(theta);
        Shoot(dstPos);
    }
}

void Gun::Shoot()
{
    if( last_shoot < bullet_fire_rate )
    {
        return;
    }
    last_shoot = 0.0f;

    glm::vec2 destPos = {mousePos.x, mousePos.y};
    Shoot(destPos);
}


void Gun::Shoot(glm::vec2 destPos)
{
    glm::vec2 srcPos ;
    srcPos.x = barrel.getPosition().x;
    srcPos.y = barrel.getPosition().y;

    glm::vec2 disp = destPos - srcPos;
    float distance = std::hypot(disp.x, disp.y);
    glm::vec2 velocity = disp * (bullet_speed / distance);

    Ball bullet(srcPos, velocity, bullet_radius, 0.1f);
    bullet.SetMaxHealth(1);
    bullet.m_circle.setFillColor({121, 131, 134});
    bullet.OnWallCollide([](Ball& self)
    {
        self.TakeLife(1);
    });

    bullet.OnCollideOtherBall([this](Ball& self, Ball& other)
    {
        bool isCrit = Random(0.0f, 100.0f)(rng) <= critical_chance;
        float bullet_dmg = isCrit ? bullet_damage * crit_damage : bullet_damage;
        self.TakeLife(1);
        other.TakeLife(bullet_dmg);

        GetBinder().DispatchSignal(Signal::Enemy::GotHit, other.m_position.x, other.m_position.y, bullet_dmg, isCrit);

        float colorCoeff = utils::map<float>(other.m_currentHealth, 0, other.m_maxHealth, 0.0f, 1.0f);
        uint8_t c = uint8_t(255.0f * colorCoeff);
        other.m_circle.setFillColor({c, c, c});
    });
    bullets.emplace_back(std::move(bullet));
}

void Gun::Update(float dt)
{
    LookAtMousePos();
    last_shoot += dt;
    last_ultimate += dt;

    float cooldown = std::max(0.0f, (ultimate_cooldown - last_ultimate));
    GetSharedContext().Add(Property::UltimateCooldown, int(std::ceil((cooldown) / 1000.0f)));

    for (auto& bullet : bullets)
    {
        bullet.Update(dt);
    }

    auto iter = remove_if(bullets.begin(), bullets.end(), [](const Ball& bullet)
    {
       return bullet.IsAlive() == false;
    });
    bullets.erase(iter, bullets.end());
}

void Gun::Draw(sf::RenderWindow &window)
{
    mousePos = sf::Mouse::getPosition(window);

    for (auto& bullet : bullets)
    {
        bullet.Draw(window);
    }
    window.draw(barrel);
}

void Gun::setBarrelPositions(glm::vec2 pos)
{
    barrel.setPosition(pos.x, pos.y);
}

void Gun::LookAtMousePos()
{
     double dx = double(mousePos.x - barrel.getPosition().x);
     double dy = double(mousePos.y - barrel.getPosition().y);
     float angle = float(atan2(dy, dx) * 180 / M_PI - 270);
     barrel.setRotation(angle);
}

std::vector<Ball> &Gun::GetBullets()
{
    return bullets;
}

void Gun::refreshContext()
{
    GetSharedContext().Add(Property::BulletDamage, bullet_damage);
    GetSharedContext().Add(Property::BulletFireRate, bullet_fire_rate);
    GetSharedContext().Add(Property::CriticalChance, critical_chance);
    GetSharedContext().Add(Property::CriticalDamage, crit_damage);
}

//void Gun::DebugMenu()
//{
//    auto& debugMenu = GetDebugMenu();
//    debugMenu.AddButton("FIRE RATE +", [this]()
//    {
//        Gun::bullet_frame_rate = std::max(0.0f,bullet_frame_rate - 25.0f);
//        GetBinder().DispatchSignal(Signal::Bullet::FireRate,int(bullet_frame_rate));
//    });
//    debugMenu.AddButton("FIRE RATE -", [this]()
//    {
//        Gun::bullet_frame_rate += 25.0f;
//        GetBinder().DispatchSignal(Signal::Bullet::FireRate,int(bullet_frame_rate));
//    });

//    debugMenu.AddButton("BULLET SPEED +", [this]()
//    {
//        Gun::bullet_speed += 1.0f;
//        GetBinder().DispatchSignal(Signal::Bullet::Speed,int(bullet_speed));
//    });
//    debugMenu.AddButton("BULLET SPEED -", [this]()
//    {
//        Gun::bullet_speed = std::max(1.0f, bullet_speed - 1.0f);
//        GetBinder().DispatchSignal(Signal::Bullet::Speed,int(bullet_speed));
//    });

//    debugMenu.AddButton("BULLET DMG +", [this]()
//    {
//        Gun::bullet_damage += 1;
//        Gun::bullet_radius = std::min(bullet_radius + 1.0f, 8.0f);
//        GetBinder().DispatchSignal(Signal::Bullet::Damage,int(bullet_damage));
//    });
//    debugMenu.AddButton("BULLET DMG -", [this]()
//    {
//        Gun::bullet_damage = std::max(1.0f, bullet_damage - 1.0f);
//        Gun::bullet_radius = std::max(4.0f, bullet_radius - 1.0f);
//        GetBinder().DispatchSignal(Signal::Bullet::Damage,int(bullet_damage));
//    });
//}


