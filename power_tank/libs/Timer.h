#pragma once

class Timer
{
public:
    float GetFrameTime() const;
    void Process(float frameTime);
private:
    float m_frameTime;
};

inline Timer& GetTimer()
{
    static Timer timer;
    return timer;
}
