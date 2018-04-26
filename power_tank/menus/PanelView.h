#pragma once
#include "../views/IView.h"
#include "../utils/FontMgr.h"
#include "SFML/Graphics/RenderWindow.hpp"

class PanelView : public IView
{
public:
    void Draw(sf::RenderWindow &window) override;
    void Refresh();

    //Not used for now
    void Init() override{}
    void Update(float) override{}
    void Show() override{}
    void Hide() override{}
private:
    void setText(sf::Text& text, const sf::Color& color = sf::Color::White, const sf::Vector2f& scale = {0.7f,0.7f} );

    // GUN
//    int m_move_speed = 0;
    int m_fire_rate = 0;
    float m_bullet_damage = 0.0f;
    int m_crit_chance = 0;
    float m_crit_dmg = 0;

    float m_health = 0;
    int m_ultimate_cooldown = 0;
    int m_high_score_points = 0;
};
