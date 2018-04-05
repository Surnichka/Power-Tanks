#include "DebugMenu.h"
#include "FontMgr.h"
#include <iostream>
#include "SignalSystem.h"

DebugMenu& GetDebugMenu()
{
    static DebugMenu debugMenu;
    return debugMenu;
}

DebugMenu::DebugMenu()
{
    GetSignals().ConnectSlot("toggle_debug_menu", [this]()
    {
        enabled = !enabled;
    });

    GetSignals().ConnectSlot("button_pressed", [this](glm::vec2 mousePos)
    {
        HandleInput(mousePos);
    });
}

void DebugMenu::AddButton(const std::string &text, DebugMenu::Callback cb)
{
    if( buttons.size() >= maxRow * maxCol )
    {
        std::cout << "You reach the limit of maximum buttons: " << maxRow * maxCol << std::endl;
        return;
    }

    int xPos = 100 + int(buttons.size() % maxCol) * cellWidth;
    int yPos = 100 + int(buttons.size() % maxRow) * cellHeight;

    auto& font = FontMgr::Get().GetFont();

    sf::Text sfText;
    sfText.setFillColor(sf::Color::Black);
    sfText.setString(text);
    sfText.setFont(font);
    sfText.setScale(0.6f, 0.6f);
    sfText.setPosition(xPos + 25, yPos + 25);

    sf::RectangleShape rect;
    rect.setPosition(xPos, yPos);
    rect.setSize({cellWidth, cellHeight});
    rect.setFillColor({255, 169, 169, 100});
    rect.setOutlineColor({0, 0, 0, 100});
    rect.setOutlineThickness(1);

    Button button;
    button.rect = rect;
    button.text = sfText;
    button.callback = cb;

    buttons.push_back(button);
}

void DebugMenu::Draw(sf::RenderWindow &window)
{
    if( false == enabled )
    {
        return;
    }

    for(const auto& button : buttons)
    {
        window.draw(button.rect);
        window.draw(button.text);
    }
}

void DebugMenu::HandleInput(glm::vec2 mousePos)
{
    if( false == enabled )
    {
        return;
    }

    for(const auto& button : buttons)
    {
        if( button.rect.getGlobalBounds().contains(mousePos.x, mousePos.y) )
        {
            if( nullptr != button.callback)
            {
                button.callback();
            }
        }
    }

}
