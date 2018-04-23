#include "LevelUpMenu.h"
#include "../utils/ResourceMgr.h"
#include "../Window.h"

void LevelUpMenu::Init()
{
    InitResources();

}

void LevelUpMenu::InitResources()
{
    auto& resMgr = ResoruceMgr::Get();
    resMgr.InitLevelUpResources();

    skillsContainer.resize(4);
    skillsContainer[0].sprite = resMgr.GetSprite("sword");
    skillsContainer[1].sprite = resMgr.GetSprite("quiver");
    skillsContainer[2].sprite = resMgr.GetSprite("bullet");
    skillsContainer[3].sprite = resMgr.GetSprite("speed");
    skillsContainer[0].text.setString("DAMAGE");
    skillsContainer[1].text.setString("ATTACK SPEED");
    skillsContainer[2].text.setString("BULLET SPEED");
    skillsContainer[3].text.setString("MOVE SPEED");

    int yPos = 100;
    int xPos = 200;
    for (auto& vec : skillsContainer)
    {
        vec.sprite.setPosition(xPos, yPos);
        vec.shape.setPosition(xPos - 20, yPos - 20);
        vec.text.setPosition(xPos, yPos - 50);
        setText(vec.text);

        xPos += 300;
    }
    setShape();
}

void LevelUpMenu::setText(sf::Text& text, const sf::Color& color, const sf::Vector2f& scale)
{
    auto& font = FontMgr::Get().GetFont();

    text.setScale(scale);
    text.setFont(font);
    text.setFillColor(color);
}

void LevelUpMenu::setShape()
{
    for (auto& vec : skillsContainer)
    {
        vec.shape.setSize({175,175});
        vec.shape.setOutlineColor(sf::Color::Black);
        vec.shape.setOutlineThickness(3);
        vec.shape.setFillColor({255,255,255,0});
    }
}

void LevelUpMenu::Draw(sf::RenderWindow &window)
{
    for (const auto& vec : skillsContainer)
    {
        window.draw(vec.text);
        window.draw(vec.sprite);
        window.draw(vec.shape);
    }
}
