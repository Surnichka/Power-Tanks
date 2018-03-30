#include "Gun.h"
#include "Window.h"

void Gun::Init(const glm::vec2& playerPos, const sf::Vector2i& mousePos)
{
    float radius = 4;
    glm::vec2 velocity = {4.0f, 4.0f};
    glm::vec2 bulletPos = playerPos;
    Ball bullet;
    bullet.Init(bulletPos, velocity, radius, radius);
    bullet.isAlive = true;
    bullets.push_back(bullet);
    Shoot(mousePos);
}

void Gun::Shoot(const float& rotation)
{
    float xPos = mousePos.x;
    float yPos = mousePos.y;

    auto widthPos = Window::width / 2;
    auto heightPos = Window::height / 2;
}

void Gun::Update()
{
    auto iter = remove_if(bullets.begin(), bullets.end(), [](const Ball& bullet)
    {
       return bullet.isAlive == false;
    });

    bullets.erase(iter, bullets.end());
}

void Gun::Draw(sf::RenderWindow &window)
{
    for (const auto& bullet : bullets)
    {
        window.draw(bullet.circle);
    }
}
