#include "PanelView.h"
#include "SignalSystem.h"
#include "FontMgr.h"
#include "Window.h"

PanelView::PanelView()
{
    GetSignals().ConnectSlot("health_change", [this](int newHealth)
    {
        m_health = newHealth;
    });

    GetSignals().ConnectSlot("add_points", [this]()
    {
        ++m_totalPoints;
    });

    GetSignals().ConnectSlot("move_speed", [this](int speed)
    {
        m_move_speed = speed;
    });

    GetSignals().ConnectSlot("ultimate_cooldown", [this](int timeLeft)
    {
        m_ultimate_cooldown = timeLeft;
    });

    GetSignals().ConnectSlot("bullet_damage", [this](int bulletDmg)
    {
        m_bullet_damage = bulletDmg;
    });

    GetSignals().ConnectSlot("fire_rate", [this](int fire_rate)
    {
        m_fire_rate = fire_rate;
    });

    GetSignals().ConnectSlot("bullet_speed", [this](int bullet_speed)
    {
        m_bullet_speed = bullet_speed;
    });

    GetSignals().ConnectSlot("draw_pause", [this](int pause)
    {
        m_pause = pause;
    });
}

void PanelView::DrawStats(sf::RenderWindow &window)
{
    auto& font = FontMgr::Get().GetFont();

    // PLAYER
    sf::Text txtHealth;
    txtHealth.setScale(0.8f,0.8f);
    txtHealth.setFont(font);
    txtHealth.setFillColor({153, 0, 0});
    txtHealth.setString("HEALTH: " + std::to_string(m_health));
    txtHealth.setPosition(Window::width / 2 - 15, Window::height - 90);
    window.draw(txtHealth);

    sf::Text txtPoints;
    txtPoints.setScale(0.8f,0.8f);
    txtPoints.setFont(font);
    txtPoints.setFillColor({51, 102, 153});
    txtPoints.setString("POINTS: " + std::to_string(m_totalPoints));
    txtPoints.setPosition(Window::width / 2 - 15, Window::height - 40);
    window.draw(txtPoints);

    sf::Text txtSpeed;
    txtSpeed.setScale(0.8f,0.8f);
    txtSpeed.setFont(font);
    txtSpeed.setFillColor(sf::Color::White);
    txtSpeed.setString("MOVE SPEED: " + std::to_string(m_move_speed));
    txtSpeed.setPosition(260, Window::height - 90);
    window.draw(txtSpeed);

    // GUN
    sf::Text txtBulletDmg;
    txtBulletDmg.setScale(0.8f,0.8f);
    txtBulletDmg.setFont(font);
    txtBulletDmg.setFillColor(sf::Color::White);
    txtBulletDmg.setString("BULLET DAMAGE: " + std::to_string(m_bullet_damage));
    txtBulletDmg.setPosition(10, Window::height - 40);
    window.draw(txtBulletDmg);

    sf::Text txtBulletSpeed;
    txtBulletSpeed.setScale(0.8f,0.8f);
    txtBulletSpeed.setFont(font);
    txtBulletSpeed.setFillColor(sf::Color::White);
    txtBulletSpeed.setString("BULLET SPEED: " + std::to_string(m_bullet_speed));
    txtBulletSpeed.setPosition(10, Window::height - 90);
    window.draw(txtBulletSpeed);

    sf::Text txtFireRate;
    txtFireRate.setScale(0.8f,0.8f);
    txtFireRate.setFont(font);
    txtFireRate.setFillColor(sf::Color::White);
    txtFireRate.setString("FIRE RATE: 0." + std::to_string(m_fire_rate));
    txtFireRate.setPosition(260, Window::height - 40);
    window.draw(txtFireRate);

    std::string ultimate;
    sf::Text txtUltimate;
    txtUltimate.setScale(0.8f,0.8f);
    txtUltimate.setFont(font);
    txtUltimate.setFillColor({153, 51, 153});
    if (m_ultimate_cooldown == 0)
        ultimate = "READY!";
    else
        ultimate = std::to_string(m_ultimate_cooldown);
    txtUltimate.setString("ULTIMATE CD: " + ultimate);
    txtUltimate.setPosition(Window::width / 2 + 200, Window::height - 70);
    window.draw(txtUltimate);

    elapsedTime += 20;
    sf::Text txtPause;
    txtPause.setScale(4.0f,3.0f);
    txtPause.setFont(font);
    txtPause.setFillColor(sf::Color::Red);
    txtPause.setString("PAUSE");
    txtPause.setPosition(Window::width / 2 - 200 ,Window::height / 2 - 200);
    if(m_pause)
    {
        window.draw(txtPause);
    }
}

void PanelView::Draw(sf::RenderWindow &window)
{
    DrawStats(window);
}
