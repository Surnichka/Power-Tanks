#pragma once
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "../../utils/ProgressBar.h"
#include "../../libs/Binder/Bindable.h"
#include <map>
#include <set>
#include <memory>
#include <functional>

class AbilityButton
{
public:
    using uPtr = std::unique_ptr<AbilityButton>;
    AbilityButton();

    using OnClickCB = std::function<void()>;
    using EnableIfFunc = std::function<bool()>;
    enum class State { Enabled, Disabled, OnFocus, Maxed};
    enum class Requirement { PlayerLevel, AbilityPoint };

    void Init(sf::Vector2f pos, int abilityMaxLevel);

    void SetConsumeAmount(size_t abilityConsume);

    void SetHeader(const std::string& text);
    void SetFooter(const std::string& text);
    void SetPicture(const std::string& pictureKey);

    void SetHeader(State state, const std::string& text);
    void SetFooter(State state, const std::string& text);
    void SetPicture(State state, const std::string& pictureKey);

    void OnClick(OnClickCB onClick);
    void EnableIf( std::set<Requirement> requirements, EnableIfFunc enableFunc);

    void HandleEvent(const sf::Event& event);
    void Draw(sf::RenderWindow& window);

    void Refresh();
private:
    size_t m_consumeAmount = 1;

    State m_currentState = State::Disabled;
    std::map<State, sf::Text> m_header;
    std::map<State, sf::Text> m_footer;
    std::map<State, sf::RectangleShape> m_pictureFrame;
    std::map<State, sf::Sprite> m_picture;

    ProgressBar m_levelLeftBar;
    ProgressBar m_levelRightBar;

    std::set<Requirement> m_requirements;
    OnClickCB m_onClick;
    EnableIfFunc m_enableIf;

    Bindable::Ptr m_sentiniel;
};
