#pragma once
#include "../utils/FontMgr.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include <functional>

class LevelUpMenu
{
public:
    struct PassiveSkillStruct
    {
        sf::Text skillText;
        sf::Sprite sprite;
        sf::RectangleShape shape;
        std::vector<sf::Text> skillsInfo;
        std::function<void()> sendOnUpdate = nullptr;
        bool drawInfoForSkills = false;
    };

    struct testSkills
    {
        sf::Text skillText;
        sf::Text skillInfo;
        sf::Text skillReq;
        sf::Sprite sprite;
        sf::RectangleShape shape;
        bool drawSkillInfo = false;
        bool drawSkillReq = false;
    };

    void Init();
    void InitTestSkills();
    void HandlEvent(sf::Event event);
    void HandleTestEvent(sf::Event event);
    void Draw(sf::RenderWindow& window);
    void RefreshColors();

private:
    void drawInfoForSkills(sf::RenderWindow& window);
    void connectSignals();
    void setShape();
    void setText(sf::Text& text,
                 const sf::Color& color = sf::Color::Black,
                 const sf::Vector2f& scale = {0.7f,0.7f} );
private:
    sf::Vector2i mouse;
    int levelPoints = 1;
    sf::Text levelPointsTxt;
    std::vector<PassiveSkillStruct> skillsContainer;

    testSkills testSkill;
};
