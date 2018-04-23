#include "LevelUpView.h"
#include "../Window.h"
#include "../utils/ResourceMgr.h"
#include "../libs/Binder/Binder.h"
#include "../SignalDefinitions.h"

void LevelUpView::Init()
{
    rect.setSize({Window::Playable::width,Window::Playable::height});
    rect.setFillColor({170,170,170});
    levelMenu.Init();
}

void LevelUpView::Update(float dt)
{

}

void LevelUpView::Draw(sf::RenderWindow &window)
{
    window.draw(rect);
    levelMenu.Draw(window);
}

void LevelUpView::Show()
{

}

void LevelUpView::Hide()
{

}

void LevelUpView::OnEvent(sf::Event event)
{
    if( event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space)
    {
        GetBinder().DispatchSignal(Signal::View::RequestGamePlay);
    }
}
