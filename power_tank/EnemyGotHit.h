#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include "particles/ParticleSystem.h"

class EnemyGotHit
{
public:
    EnemyGotHit();
    void Upadate(float dt);
    void Draw(sf::RenderWindow& window);

    bool showDeadAnimation = true;
    bool showHitDamage = true;
private:
    struct ShowInfo
    {
        int xPos = 0;
        int yPos = 0;

        std::string damage;
        float elapsed = 0.0f;
        float duration = 300.0f;
        float fadingStart = 150.0f;

        int offsetY = 0;
        int desiredOffset = -50;
    };
    std::vector<ShowInfo> m_showInfo;
    ParticleSystem m_particles;
};