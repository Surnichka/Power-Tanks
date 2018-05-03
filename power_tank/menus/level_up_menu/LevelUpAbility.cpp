#include "LevelUpAbility.h"
#include "../../utils/FontMgr.h"
#include "../../utils/ResourceMgr.h"
#include "../../libs/Binder/Binder.h"
#include "../../SignalDefinitions.h"
#include "../../utils/Utils.hpp"

namespace
{
    static const int frameSize = 175;
    static sf::Vector2i mousePos;
}

namespace helper
{
sf::Text CreateText(float x, float y, sf::Color color = sf::Color::Black)
{
    auto& font = FontMgr::Get().GetFont();
    sf::Text text;
    text.setFont(font);
    text.setPosition(x, y);
    text.setScale(0.7f, 0.7f);
    text.setFillColor(color);
    utils::CenterSFMLText(text);
    return text;
}

sf::RectangleShape CreateFrame(float x, float y, sf::Color color)
{
    sf::RectangleShape frame;
    frame.setSize({frameSize,frameSize});
    frame.setPosition(x, y);
    frame.setOutlineColor(sf::Color::Black);
    frame.setOutlineThickness(2);
    frame.setFillColor(color);
    return frame;
}

}

AbilityButton::AbilityButton()
{
    m_sentiniel = std::make_shared<Bindable>();

    GetBinder().ConnectSlot(Signal::Player::LevelUp, m_sentiniel->GetWeakPtr(), [this]() { Refresh(); }, slots::LOW_PRIORITY);
    GetBinder().ConnectSlot(Signal::Ability::AddPoint, m_sentiniel->GetWeakPtr(), [this]() { Refresh(); }, slots::LOW_PRIORITY);
    GetBinder().ConnectSlot(Signal::Ability::ConsumePoint, m_sentiniel->GetWeakPtr(), [this]() { Refresh(); }, slots::LOW_PRIORITY);
}

void AbilityButton::Init(sf::Vector2f pos, int abilityMaxLevel)
{
    auto barOrientation = ProgressBar::Orientation::Vertical;
    auto barFillDir = ProgressBar::FillDirection::End;

    m_levelLeftBar.Init(sf::FloatRect(pos.x - 30, pos.y, 20, frameSize), abilityMaxLevel, barOrientation, barFillDir);
    m_levelRightBar.Init(sf::FloatRect(pos.x + frameSize + 10, pos.y, 20, frameSize), abilityMaxLevel, barOrientation,  barFillDir);

    m_header[State::Enabled] = helper::CreateText(pos.x + frameSize / 2, pos.y - 35);
    m_header[State::Disabled] = helper::CreateText(pos.x + frameSize / 2, pos.y - 35);
    m_header[State::OnFocus] = helper::CreateText(pos.x + frameSize / 2, pos.y - 35);
    m_header[State::Maxed] = helper::CreateText(pos.x + frameSize / 2, pos.y - 35);

    m_footer[State::Enabled] = helper::CreateText(pos.x + frameSize / 2, pos.y + frameSize);
    m_footer[State::Disabled] = helper::CreateText(pos.x + frameSize / 2, pos.y + frameSize);
    m_footer[State::OnFocus] = helper::CreateText(pos.x + frameSize / 2, pos.y + frameSize);
    m_footer[State::Maxed] = helper::CreateText(pos.x + frameSize / 2, pos.y + frameSize);

    m_pictureFrame[State::Enabled] = helper::CreateFrame(pos.x, pos.y, sf::Color(170, 170, 170) );
    m_pictureFrame[State::Disabled] = helper::CreateFrame(pos.x, pos.y, sf::Color::Red );
    m_pictureFrame[State::OnFocus] = helper::CreateFrame(pos.x, pos.y, sf::Color::Green );
    m_pictureFrame[State::Maxed] = helper::CreateFrame(pos.x, pos.y, sf::Color::Magenta );

    m_picture[State::Enabled].setPosition(pos.x + 20, pos.y + 20);
    m_picture[State::Disabled].setPosition(pos.x + 20, pos.y + 20);
    m_picture[State::OnFocus].setPosition(pos.x + 20, pos.y + 20);
    m_picture[State::Maxed].setPosition(pos.x + 20, pos.y + 20);
}

void AbilityButton::SetConsumeAmount(size_t abilityConsume)
{
    m_consumeAmount = abilityConsume;
}

void AbilityButton::SetHeader(const std::string& text)
{
    SetHeader(State::Enabled, text);
    SetHeader(State::Disabled, text);
    SetHeader(State::OnFocus, text);
    SetHeader(State::Maxed, text);
}

void AbilityButton::SetFooter(const std::string& text)
{
    SetFooter(State::Enabled, text);
    SetFooter(State::Disabled, text);
    SetFooter(State::OnFocus, text);
    SetFooter(State::Maxed, text);
}

void AbilityButton::SetPicture(const std::string& pictureKey)
{
    SetPicture(State::Enabled, pictureKey);
    SetPicture(State::Disabled, pictureKey);
    SetPicture(State::OnFocus, pictureKey);
    SetPicture(State::Maxed, pictureKey);
}

void AbilityButton::SetHeader(State state, const std::string &text)
{
    m_header.at(state).setString(text);
    utils::CenterSFMLTextForX(m_header.at(state));
}

void AbilityButton::SetFooter(AbilityButton::State state, const std::string &text)
{
    m_footer.at(state).setString(text);
    utils::CenterSFMLTextForX(m_footer.at(state));
}

void AbilityButton::SetPicture(AbilityButton::State state, const std::string &pictureKey)
{
    auto& texture = ResoruceMgr::Get().GetTexture(pictureKey);
    m_picture.at(state).setTexture(texture);
}

void AbilityButton::OnClick(AbilityButton::OnClickCB onClick)
{
    m_onClick = onClick;
}

void AbilityButton::EnableIf(std::set<AbilityButton::Requirement> requirements,
                             AbilityButton::EnableIfFunc enableFunc)
{
    m_requirements = requirements;
    m_enableIf = enableFunc;
}

void AbilityButton::HandleEvent(const sf::Event &event)
{
    if(State::Disabled == m_currentState ||
       State::Maxed == m_currentState)
    {
        return;
    }

    if(m_pictureFrame.at(m_currentState).getGlobalBounds().contains(mousePos.x, mousePos.y))
    {
        m_currentState = State::OnFocus;

        if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left)
        {
            if(nullptr != m_onClick)
            {
                for(size_t i = 0; i < m_consumeAmount; i++)
                {
                    GetBinder().DispatchSignal(Signal::Ability::ConsumePoint);
                }
                m_levelLeftBar.Add();
                m_levelRightBar.Add();
                m_onClick();
                if(m_levelLeftBar.isFull())
                {
                    m_currentState = State::Maxed;
                }
            }
        }
    }
    else
    {
        m_currentState = State::Enabled;
    }
}

void AbilityButton::Draw(sf::RenderWindow &window)
{
    mousePos = sf::Mouse::getPosition(window);

    window.draw(m_header.at(m_currentState));
    window.draw(m_pictureFrame.at(m_currentState));
    window.draw(m_picture.at(m_currentState));
    window.draw(m_footer.at(m_currentState));

    m_levelLeftBar.Draw(window);
    m_levelRightBar.Draw(window);
}

void AbilityButton::Refresh()
{
    if( m_levelLeftBar.isFull() )
    {
        m_currentState = State::Maxed;
    }
    else if( nullptr != m_enableIf )
    {
        if(m_currentState == State::OnFocus)
        {
            auto resultState = m_enableIf() ? State::Enabled : State::Disabled;
            if(resultState == State::Disabled)
            {
                m_currentState = resultState;
            }
        }
        else
        {
            m_currentState = m_enableIf() ? State::Enabled : State::Disabled;
        }
    }
}

