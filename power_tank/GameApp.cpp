#include "GameApp.h"
#include "views/PowerTankView.h"
#include "views/DefaultView.h"
#include "SignalSystem.h"

void GameApp::Init()
{
//    m_views["main_menu_view"] = std::make_shared<MainMenuView>();
    m_views["game_play_view"] = std::make_shared<PowerTankView>();
//    m_views["level_up_view"]  = std::make_shared<LevelUpView>();
    m_views["default_view"]   = std::make_shared<DefaultView>();

    for(auto& p : m_views)
    {
        p.second->Init();
    }

    GetSignals().ConnectSlot("back_button_pressed", [this]()
    {
        if( m_currentView == "level_up_view")
        {
            m_currentView = "game_play_view";
        }
        else if( m_currentView == "game_play_view" )
        {
            m_currentView = "main_menu_view";
        }
    });

    GetSignals().ConnectSlot("level_up_rune_taken", [this]()
    {
        m_currentView = "level_up_view";
    });
}

void GameApp::Update(float dt)
{
    m_views.at(m_currentView)->Update(dt);
}

void GameApp::Draw(sf::RenderWindow &window)
{
    m_views.at(m_currentView)->Draw(window);
}

void GameApp::ChangeView(const std::string &viewName)
{
    auto iter = m_views.find(viewName);
    if( iter == m_views.end() )
    {
        return;
    }

    auto view = m_views.at(m_currentView);
    view->Hide();

    m_currentView = viewName;
    m_views.at(m_currentView)->Show();
}
