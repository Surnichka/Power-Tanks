#include "GameApp.h"
#include "views/PowerTankView.h"
#include "views/LevelUpView.h"
#include "views/BoidsExampleView.h"
#include "views/TestView.h"
#include "libs/Binder/Binder.h"
#include "menus/PanelView.h"
#include "Window.h"
#include "SignalDefinitions.h"

void GameApp::Init()
{
    m_views[Views::GamePlay] = std::make_shared<PowerTankView>();
    m_views[Views::LevelUp]  = std::make_shared<LevelUpView>();
    m_views[Views::BoidsExample] = std::make_shared<BoidsExampleView>();
    m_views[Views::Panel] = std::make_shared<PanelView>();
    m_views[Views::Test] = std::make_shared<TestView>();

    for(auto& p : m_views)
    {
        p.second->Init();
    }

    GetBinder().ConnectSlot(Signal::View::RequestLevelUp, [this]()
    {
        ChangeView({Views::LevelUp, Views::Panel});
    });

    GetBinder().ConnectSlot(Signal::View::RequestGamePlay, [this]()
    {
        ChangeView({Views::GamePlay, Views::Panel});
    });

    GetBinder().ConnectSlot("pause_game", [this]()
    {
        pause = !pause;
        GetBinder().DispatchSignal("draw_pause", pause);
    });
}

void GameApp::OnEvent(sf::Event event)
{
    for(const auto& viewName : m_activeViews)
    {
        m_views.at(viewName)->OnEvent(event);
    }
}

void GameApp::Update(float dt)
{
    if (pause)
    {
        return;
    }

    for(const auto& viewName : m_activeViews)
    {
        m_views.at(viewName)->Update(dt);
    }
}

void GameApp::Draw(sf::RenderWindow &window)
{
    for(const auto& viewName : m_activeViews)
    {
        m_views.at(viewName)->Draw(window);
    }
}

void GameApp::ChangeView(const std::vector<Views> &viewsName)
{
    for(const auto& viewName : m_activeViews)
    {
        m_views.at(viewName)->Hide();
    }

    m_activeViews = viewsName;

    for(const auto& viewName : m_activeViews)
    {
        m_views.at(viewName)->Show();
    }
}
