#include <iostream>
#include "GameApp.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window.hpp"
#include "Window.h"
#include "SignalSystem.h"

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
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { GetSignals().Dispatch("player_move_left"); }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { GetSignals().Dispatch("player_move_right"); }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { GetSignals().Dispatch("player_move_up"); }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { GetSignals().Dispatch("player_move_down"); }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) { GetSignals().Dispatch("player_shoot"); }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){ GetSignals().Dispatch("player_shoot_supernova"); }

        //Update metods
        gameApp.Update(20.0f);
        //Draw metods
        gameApp.Draw(window);

        window.display();
    }

    return 0;
}
