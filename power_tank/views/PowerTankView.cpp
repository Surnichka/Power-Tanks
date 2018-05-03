#include "PowerTankView.h"
#include "../Window.h"
#include "../Gun.h"
#include "../ShowBallInfo.h"
#include "../libs/Binder/Binder.h"
#include "../SignalDefinitions.h"

void PowerTankView::Init()
{
    m_enemies.Init();
    m_player.Init();

    const int rowsCount = 50;
    int yPos = 0;
    int height = Window::Playable::height / rowsCount;
    for(int i = 0; i < rowsCount; i++)
    {
        sf::RectangleShape rect;
        rect.setPosition(0, yPos);
        rect.setSize({Window::Playable::width, float(height)});

        if( i % 2 ){ rect.setFillColor({170,170,170}); }
        else{ rect.setFillColor({144, 144, 144}); }

        yPos += height;

        m_background.emplace_back(std::move(rect));
    }

    connectSignals();
}

void PowerTankView::connectSignals()
{
    GetBinder().ConnectSlot("pause_game", [this]()
    {
        pause = !pause;
        GetBinder().DispatchSignal("draw_pause", pause);
    });
}

void PowerTankView::Update(float dt)
{
    if (pause)
    {
        return;
    }

    m_enemies.Update(dt, m_player.GetPlayer());
    m_player.Update(dt);

    auto& player = m_player.GetPlayer();
    auto& enemies = m_enemies.GetEnemies();

    for(auto& enemy : enemies)
    {
        if( player.IsCollide(enemy.ball) )
        {
            player.ResolveCollision(enemy.ball);
        }

        auto& player_bullets = m_player.GetGun().GetBullets();
        for(auto& bullet : player_bullets)
        {
            if( bullet.IsCollide(enemy.ball) )
            {
                bullet.ResolveCollision(enemy.ball);
            }
        }
    }

}

void PowerTankView::Draw(sf::RenderWindow &window)
{
    for(const auto& line : m_background) { window.draw(line); }
    m_player.Draw(window);
    m_enemies.Draw(window);
}

void PowerTankView::Show()
{

}

void PowerTankView::Hide()
{

}

void PowerTankView::OnEvent(const sf::Event& event)
{
    if( event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space)
    {
        GetBinder().DispatchSignal(Signal::View::RequestLevelUp);
    }
    if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::P)
    {
        GetBinder().DispatchSignal("pause_game");
    }
    if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F1)
    {
        GetBinder().DispatchSignal("toggle_debug_menu");
    }
}
