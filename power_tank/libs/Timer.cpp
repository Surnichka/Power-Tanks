#include "Timer.h"

float Timer::GetFrameTime() const
{
    return m_frameTime;
}

void Timer::Process(float frameTime)
{
    m_frameTime = frameTime;
}
