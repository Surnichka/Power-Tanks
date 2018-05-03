#include "LevelCounter.h"
#include "FontMgr.h"
#include "../Window.h"
#include "../SharedContext.h"
#include "../SignalDefinitions.h"
#include "../libs/Binder/Binder.h"
#include "../libs/Timer.h"
#include "../utils/Utils.hpp"
#include <math.h>

LevelCounter::LevelCounter()
{
    float expRate = 5.0f;
    for(int i = 1; i <= maxLevel; i++)
    {
        expirienceRequirements[i] = std::ceil(expRate);
        expirienceGained[i] = 0;
        expRate *= expirienceRate;
        expirienceRate = std::max(expirienceRate - 0.25f, 1.10f);
    }

    box.setSize({200, 20});
    box.setPosition(Window::width / 2 + 50 ,Window::height - 60);
    GetSharedContext().Add(Property::PlayerLevel, currentLevel);
}

void LevelCounter::Enable()
{
    enabled = true;
}

void LevelCounter::GainExp()
{
    if( false == enabled ){ return; }

    if( currentLevel == maxLevel )
    {
        return;
    }

    auto exp = (expirienceGained.at(currentLevel)++);
    if( exp >= expirienceRequirements.at(currentLevel) )
    {
        currentLevel++;
        GetSharedContext().Add(Property::PlayerLevel, currentLevel);

        GetBinder().DispatchSignal(Signal::Player::LevelUp, currentLevel);
        GetBinder().DispatchSignal(Signal::Ability::AddPoint, currentLevel);
        expirienceGained.at(currentLevel)++;
    }
}

int LevelCounter::GetCurrentLevel() const
{
    return currentLevel;
}

int LevelCounter::GetCurrentExp() const
{
    return expirienceGained.at(currentLevel);
}

void LevelCounter::Draw(sf::RenderWindow &window)
{
    if( false == enabled ){ return; }

    auto& font = FontMgr::Get().GetFont();

    //1. Draw current level
    auto pos = box.getPosition();
    sf::Text txtLvl;
    txtLvl.setPosition(pos.x + 65, pos.y - 30);
    txtLvl.setFont(font);
    txtLvl.setScale(0.7f,0.7f);
    txtLvl.setFillColor({255, 153, 0});
    txtLvl.setString("Level: " + std::to_string(currentLevel));
    window.draw(txtLvl);

    //2. Draw available points
    int levelPoints = GetSharedContext().Get(Property::AbilityPoint);

    static float elapsedForLevelPoints = 0.0f;

    uint8_t alpha = 255;
    if( levelPoints > 0 )
    {
        elapsedForLevelPoints += GetTimer().GetFrameTime();
        static constexpr float levelPointDuration = 1000.0f;
        if(elapsedForLevelPoints > levelPointDuration)
        {
            elapsedForLevelPoints = 0.0f;
        }

        if( elapsedForLevelPoints < levelPointDuration / 2)
        {
            alpha = uint8_t(utils::map<float>(elapsedForLevelPoints, 0.0f, levelPointDuration / 2, 255, 50));
        }
        else
        {
            alpha = uint8_t(utils::map<float>(elapsedForLevelPoints, levelPointDuration / 2, levelPointDuration, 50, 255));
        }
    }
    else
    {
        elapsedForLevelPoints = 0.0f;
    }

    sf::Text txtAvailablePoints;
    txtAvailablePoints.setPosition(pos.x + 13, pos.y + 20);
    txtAvailablePoints.setFont(font);
    txtAvailablePoints.setScale(0.7f,0.7f);
    txtAvailablePoints.setFillColor({255, 153, 0, alpha});
    txtAvailablePoints.setString("avaliable points: " + std::to_string(levelPoints));
    window.draw(txtAvailablePoints);

    //3. Draw exp bar
    if( currentLevel == maxLevel )
    {
        box.setFillColor({204, 102, 0});
        window.draw(box);
    }
    else
    {
        int currentExp = expirienceGained.at(currentLevel);
        int requiredExp = expirienceRequirements.at(currentLevel);
        float percentageToLvlUp = float(currentExp) / float(requiredExp);

        int boxWidth = int(box.getSize().x);
        float expBarLength = float(boxWidth) * percentageToLvlUp;

        auto expBarRect = box;
        expBarRect.setSize({expBarLength, box.getSize().y});
        expBarRect.setFillColor({255, 153, 0});

        window.draw(box);
        window.draw(expBarRect);

        sf::Text txtExpGained;
        std::string expGainedStr = std::to_string(currentExp) + " / " + std::to_string(requiredExp);
        txtExpGained.setString(expGainedStr);
        int centerTxtPos = pos.x + ((boxWidth - expGainedStr.size()) / 2);
        txtExpGained.setPosition(centerTxtPos - 20, pos.y - 4);
        txtExpGained.setFont(font);
        txtExpGained.setScale(0.7f,0.7f);
        txtExpGained.setFillColor({sf::Color::Black});
        window.draw(txtExpGained);
    }
}
