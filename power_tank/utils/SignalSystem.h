#pragma once
#include <functional>
#include <unordered_map>
#include <glm/glm.hpp>
#include <vector>

class SignalSystem
{
public:
    using Func = std::function<void()>;
    using FuncInt = std::function<void(int)>;
    using FuncVec2 = std::function<void(glm::vec2)>;

    using VectorFunc = std::vector<Func>;
    using VectorFuncInt = std::vector<FuncInt>;
    using VectorFuncVec2 = std::vector<FuncVec2>;

    void ConnectSlot(const std::string& name, Func func);
    void ConnectSlot(const std::string& name, FuncInt func);
    void ConnectSlot(const std::string& name, FuncVec2 func);

    void Dispatch(const std::string &name);
    void Dispatch(const std::string &name, int value);
    void Dispatch(const std::string &name, glm::vec2 vec2);
    //TODO - DisconnectSlot
private:
    std::unordered_map<std::string, VectorFunc> m_signals;
    std::unordered_map<std::string, VectorFuncInt> m_signalsInt;
    std::unordered_map<std::string, VectorFuncVec2> m_signalsVec2;
};
SignalSystem& GetSignals();
