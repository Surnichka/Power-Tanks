#pragma once
#include "SFML/Graphics/RenderWindow.hpp"

class PanelView
{
public:
    PanelView();
    void Init();
    void DrawStats(sf::RenderWindow &window);
    void Draw(sf::RenderWindow &window);
private:
    // PLAYER
    int m_move_speed = 0;
    int m_health = 0;
    int m_totalPoints = 0;
    // GUN
    int m_fire_rate = 0;
    int m_bullet_speed = 0;
    int m_bullet_damage = 0;
    int m_ultimate_cooldown;
    // ENEMY
};
