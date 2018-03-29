#pragma once
#include <functional>
#include <unordered_map>
#include <vector>

class SignalSystem
{
public:
    using Func = std::function<void()>;
    using VectorFunc = std::vector<Func>;

    void ConnectSlot(const std::string& name, Func func);
    void Dispatch(const std::string& name);
    //TODO - DisconnectSlot
private:
    std::unordered_map<std::string, VectorFunc> m_signals;
};
SignalSystem& GetSignals();
