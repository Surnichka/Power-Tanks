#include "DebugMenu.h"
#include "../utils/FontMgr.h"
#include "../libs/Binder/Binder.h"
#include <iostream>

DebugMenu& GetDebugMenu()
{
    static DebugMenu debugMenu;
    return debugMenu;
}

DebugMenu::DebugMenu()
{
    GetBinder().ConnectSlot("toggle_debug_menu", [this]()
    {
        enabled = !enabled;
    });

    GetBinder().ConnectSlot("button_pressed", [this](int mouseX, int mouseY)
    {
        HandleInput({mouseX, mouseY});
    });
}

void DebugMenu::AddButton(const std::string &text, DebugMenu::Callback cb)
{
    if( buttons.size() >= maxRow * maxCol )
    {
        std::cout << "You reach the limit of maximum buttons: " << maxRow * maxCol << std::endl;
        return;
    }

    int xPos = int(150 + (buttons.size() / maxCol + (buttons.size() % maxCol)) * cellWidth);
    int yPos = int(150 + (buttons.size() / maxCol) * cellHeight);

    auto& font = FontMgr::Get().GetFont();

    sf::Text sfText;
    sfText.setFillColor(sf::Color::Black);
    sfText.setString(text);
    sfText.setFont(font);
    sfText.setScale(0.6f, 0.6f);
    sfText.setPosition(xPos , yPos + 25);

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
    BaseMenu::Draw(window);
}

void DebugMenu::HandleInput(glm::vec2 mousePos)
{
    BaseMenu::HandleInput(mousePos);
}
