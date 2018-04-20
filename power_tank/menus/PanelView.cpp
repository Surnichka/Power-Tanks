#include "PanelView.h"
#include "../libs/Binder/Binder.h"
#include "../Window.h"

PanelView::PanelView()
{
    GetBinder().ConnectSlot("health_change", [this](int newHealth)
    {
        m_health = newHealth;
    });

    GetBinder().ConnectSlot("add_points", [this]()
    {
        ++m_totalPoints;
    });

    GetBinder().ConnectSlot("move_speed", [this](int speed)
    {
        m_move_speed = speed;
    });

    GetBinder().ConnectSlot("ultimate_cooldown", [this](int timeLeft)
    {
        m_ultimate_cooldown = timeLeft;
    });

    GetBinder().ConnectSlot("bullet_damage", [this](int bulletDmg)
    {
        m_bullet_damage = bulletDmg;
    });

    GetBinder().ConnectSlot("fire_rate", [this](int fire_rate)
    {
        m_fire_rate = fire_rate;
    });

    GetBinder().ConnectSlot("bullet_speed", [this](int bullet_speed)
    {
        m_bullet_speed = bullet_speed;
    });

    GetBinder().ConnectSlot("draw_pause", [this](int pause)
    {
        m_pause = pause;
    });
}

void PanelView::DrawStats(sf::RenderWindow &window)
{
    // PLAYER
    sf::Text txtHealth;
    setText(txtHealth,{220, 0, 0});
    txtHealth.setString("HEALTH: " + std::to_string(m_health));
    txtHealth.setPosition(Window::width / 2 - 110, Window::height - 90);
    window.draw(txtHealth);

    sf::Text txtPoints;
    setText(txtPoints,{0, 153, 255});
    txtPoints.setString("POINTS: " + std::to_string(m_totalPoints));
    txtPoints.setPosition(Window::width / 2 - 110, Window::height - 40);
    window.draw(txtPoints);

    int xGunPos = 50;
    sf::Text txtSpeed;
    setText(txtSpeed);
    txtSpeed.setString("MOVE SPEED: " + std::to_string(m_move_speed));
    txtSpeed.setPosition(xGunPos + 280, Window::height - 90);
    window.draw(txtSpeed);

    // GUN
    sf::Text txtBulletDmg;
    setText(txtBulletDmg);
    txtBulletDmg.setString("BULLET DAMAGE: " + std::to_string(m_bullet_damage));
    txtBulletDmg.setPosition(xGunPos, Window::height - 40);
    window.draw(txtBulletDmg);

    sf::Text txtBulletSpeed;
    setText(txtBulletSpeed);
    txtBulletSpeed.setString("BULLET SPEED: " + std::to_string(m_bullet_speed));
    txtBulletSpeed.setPosition(xGunPos, Window::height - 90);
    window.draw(txtBulletSpeed);

    sf::Text txtFireRate;
    setText(txtFireRate);
    txtFireRate.setString("FIRE RATE: 0." + std::to_string(m_fire_rate));
    txtFireRate.setPosition(xGunPos + 280, Window::height - 40);
    window.draw(txtFireRate);

    std::string ultimate;
    sf::Text txtUltimate;
    setText(txtUltimate,{153, 51, 153});
    if (m_ultimate_cooldown == 0)
    {
        txtUltimate.setFillColor(sf::Color::Magenta);
        ultimate = "READY!";
    }
    else
        ultimate = std::to_string(m_ultimate_cooldown);
    txtUltimate.setString("ULTIMATE CD: " + ultimate);
    txtUltimate.setPosition(Window::width / 2 + 300, Window::height - 65);
    window.draw(txtUltimate);

    sf::Text txtPause;
    setText(txtPause,sf::Color::Red,{4.0f,3.0f});
    txtPause.setString("PAUSE");
    txtPause.setPosition(Window::width / 2 - 200 ,Window::height / 2 - 200);
    if(m_pause)
    {
        window.draw(txtPause);
    }
}

void PanelView::setText(sf::Text& text, const sf::Color& color, const sf::Vector2f& scale)
{
    auto& font = FontMgr::Get().GetFont();

    text.setScale(scale);
    text.setFont(font);
    text.setFillColor(color);
}

void PanelView::Draw(sf::RenderWindow &window)
{
    DrawStats(window);
}
