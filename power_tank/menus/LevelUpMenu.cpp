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

    int yPos = 100;
    sword.skillSprite = resMgr.GetSprite("sword");
    sword.skillSprite.setPosition(200, yPos );
    quiver.skillSprite = resMgr.GetSprite("quiver");
    quiver.skillSprite.setPosition(500, yPos);
    bullet.skillSprite = resMgr.GetSprite("bullet");
    bullet.skillSprite.setPosition(800, yPos );
    speed.skillSprite = resMgr.GetSprite("speed");
    speed.skillSprite.setPosition(1100, yPos);

    setText(sword.skillText);
    sword.skillText.setString("DAMAGE");
    sword.skillText.setPosition(200, yPos - 50);
    setText(quiver.skillText);
    quiver.skillText.setString("ATTACK SPEED");
    quiver.skillText.setPosition(500, yPos - 50);
    setText(bullet.skillText);
    bullet.skillText.setString("BULLET SPEED");
    bullet.skillText.setPosition(800, yPos - 50);
    setText(speed.skillText);
    speed.skillText.setString("MOVE SPEED");
    speed.skillText.setPosition(1100, yPos - 50);
}

void LevelUpMenu::setText(sf::Text& text, const sf::Color& color, const sf::Vector2f& scale)
{
    auto& font = FontMgr::Get().GetFont();

    text.setScale(scale);
    text.setFont(font);
    text.setFillColor(color);
}

void LevelUpMenu::Draw(sf::RenderWindow &window)
{
    auto& resMgr = ResoruceMgr::Get();
    DrawSprites(window);
}

void LevelUpMenu::DrawSprites(sf::RenderWindow &window)
{
    window.draw(sword.skillSprite);
    window.draw(sword.skillText);
    window.draw(quiver.skillSprite);
    window.draw(quiver.skillText);
    window.draw(bullet.skillSprite);
    window.draw(bullet.skillText);
    window.draw(speed.skillSprite);
    window.draw(speed.skillText);
}
