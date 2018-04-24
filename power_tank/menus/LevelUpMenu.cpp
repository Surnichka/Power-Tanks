#include "LevelUpMenu.h"
#include "../utils/ResourceMgr.h"
#include "../Window.h"
#include "../libs/Binder/Binder.h"
#include "../SignalDefinitions.h"

void LevelUpMenu::Init()
{
    InitResources();
}

void LevelUpMenu::HandlEvent(sf::Event event)
{
    if( totalPoints == 0 )
    {
        return;
    }

    sf::Color defaultColor = totalPoints == 0 ? sf::Color::Red : sf::Color(170, 170, 170);
    for(auto& box : skillsContainer)
    {
        box.sprite.setScale(1.0f, 1.0f);
        box.shape.setFillColor(defaultColor);

        if(event.type == sf::Event::MouseMoved )
        {
            if(box.shape.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y))
            {
                box.shape.setFillColor(sf::Color::Green);
            }
        }
        else if(event.type == sf::Event::MouseButtonReleased && sf::Mouse::Button::Left)
        {
            totalPoints--;
            if( nullptr == box.sendOnUpdate )
            {
                box.sendOnUpdate();
            }
            if( totalPoints == 0 )
            {
                return;
            }
        }
    }
}

void LevelUpMenu::InitResources()
{
    auto& resMgr = ResoruceMgr::Get();
    resMgr.InitLevelUpResources();

    levelPoints.setString("TOTAL POINTS: " + std::to_string(totalPoints));
    levelPoints.setPosition({600,500});
    setText(levelPoints, sf::Color::Black, {1.0f,1.0f});

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
    skillsContainer[1].sendOnUpdate = [](){ GetBinder().DispatchSignal(Signal::Bullet::Speed, 1.0f); };
    skillsContainer[2].sendOnUpdate = [](){ GetBinder().DispatchSignal(Signal::Bullet::FireRate, 1.0f); };
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
    connectSignals();
}

void LevelUpMenu::connectSignals()
{
    GetBinder().ConnectSlot(Signal::Player::LevelUp, [this]()
    {
        totalPoints++;
        levelPoints.setString("TOTAL POINTS: " + std::to_string(totalPoints));
    });
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
    sf::Color defaultColor = totalPoints == 0 ? sf::Color::Red : sf::Color(170, 170, 170);
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
    for (auto& vec : skillsContainer)
    {
        window.draw(vec.skillText);
        window.draw(vec.shape);
        window.draw(vec.sprite);
    }
    window.draw(levelPoints);
}
