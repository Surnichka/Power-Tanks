#pragma once
#include "sound/SoundMgr.h"
#include <string>
#include <map>

class PlayerKillSound
{
public:
    void Init();
    void Update(float dt);
private:
    SoundMgr m_soundMgr;
    using KillCount = int;
    using SoundId = std::string;

    KillCount standard_counter = 0;
    KillCount consecutive_counter = 0;

    std::map<KillCount, SoundId> m_standardKillSound;
    std::map<KillCount, SoundId> m_consecutiveKillSounds;

    float elapsedForConsecutive = 0.0f;
    float timerForConsecutiveKills = 2000.0f;
};
