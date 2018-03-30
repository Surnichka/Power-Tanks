#include "Gun.h"
#include "Window.h"

void Gun::Init(int frameRate, int damage)
{

}

void Gun::Shoot(const glm::vec2& srcPos, const glm::vec2& destPos)
{
    glm::vec2 disp = destPos - srcPos;
    float distance = std::hypot(disp.x, disp.y);
    glm::vec2 velocity = disp * (bullet_speed / distance);

    Ball bullet(srcPos, velocity, bullet_radius, bullet_mass);
    bullets.emplace_back(std::move(bullet));
}

void Gun::Update()
{
    for (auto& bullet : bullets)
    {
        bullet.Update();
    }

    auto iter = remove_if(bullets.begin(), bullets.end(), [](const Ball& bullet)
    {
       return bullet.isAlive == false;
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


