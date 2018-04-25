#include <iostream>
#include "GameApp.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window.hpp"
#include "Window.h"
#include "libs/Binder/Binder.h"
#include "menus/DebugMenu.h"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "editor/Editor.h"
#include "SignalDefinitions.h"
#include "libs/Timer.h"

int main()
{
    GameApp gameApp;
    gameApp.Init();

    sf::RenderWindow window(sf::VideoMode(Window::width, Window::height), "PowerTank");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    while(window.isOpen())
    {
        window.clear();

        sf::Event event;

        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if(event.type == sf::Event::Closed)
            {
                window.close();
            }

            gameApp.OnEvent(event);

            if(event.type == sf::Event::MouseButtonPressed &&
               event.mouseButton.button == sf::Mouse::Button::Left )
            {
                auto mousePos = sf::Mouse::getPosition(window);
                GetBinder().DispatchSignal("left_button_pressed", mousePos.x, mousePos.y );
            }
            if(event.type == sf::Event::MouseButtonPressed &&
               event.mouseButton.button == sf::Mouse::Button::Right )
            {
                auto mousePos = sf::Mouse::getPosition(window);
                GetBinder().DispatchSignal("right_button_pressed", mousePos.x, mousePos.y );
            }

        }
        auto delta_time = deltaClock.restart();
        float dt = delta_time.asMilliseconds();

        //Update metods
        ImGui::SFML::Update(window, delta_time);
        GetTimer().Process(dt);
        gameApp.Update(dt);
        //Draw metods
        gameApp.Draw(window);
        GetDebugMenu().Draw(window);

//        ImGui::Editor::Proccess();
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();

    return 0;
}
