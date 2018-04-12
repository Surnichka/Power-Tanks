#include <iostream>
#include "GameApp.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window.hpp"
#include "Window.h"
#include "utils/SignalSystem.h"
#include "menus/DebugMenu.h"

int main()
{
    GameApp gameApp;
    gameApp.Init();

    sf::RenderWindow window(sf::VideoMode(Window::width, Window::height), "PowerTank");
    window.setFramerateLimit(60);

    while(window.isOpen())
    {
        window.clear();

        sf::Event event;

        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();

            if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space)
            {
                GetSignals().Dispatch("level_up");
            }

            if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F1)
            {
                GetSignals().Dispatch("toggle_debug_menu");
            }
            if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::P)
            {
                GetSignals().Dispatch("pause_game");
            }
            if(event.type == sf::Event::MouseButtonPressed &&
               event.mouseButton.button == sf::Mouse::Button::Left )
            {
                auto mousePos = sf::Mouse::getPosition(window);
                GetSignals().Dispatch("button_pressed", {mousePos.x, mousePos.y} );
                GetSignals().Dispatch("left_button_pressed", {mousePos.x, mousePos.y} );
            }
            if(event.type == sf::Event::MouseButtonPressed &&
               event.mouseButton.button == sf::Mouse::Button::Right )
            {
                auto mousePos = sf::Mouse::getPosition(window);
                GetSignals().Dispatch("right_button_pressed", {mousePos.x, mousePos.y} );
            }

        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { GetSignals().Dispatch("player_move_left"); }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { GetSignals().Dispatch("player_move_right"); }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { GetSignals().Dispatch("player_move_up"); }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { GetSignals().Dispatch("player_move_down"); }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){ GetSignals().Dispatch("player_shoot"); }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){ GetSignals().Dispatch("player_shoot_supernova"); }

        //Update metods
        gameApp.Update(20.0f);
        //Draw metods
        gameApp.Draw(window);
        GetDebugMenu().Draw(window);

        window.display();
    }

    return 0;
}
