#include "SoundMgr.h"

void SoundMgr::AddSound(const std::string &key, const std::string &path)
{
    auto iter = m_sounds.find(key);
    if(iter == m_sounds.end())
    {
        m_sounds[key].buffer.loadFromFile(path);
        m_sounds[key].sound.setBuffer(m_sounds[key].buffer);
        m_sounds[key].sound.setVolume(30);
    }
}

void SoundMgr::Play(const std::string &key)
{
    if (false == m_enableSound)
    {
        return;
    }

    auto iter = m_sounds.find(key);
    if(iter != m_sounds.end())
    {
        iter->second.sound.play();
    }
}

void SoundMgr::Stop(const std::string &key)
{
    auto iter = m_sounds.find(key);
    if( iter != m_sounds.end())
    {
        iter->second.sound.stop();
    }
}

void SoundMgr::StopAll()
{
    for(auto& p : m_sounds)
    {
        p.second.sound.stop();
    }
}
