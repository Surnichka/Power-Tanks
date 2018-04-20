#include "Bindable.h"

bool Bindable::IsPaused() const
{
    return m_isPaused;
}

void Bindable::SetPaused(bool paused, uint64_t now)
{
    if(paused)
    {
        if (false == m_isPaused)
        {
            m_lastPauseStart = now;
        }
        m_isPaused = true;
        OnObjectPause();
    }
    else
    {
        if (true == m_isPaused)
        {
            m_lastPauseDuration = now - m_lastPauseStart;
        }
        m_isPaused = false;
        OnObjectResume();
    }
    
    
}

Bindable::WeakPtr Bindable::GetWeakPtr()
{
    return shared_from_this();
}

///////////////////////////////////////////////////////////////////////////
uint64_t Bindable::GetLastPauseDuration() const
{
    return m_lastPauseDuration;
}

///////////////////////////////////////////////////////////////////////////
uint64_t Bindable::GetLastPauseStartTime() const
{
    return m_lastPauseStart;
}
