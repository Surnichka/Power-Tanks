#pragma once
#include <map>
#include <string>
#include "views/IView.h"
#include "SFML/Graphics/RenderWindow.hpp"

class GameApp
{
public:
    void Init();
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
    void ChangeView(const std::string& viewName);
private:
    std::string m_currentView = "game_play_view";
    std::map<std::string, IView::Ptr> m_views;
    bool pause = false;
};
