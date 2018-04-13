#include "DumbEnemy.h"
#include <random>
#include "../utils/FontMgr.h"
#include "../utils/Utils.hpp"

namespace
{
    static std::random_device rd;
    static std::default_random_engine rng(rd());
    using Random = std::uniform_real_distribution<float>;
}

DumbBoid::Ptr DumbBoid::Create(glm::vec2 loc)
{
    auto boid = std::make_shared<DumbBoid>();
    boid->location = loc;
    boid->velocity.x = Random(-3.5f, 3.5f)(rng);
    boid->velocity.y = Random(-3.5f, 3.5f)(rng);
    boid->radius = Random(12.0f, 22.f)(rng);

    boid->visual.body.setPosition(loc.x, loc.y);
    boid->visual.body.setOrigin(boid->radius, boid->radius);
    boid->visual.body.setRadius(boid->radius);
    boid->visual.body.setFillColor(sf::Color::White);
    boid->visual.body.setOutlineColor(sf::Color::Black);
    boid->visual.body.setOutlineThickness(1.0f);

    auto& font = FontMgr::Get().GetFont();
    boid->visual.health.setFont(font);
    boid->visual.health.setPosition(loc.x, loc.y);
    boid->visual.health.setFillColor(sf::Color::White);
    boid->visual.health.setOutlineColor(sf::Color::Black);
    boid->visual.health.setOutlineThickness(1.0f);

    return boid;
}

void DumbBoid::Update()
{
    IBoid::Update();
    currentHealth += healthRegen;

    float colorCoeff = utils::map<float>(currentHealth, 0, maxHealth, 0.0f, 1.0f);
    uint8_t c = uint8_t(255.0f * colorCoeff);
    visual.body.setFillColor({c, c, c});
    visual.body.setPosition(location.x, location.y);

    visual.health.setPosition(location.x, location.y);
    visual.health.setString(std::to_string(currentHealth));
}

void DumbBoid::Draw(sf::RenderWindow &window)
{
    window.draw(visual.body);
    //window.draw(visual.health);
}
