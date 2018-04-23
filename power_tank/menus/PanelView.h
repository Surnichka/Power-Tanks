#pragma once
#include "../views/IView.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "../utils/FontMgr.h"

class PanelView : public IView
{
public:
    PanelView();
    void Init();
    void Draw(sf::RenderWindow &window);
    void Update(float dt) override;
    void Show() override;
    void Hide() override;
private:
    void DrawStats(sf::RenderWindow &window);
    void setText(sf::Text& text,
                 const sf::Color& color = sf::Color::White,
                 const sf::Vector2f& scale = {0.7f,0.7f} );
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

    //GAME STATE
    bool m_pause = false;
    float elapsedTime = 0.0f;
};
