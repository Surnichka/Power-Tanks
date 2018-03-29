#include "SignalSystem.h"

void SignalSystem::ConnectSlot(const std::string &name, SignalSystem::Func func)
{
    m_signals[name].push_back(func);
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

SignalSystem &GetSignals()
{
    static SignalSystem signal;
    return signal;
}
