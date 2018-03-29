#pragma once
#include "IView.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "../Enemies.h"
#include "../Player.h"

class PowerTankView : public IView
{
public:
    void Init() final override;
    void Update() final override;
    void Draw(sf::RenderWindow& window) final override;
    void Show() final override;
    void Hide() final override;
private:
    Enemies m_enemies;
    Player  m_player;

    std::vector<sf::RectangleShape> m_background;
};
