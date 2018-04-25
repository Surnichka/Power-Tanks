#include "LevelUpMenu.h"
#include "../Window.h"
#include "../PanelContext.h"
#include "../SignalDefinitions.h"
#include "../utils/ResourceMgr.h"
#include "../libs/Binder/Binder.h"
#include "../PanelContext.h"
#include <iostream>

void LevelUpMenu::Init()
{
    auto& resMgr = ResoruceMgr::Get();
    resMgr.InitLevelUpResources();

    levelPointsTxt.setString("LEVEL POINTS: " + std::to_string(levelPoints));
    levelPointsTxt.setPosition({600,500});
    setText(levelPointsTxt, sf::Color::Black, {1.0f,1.0f});

    skillsContainer.resize(4);
    skillsContainer[0].sprite = resMgr.GetSprite("sword");
    skillsContainer[1].sprite = resMgr.GetSprite("quiver");
    skillsContainer[2].sprite = resMgr.GetSprite("bullet");
    skillsContainer[3].sprite = resMgr.GetSprite("speed");

    skillsContainer[0].skillText.setString("DAMAGE");
    skillsContainer[1].skillText.setString("ATTACK SPEED");
    skillsContainer[2].skillText.setString("BULLET SPEED");
    skillsContainer[3].skillText.setString("MOVE SPEED");

    skillsContainer[0].sendOnUpdate = [](){ GetBinder().DispatchSignal(Signal::Bullet::Damage, 1.0f); };
    skillsContainer[1].sendOnUpdate = [](){ GetBinder().DispatchSignal(Signal::Bullet::FireRate, -50.0f); };
    skillsContainer[2].sendOnUpdate = [](){ GetBinder().DispatchSignal(Signal::Bullet::Speed, 1.0f); };
    skillsContainer[3].sendOnUpdate = [](){ GetBinder().DispatchSignal(Signal::Player::MoveSpeed, 1.0f); };

    int yPos = 100;
    int xPos = 200;
    for (auto& vec : skillsContainer)
    {
        vec.sprite.setPosition(xPos, yPos);
        vec.shape.setPosition(xPos - 20, yPos - 20);
        vec.skillText.setPosition(xPos, yPos - 50);
        setText(vec.skillText);
        xPos += 300;
    }
    setShape();

    GetPanelContext().AddValue("level_points", levelPoints);
    connectSignals();
}

void LevelUpMenu::connectSignals()
{
    GetBinder().ConnectSlot(Signal::Player::LevelUp, [this]()
    {
        levelPoints++;
        GetPanelContext().AddValue("level_points", levelPoints);
        levelPointsTxt.setString("LEVEL POINTS: " + std::to_string(levelPoints));
    });
}

void LevelUpMenu::HandlEvent(sf::Event event)
{
    if( levelPoints == 0 )
    {
        return;
    }

    for(auto& box : skillsContainer)
    {
        box.sprite.setScale(1.0f, 1.0f);
        if(box.shape.getGlobalBounds().contains(mouse.x, mouse.y))
        {
            box.shape.setFillColor(sf::Color::Green);
        }
        else
        {
            box.shape.setFillColor({170,170,170});
        }

        if(box.shape.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
        {
            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left)
            {
                box.sprite.setScale(1.2f, 1.2f);
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left)
            {
                levelPoints--;
                GetPanelContext().AddValue("level_points", levelPoints);
                levelPointsTxt.setString("LEVEL POINTS: " + std::to_string(levelPoints));
                if( nullptr != box.sendOnUpdate )
                {
                    box.sendOnUpdate();
                }
            }
        }
    }
    if( levelPoints == 0 )
    {
        RefreshColors();
    }
}

void LevelUpMenu::setText(sf::Text& text, const sf::Color& color, const sf::Vector2f& scale)
{
    auto& font = FontMgr::Get().GetFont();

    text.setScale(scale);
    text.setFont(font);
    text.setFillColor(color);
}

void LevelUpMenu::RefreshColors()
{
    sf::Color defaultColor = levelPoints == 0 ? sf::Color::Red : sf::Color(170, 170, 170);
    for(auto& box : skillsContainer)
    {
        box.sprite.setScale(1.0f, 1.0f);
        box.shape.setFillColor(defaultColor);
    }
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
    mouse = sf::Mouse::getPosition(window);
    for (auto& vec : skillsContainer)
    {
        window.draw(vec.skillText);
        window.draw(vec.shape);
        window.draw(vec.sprite);
    }
    window.draw(levelPointsTxt);
}
