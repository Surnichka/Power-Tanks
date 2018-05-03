#include "LevelUpView.h"
#include "../Window.h"
#include "../utils/ResourceMgr.h"
#include "../libs/Binder/Binder.h"
#include "../SignalDefinitions.h"
#include "../PanelContext.h"
#include "../SharedContext.h"
#include <cmath>

LevelUpView::LevelUpView()
{
    background.setSize({Window::Playable::width,Window::Playable::height});
    background.setFillColor({170,170,170});
}

void LevelUpView::Init()
{
    GetSharedContext().Add(Property::AbilityPoint, m_abilityPoint);

    GetBinder().ConnectSlot(Signal::Ability::AddPoint, [this]()
    {
        m_abilityPoint++;
        GetSharedContext().Add(Property::AbilityPoint, m_abilityPoint);
    }, slots::HIGH_PRIORITY);

    GetBinder().ConnectSlot(Signal::Ability::ConsumePoint, [this]()
    {
        m_abilityPoint--;
        GetSharedContext().Add(Property::AbilityPoint, m_abilityPoint);
    }, slots::HIGH_PRIORITY);

    m_abilityBtns.emplace_back( CreateDamage({150, 100}) );
    m_abilityBtns.emplace_back( CreateAttackSpeed({450, 100}) );
    m_abilityBtns.emplace_back( CreateCritical({750, 100}) );
    m_abilityBtns.emplace_back( CreateLifesteal({1050, 100}) );
}

void LevelUpView::Update(float dt)
{

}

void LevelUpView::Draw(sf::RenderWindow &window)
{
    window.draw(background);
    for(auto& abilityBtn : m_abilityBtns)
    {
        abilityBtn->Draw(window);
    }
}

void LevelUpView::Show()
{
    for(auto& abilityBtn : m_abilityBtns)
    {
        abilityBtn->Refresh();
    }
}

void LevelUpView::Hide()
{

}

void LevelUpView::OnEvent(const sf::Event& event)
{
    if( event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space)
    {
        GetBinder().DispatchSignal(Signal::View::RequestGamePlay);
    }
    for(auto& abilityBtn : m_abilityBtns)
    {
        abilityBtn->HandleEvent(event);
    }
}


AbilityButton::uPtr LevelUpView::CreateDamage(const sf::Vector2f& pos)
{
    auto rawAbilityBtn = new AbilityButton();
    auto abilityBtn = std::unique_ptr<AbilityButton>(rawAbilityBtn);

    abilityBtn->Init(pos, 10);
    abilityBtn->SetHeader("Damage");
    abilityBtn->SetFooter(AbilityButton::State::OnFocus,
                                "Increases your damage with 0.7 \n"
                                "Increases your critical damage with x0.5");
    abilityBtn->SetPicture("sword");

    abilityBtn->EnableIf({AbilityButton::Requirement::AbilityPoint}, [this]()
    {
        return m_abilityPoint > 0;
    });

    abilityBtn->OnClick([this]()
    {
        GetBinder().DispatchSignal(Signal::Bullet::Damage, 0.7f, 0.5f);
    });

    return abilityBtn;
}

AbilityButton::uPtr LevelUpView::CreateAttackSpeed(const sf::Vector2f& pos)
{
    auto rawAbilityBtn = new AbilityButton();
    auto abilityBtn = std::unique_ptr<AbilityButton>(rawAbilityBtn);

    abilityBtn->Init(pos, 10);
    abilityBtn->SetHeader("Attack Speed");
    abilityBtn->SetFooter(AbilityButton::State::OnFocus,
                                "Increases your fire rate with 20% \n"
                                "Increases your bullet speed with 0.5");
    abilityBtn->SetPicture("quiver");

    abilityBtn->EnableIf({AbilityButton::Requirement::AbilityPoint}, [this]()
    {
        return m_abilityPoint > 0;
    });

    abilityBtn->OnClick([this]()
    {
        GetBinder().DispatchSignal(Signal::Bullet::FireRate, -100.0f, 0.5f);
    });

    return abilityBtn;
}

AbilityButton::uPtr LevelUpView::CreateCritical(const sf::Vector2f& pos)
{
    auto rawAbilityBtn = new AbilityButton();
    auto abilityBtn = std::unique_ptr<AbilityButton>(rawAbilityBtn);

    abilityBtn->Init(pos, 10);
    abilityBtn->SetHeader("Critical");
    abilityBtn->SetFooter(AbilityButton::State::OnFocus,
                                "Increases your critical \n"
                                "chance with 7% \n"
                                "and Decreases your ultimate \n"
                                "cooldown by 2.5 seconds");
    abilityBtn->SetPicture("critHit");

    abilityBtn->EnableIf({AbilityButton::Requirement::AbilityPoint}, [this]()
    {
        return m_abilityPoint > 0;
    });

    abilityBtn->OnClick([this]()
    {
        GetBinder().DispatchSignal(Signal::Bullet::Critical,  7.0f, -2500.0f);
    });

    return abilityBtn;
}

AbilityButton::uPtr LevelUpView::CreateLifesteal(const sf::Vector2f& pos)
{
    auto rawAbilityBtn = new AbilityButton();
    auto abilityBtn = std::unique_ptr<AbilityButton>(rawAbilityBtn);

    // TODO : FERI - strong typedef
    int maxLevel = 1;
    abilityBtn->Init(pos, maxLevel);
    abilityBtn->SetHeader("Life steal");
    abilityBtn->SetHeader(AbilityButton::State::OnFocus, "Life steal on Focus");

    abilityBtn->SetFooter(AbilityButton::State::Disabled,
                                "Requires level 3 and\n"
                                "2 ability points to unlock");
    abilityBtn->SetFooter(AbilityButton::State::OnFocus, "It gives you 0.1 hp per kill");

    abilityBtn->SetPicture("lifesteal");
    abilityBtn->SetConsumeAmount(2);

    abilityBtn->SetFooter(AbilityButton::State::Maxed, "Maxed level !");

    abilityBtn->EnableIf({AbilityButton::Requirement::AbilityPoint,
                          AbilityButton::Requirement::PlayerLevel}, [this]()
    {
        int playerLevel = GetSharedContext().Get(Property::PlayerLevel);
        return m_abilityPoint >= 2 && playerLevel >= 5;
    });

    abilityBtn->OnClick([this]()
    {
        GetBinder().DispatchSignal(Signal::Bullet::LifeSteal, 0.1f);
    });

    return abilityBtn;
}
