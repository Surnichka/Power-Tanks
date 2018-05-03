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
    void Update(float dt) final override;
    void Draw(sf::RenderWindow& window) final override;
    void Show() final override;
    void Hide() final override;
    void OnEvent(const sf::Event& event) final override;

private:
    void connectSignals();

    Player  m_player;
    Enemies m_enemies;
    bool pause = false;

    std::vector<sf::RectangleShape> m_background;
};
