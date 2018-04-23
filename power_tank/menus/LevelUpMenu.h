#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "../utils/FontMgr.h"

class LevelUpMenu
{
public:
    // TODO: (LAMBDA MASTER) make the sprites(sword,bullet... together with text
    // and the plus sprite seperate in a vector maybe?

    struct PassiveSkillStruct
    {
        sf::Text text;
        sf::Sprite sprite;
        sf::RectangleShape shape;
    };

    void Init();
    void InitResources();
    void setText(sf::Text& text,
                 const sf::Color& color = sf::Color::Black,
                 const sf::Vector2f& scale = {0.7f,0.7f} );
    void setShape();

    void Draw(sf::RenderWindow& window);

private:
    std::vector<PassiveSkillStruct> skillsContainer;
};
