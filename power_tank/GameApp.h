#pragma once
#include <map>
#include <string>
#include "views/IView.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

class GameApp
{
public:
    enum class Views { GamePlay, LevelUp, Panel, Test, BoidsExample };

    void Init();
    void OnEvent(sf::Event event);
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
    void ChangeView(const std::vector<Views>& viewsName);
private:
    std::vector<Views> m_activeViews = {Views::GamePlay, Views::Panel};
    std::map<Views, IView::Ptr> m_views;
};
