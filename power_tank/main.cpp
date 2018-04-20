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
                window.close();

            if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space)
            {
                GetBinder().DispatchSignal("level_up");
            }

            if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F1)
            {
                GetBinder().DispatchSignal("toggle_debug_menu");
            }
            if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::P)
            {
                GetBinder().DispatchSignal("pause_game");
            }
            if(event.type == sf::Event::MouseButtonPressed &&
               event.mouseButton.button == sf::Mouse::Button::Left )
            {
                auto mousePos = sf::Mouse::getPosition(window);
                GetBinder().DispatchSignal("button_pressed", mousePos.x, mousePos.y );
                GetBinder().DispatchSignal("left_button_pressed", mousePos.x, mousePos.y );
            }
            if(event.type == sf::Event::MouseButtonPressed &&
               event.mouseButton.button == sf::Mouse::Button::Right )
            {
                auto mousePos = sf::Mouse::getPosition(window);
                GetBinder().DispatchSignal("right_button_pressed", mousePos.x, mousePos.y );
            }

        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { GetBinder().DispatchSignal("player_move_left"); }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { GetBinder().DispatchSignal("player_move_right"); }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { GetBinder().DispatchSignal("player_move_up"); }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { GetBinder().DispatchSignal("player_move_down"); }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){ GetBinder().DispatchSignal("player_shoot"); }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){ GetBinder().DispatchSignal("player_shoot_supernova"); }

        auto delta_time = deltaClock.restart();
        float dt = delta_time.asMilliseconds();
        ImGui::SFML::Update(window, delta_time);

        //Update metods
        gameApp.Update(dt);
        //Draw metods
        gameApp.Draw(window);
        GetDebugMenu().Draw(window);

        ImGui::Editor::Proccess();
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();

    return 0;
}
