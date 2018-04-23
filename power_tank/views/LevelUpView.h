#pragma once
#include "IView.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "../menus/LevelUpMenu.h"

class LevelUpView : public IView
{
public:
    void Init() final override;
    void Update(float dt) final override;
    void Draw(sf::RenderWindow& window) final override;
    void Show() final override;
    void Hide() final override;
    void OnEvent(sf::Event event) final override;
private:
    sf::RectangleShape rect;
    LevelUpMenu levelMenu;
};
