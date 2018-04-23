#include "EnemyGotHit.h"
#include "libs/Binder/Binder.h"
#include "SFML/Graphics/Text.hpp"
#include "utils/FontMgr.h"
#include "utils/Utils.hpp"
#include "SignalDefinitions.h"

EnemyGotHit::EnemyGotHit()
{
    GetBinder().ConnectSlot(Signal::Enemy::GotHit, [this](int xPos, int yPos, int damage)
    {
        ShowInfo info;
        info.damage = std::to_string(damage);
        info.xPos = xPos;
        info.yPos = yPos - 30;
        m_showInfo.push_back(info);
    });
    GetBinder().ConnectSlot(Signal::Enemy::Died, [this](int xPos, int yPos)
    {
        m_particles.Emit({xPos, yPos});
    });
}

void EnemyGotHit::Upadate(float dt)
{
    if(showDeadAnimation)
    {
        m_particles.Update(dt);
    }
    if( showHitDamage )
    {
        auto iter = std::remove_if(m_showInfo.begin(), m_showInfo.end(), [](const ShowInfo& info)
        {
            return info.elapsed > info.duration;
        });
        m_showInfo.erase(iter, m_showInfo.end());

        for(auto& info : m_showInfo)
        {
            info.elapsed += dt;
            if(info.elapsed < info.duration)
            {
                info.offsetY = int(utils::map<float>(info.elapsed, 0, info.duration, 0, info.desiredOffset));
            }
        }
    }
}


void EnemyGotHit::Draw(sf::RenderWindow& window)
{
    if(showDeadAnimation)
    {
        m_particles.Draw(window);
    }
    if( showHitDamage )
    {
        for(auto& info : m_showInfo)
        {
            auto& font = FontMgr::Get().GetFont();

            uint8_t alpha = 255;
            if( info.elapsed > info.fadingStart)
            {
                alpha = uint8_t(utils::map<float>(info.elapsed, info.fadingStart, info.duration, 255, 0));
            }

            sf::Text text;
            text.setPosition(info.xPos, info.yPos + info.offsetY);
            text.setString(info.damage);
            text.setFillColor({255, 255, 255, alpha});
            text.setOutlineColor({0, 0, 0, alpha});
            text.setOutlineThickness(2.5f);
            text.setFont(font);
            text.setScale(0.75f, 0.75f);
            window.draw(text);
        }
    }
}
