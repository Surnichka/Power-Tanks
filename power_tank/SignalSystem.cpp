#include "SignalSystem.h"

void SignalSystem::ConnectSlot(const std::string &name, SignalSystem::Func func)
{
    m_signals[name].push_back(func);
}

void SignalSystem::ConnectSlot(const std::string &name, SignalSystem::FuncInt func)
{
    m_signalsInt[name].push_back(func);
}

void SignalSystem::ConnectSlot(const std::string &name, SignalSystem::FuncVec2 func)
{
    m_signalsVec2[name].push_back(func);
}

void SignalSystem::Dispatch(const std::string &name)
{
    auto iter = m_signals.find(name);
    if( iter == m_signals.end())
    {
        return;
    }

    for(auto func : iter->second)
    {
        func();
    }
}

void SignalSystem::Dispatch(const std::string &name, int value)
{
    auto iter = m_signalsInt.find(name);
    if( iter == m_signalsInt.end())
    {
        return;
    }

    for(auto func : iter->second)
    {
        func(value);
    }
}

void SignalSystem::Dispatch(const std::string &name, glm::vec2 vec2)
{
    auto iter = m_signalsVec2.find(name);
    if( iter == m_signalsVec2.end())
    {
        return;
    }

    for(auto func : iter->second)
    {
        func(vec2);
    }
}

SignalSystem &GetSignals()
{
    static SignalSystem signal;
    return signal;
}
