#pragma once
#include "../views/IView.h"
#include "../utils/FontMgr.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "../utils/LevelCounter.h"

class PanelView : public IView
{
public:
    void Draw(sf::RenderWindow &window) override;

    //Not used for now
    void Init() override;

    void Update(float) override{}
    void Show() override{}
    void Hide() override{}
private:
    void drawText(const sf::Vector2f& pos,
                  const std::string& title,
                  sf::RenderWindow &window,
                  const sf::Color& color = sf::Color::White);

    std::string ToStr(float value);
    std::string ToStr(int value);
    LevelCounter lvlCount;
};
