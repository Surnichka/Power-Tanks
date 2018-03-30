#include "Gun.h"
#include "Window.h"

void Gun::Init(float frameRate, int damage)
{
    bullet_damage = damage;
    bullet_frame_rate = frameRate;
}

void Gun::Shoot(const glm::vec2& srcPos, const glm::vec2& destPos)
{
    if( last_shoot < bullet_frame_rate )
    {
        return;
    }
    last_shoot = 0.0f;

    glm::vec2 disp = destPos - srcPos;
    float distance = std::hypot(disp.x, disp.y);
    glm::vec2 velocity = disp * (bullet_speed / distance);

    Ball bullet(srcPos, velocity, bullet_radius, 0.1f);
    bullet.OnWallCollide([](Ball& self)
    {
        self.Destroy();
    });

    bullet.OnCollideOtherBall([this](Ball& self, Ball& other)
    {
        self.Destroy();
        other.TakeLife(bullet_damage);
        if( false == other.IsAlive() )
        {
            return;
        }

        float healthPercent = 1.0f / float(other.m_health);
        uint8_t c = uint8_t(255.0f - (255.0f * healthPercent));
        other.m_circle.setFillColor({c, c, c});
    });
    bullets.emplace_back(std::move(bullet));
}

void Gun::Update(float dt)
{
    last_shoot += dt;

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
    for (auto& bullet : bullets)
    {
        bullet.Draw(window);
    }
}

std::vector<Ball> &Gun::GetBullets()
{
    return bullets;
}


