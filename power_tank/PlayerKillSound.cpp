#include "PlayerKillSound.h"
#include "libs/Binder/Binder.h"
#include "SignalDefinitions.h"

void PlayerKillSound::Init()
{
    GetBinder().ConnectSlot(Signal::Enemy::Died, [this]()
    {
        {
            standard_counter++;
            auto iter = m_standardKillSound.find(standard_counter);
            if( iter != m_standardKillSound.end() )
            {
                m_soundMgr.Play(iter->second);
            }
        }

        {
            consecutive_counter++;
            elapsedForConsecutive = 0.0f;
            auto iter = m_consecutiveKillSounds.find(consecutive_counter);
            if( iter != m_consecutiveKillSounds.end() )
            {
                //m_soundMgr.StopAll();
                //m_soundMgr.Play(iter->second);
            }
        }
    });

    GetBinder().ConnectSlot("player_shoot", [this]()
    {
        m_soundMgr.Play("shoot");
    });

    auto addSound = [this](std::map<KillCount, SoundId>& container,
            KillCount val, SoundId key, std::string path)
    {
        container[val] = key;
        m_soundMgr.AddSound(key, path);
    };

    addSound(m_standardKillSound, 1, "first_blood", "../audio/firstblood.wav");
    addSound(m_standardKillSound, 50, "killing_spree", "../audio/Killing_Spree.wav");
    addSound(m_standardKillSound, 100, "dominating", "../audio/Dominating.wav");
    addSound(m_standardKillSound, 250, "unstoppable", "../audio/Unstoppable.wav");
    addSound(m_standardKillSound, 500, "monster_kill", "../audio/MonsterKill.wav");
    addSound(m_standardKillSound, 1000, "mega_kill", "../audio/MegaKill.wav");
    addSound(m_standardKillSound, 2000, "wicked_sick", "../audio/WhickedSick.wav");
    addSound(m_standardKillSound, 5000, "god_like", "../audio/GodLike.wav");

    addSound(m_consecutiveKillSounds, 20, "double_kill", "../audio/Double_Kill.wav");
    addSound(m_consecutiveKillSounds, 40, "triple_kill", "../audio/triple_kill.wav");
    addSound(m_consecutiveKillSounds, 80, "ultra_kill", "../audio/UltraKill.wav");
    addSound(m_consecutiveKillSounds, 160, "rampage", "../audio/Rampage.wav");
}

void PlayerKillSound::Update(float dt)
{
    elapsedForConsecutive += dt;
    if( elapsedForConsecutive > timerForConsecutiveKills)
    {
        consecutive_counter = 0;
    }
}
