#include "LevelUpMenu.h"
#include "../Window.h"
#include "../PanelContext.h"
#include "../SignalDefinitions.h"
#include "../utils/ResourceMgr.h"
#include "../libs/Binder/Binder.h"
#include "../SharedContext.h"
#include "../PanelContext.h"
#include <iostream>

void LevelUpMenu::Init()
{
    auto& resMgr = ResoruceMgr::Get();
    levelPointsTxt.setString("LEVEL POINTS: " + std::to_string(levelPoints));
    levelPointsTxt.setPosition({600,500});
    setText(levelPointsTxt, sf::Color::Black, {1.0f,1.0f});

    skillsContainer.resize(3);
    skillsContainer[0].sprite = resMgr.GetSprite("sword");
    skillsContainer[1].sprite = resMgr.GetSprite("quiver");
    skillsContainer[2].sprite = resMgr.GetSprite("critHit");

    skillsContainer[0].skillText.setString("DAMAGE");
    skillsContainer[1].skillText.setString("ATTACK SPEED");
    skillsContainer[2].skillText.setString("CRITICAL");

    // Creating info for the skills
    for (auto& vec : skillsContainer) { vec.skillsInfo.resize(2); }
    skillsContainer[0].skillsInfo[0].setString("Increases your damage with 0.7");
    skillsContainer[0].skillsInfo[1].setString("Increases your critical damage with x0.5");
    skillsContainer[1].skillsInfo[0].setString("Increases your fire rate with 20%");
    skillsContainer[1].skillsInfo[1].setString("Increases your bullet speed with 0.5");
    skillsContainer[2].skillsInfo[0].setString("Increases your critical chance with 7%");
    skillsContainer[2].skillsInfo[1].setString("Decreases your ultimate cooldown by 2.5 seconds");

    int xInfoPos = 135;
    for (auto& vec : skillsContainer)
    {
        int yInfoPos = 270;
        for (auto& v : vec.skillsInfo)
        {
           v.setPosition(xInfoPos, yInfoPos);
           setText(v);
           yInfoPos += 20;
        }
        xInfoPos += 400;
    }
    skillsContainer[2].skillsInfo[0].setPosition(80,270);
    skillsContainer[2].skillsInfo[1].setPosition(80,290);

    skillsContainer[0].sendOnUpdate = [](){ GetBinder().DispatchSignal(Signal::Bullet::Damage, 0.7f, 0.5f ); };
    skillsContainer[1].sendOnUpdate = [](){ GetBinder().DispatchSignal(Signal::Bullet::FireRate, -100.0f, 0.5f); };
    skillsContainer[2].sendOnUpdate = [](){ GetBinder().DispatchSignal(Signal::Bullet::Critical, 7.0f, -2500.0f); };

    int yPos = 100;
    int xPos = 230;
    for (auto& vec : skillsContainer)
    {
        vec.sprite.setPosition(xPos, yPos);
        vec.shape.setPosition(xPos - 20, yPos - 20);
        vec.skillText.setPosition(xPos, yPos - 50);
        setText(vec.skillText);
        xPos += 400;
    }
    setShape();
    connectSignals();
    refreshContext();

    InitTestSkills();
}

void LevelUpMenu::InitTestSkills()
{
    auto& resMgr = ResoruceMgr::Get();

    testSkill.sprite = resMgr.GetSprite("lifesteal");
    testSkill.skillText.setString("LIFE STEAL");
    testSkill.skillInfo.setString("It gives you 0.1 hp per kill");
    testSkill.skillReq.setString("You need 3 points for this skill!");

    int yPos = 400;
    int xPos = 330;
    testSkill.sprite.setPosition(xPos, yPos);
    testSkill.shape.setPosition(xPos - 20, yPos - 20);
    testSkill.skillText.setPosition(xPos + 20 , yPos - 50);
    testSkill.skillInfo.setPosition(xPos - 80, yPos + 170);
    testSkill.skillReq.setPosition(xPos - 80, yPos + 170);
    setText(testSkill.skillText);
    setText(testSkill.skillInfo);
    setText(testSkill.skillReq);
    xPos += 400;

    testSkill.shape.setSize({175,175});
    testSkill.shape.setOutlineColor(sf::Color::Black);
    testSkill.shape.setOutlineThickness(3);
    testSkill.shape.setFillColor({255,255,255,0});
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

void LevelUpMenu::refreshContext()
{
    GetSharedContext().Add(Property::AbilityPoint, levelPoints);
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
            box.drawInfoForSkills = true;
        }
        else
        {
            box.shape.setFillColor({170,170,170});
            box.drawInfoForSkills = false;
        }

        if(box.shape.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
        {
            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left)
            {
                box.sprite.setScale(1.2f, 1.2f);
                box.drawInfoForSkills = false;
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
        for(auto& box : skillsContainer) { box.drawInfoForSkills = false; }
        RefreshColors();
    }

    HandleTestEvent(event);
}


void LevelUpMenu::HandleAllEvent(int levelPoints)
{
    // check for level points if they are more return

    // check if mouse is in global bounds in order to change the color and draw the info

    // chech if mouse is clicked in the global bounds in order to do

    //if mouse is on top of element, highlight it, only if it is enabled etc...

}

void LevelUpMenu::HandleTestEvent(sf::Event event)
{

    if ( levelPoints < 3)
    {
        testSkill.shape.setFillColor(sf::Color::Red);
        testSkill.drawSkillReq = testSkill.shape.getGlobalBounds().contains(mouse.x, mouse.y); // false
        return ;
    }

    if (testSkill.shape.getGlobalBounds().contains(mouse.x, mouse.y))
    {
        testSkill.shape.setFillColor(sf::Color::Green);
        testSkill.drawSkillInfo = true;
    }
    else
    {
        testSkill.shape.setFillColor({170,170,170});
        testSkill.drawSkillInfo = false;
    }

    if (testSkill.shape.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
    {
        if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left)
        {
            testSkill.sprite.setScale(1.2f, 1.2f);
            testSkill.drawSkillInfo = false;
        }
        else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left)
        {
            testSkill.shape.setScale(1.0f,1.0f);
            if (levelPoints >= 3)
            {
                testSkill.shape.setFillColor(sf::Color::Red);
                GetBinder().DispatchSignal("life_steal", true);
                levelPoints -= 3;
                GetPanelContext().AddValue("level_points", levelPoints);
                levelPointsTxt.setString("LEVEL POINTS: " + std::to_string(levelPoints));
            }
        }
    }

    testSkill.shape.setScale(1.0f,1.0f);

    if ( levelPoints < 3)
    {
        testSkill.drawSkillInfo = false;
        testSkill.shape.setFillColor(sf::Color::Red);
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
    for (const auto& vec : skillsContainer)
    {
        window.draw(vec.skillText);
        window.draw(vec.shape);
        window.draw(vec.sprite);
    }
    drawInfoForSkills(window);
    window.draw(levelPointsTxt);

// TESTING
    window.draw(testSkill.shape);
    window.draw(testSkill.skillText);
    window.draw(testSkill.sprite);
    if (testSkill.drawSkillInfo)
        window.draw(testSkill.skillInfo);
    if (testSkill.drawSkillReq)
        window.draw(testSkill.skillReq);
}

void LevelUpMenu::drawInfoForSkills(sf::RenderWindow &window)
{
    for (const auto& vec : skillsContainer)
    {
        for (const auto& v : vec.skillsInfo)
        {
            if (vec.drawInfoForSkills) { window.draw(v); }
        }
    }
}

