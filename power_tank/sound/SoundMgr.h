#pragma once
#include "SFML/Audio.hpp"
#include <map>

class SoundMgr
{
public:
    void AddSound(const std::string& key, const std::string& path);
    void Play(const std::string& key);
    void Stop(const std::string& key);
    void StopAll();
private:
    struct SoundInfo
    {
        sf::SoundBuffer buffer;
        sf::Sound sound;
    };

    std::map<std::string, SoundInfo> m_sounds;
    bool m_enableSound = false;
};
