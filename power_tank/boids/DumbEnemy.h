#pragma once
#include "IBoid.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "../utils/ProgressBar.h" //TODO(feri)

class DumbBoid : public IBoid
{
public:
    using Ptr = std::shared_ptr<DumbBoid>;
    using WeakPtr = std::weak_ptr<DumbBoid>;

    static Ptr Create(glm::vec2 loc);
    void Update() override;
    void Draw(sf::RenderWindow& window) override;

    float healthRegen = 0.0f;
    float maxHealth = 10.0f;
    float currentHealth = maxHealth;

    struct Visual
    {
        sf::CircleShape body;
        sf::Text health;
    };
    Visual visual;
};
