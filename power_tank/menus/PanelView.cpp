#include "PanelView.h"
#include "../Window.h"
#include "../PanelContext.h"
#include "../SignalDefinitions.h"
#include "../libs/Binder/Binder.h"
#include "../libs/GenericMsg/Msg.h"
#include <iomanip>
#include <sstream>

void PanelView::setText(sf::Text& text, const sf::Color& color, const sf::Vector2f& scale)
{
    auto& font = FontMgr::Get().GetFont();

    text.setScale(scale);
    text.setFont(font);
    text.setFillColor(color);
}

void PanelView::Draw(sf::RenderWindow &window)
{
    Refresh();

    // PLAYER
    sf::Text txtHealth;
    setText(txtHealth,{220, 0, 0});
    std::stringstream healthStream;
    healthStream << std::fixed << std::setprecision(1) << m_health;
    txtHealth.setString("HEALTH: " + healthStream.str());
    txtHealth.setPosition(Window::width / 2 - 110, Window::height - 90);
    window.draw(txtHealth);

    sf::Text txtPoints;
    setText(txtPoints,{0, 153, 255});
    txtPoints.setString("POINTS: " + std::to_string(m_high_score_points));
    txtPoints.setPosition(Window::width / 2 - 110, Window::height - 40);
    window.draw(txtPoints);

    int xGunPos = 50;
    sf::Text txtCritChance;
    setText(txtCritChance);
    txtCritChance.setString("CRIT CHANCE: " + std::to_string(m_crit_chance) + "%");
    txtCritChance.setPosition(xGunPos + 270, Window::height - 90);
    window.draw(txtCritChance);

    sf::Text txtCritDmg;
    setText(txtCritDmg);
    std::stringstream stream;
    stream << std::fixed << std::setprecision(1) << m_crit_dmg;
    txtCritDmg.setString("CRIT DAMAGE: x" + stream.str());
    txtCritDmg.setPosition(xGunPos + 270, Window::height - 40);
    window.draw(txtCritDmg);

    // GUN
    sf::Text txtBulletDmg;
    setText(txtBulletDmg);
    std::stringstream streamDmg;
    streamDmg << std::fixed << std::setprecision(1) << m_bullet_damage;
    txtBulletDmg.setString("BULLET DAMAGE: " + streamDmg.str());
    txtBulletDmg.setPosition(xGunPos, Window::height - 90);
    window.draw(txtBulletDmg);

    sf::Text txtFireRate;
    setText(txtFireRate);
    txtFireRate.setString("FIRE RATE: 0." + std::to_string(m_fire_rate));
    txtFireRate.setPosition(xGunPos, Window::height - 40);
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
    {
        ultimate = std::to_string(m_ultimate_cooldown);
    }

    txtUltimate.setString("ULTIMATE CD: " + ultimate);
    txtUltimate.setPosition(Window::width / 2 + 300, Window::height - 65);
    window.draw(txtUltimate);
}

void PanelView::Refresh()
{
//    GetPanelContext().GetValue("move_speed", m_move_speed);
    GetPanelContext().GetValue("health", m_health);
    GetPanelContext().GetValue("high_score_points", m_high_score_points);
    GetPanelContext().GetValue("bullet_fire_rate", m_fire_rate);
    GetPanelContext().GetValue("critical_chance", m_crit_chance);
    GetPanelContext().GetValue("critical_damage", m_crit_dmg);
    GetPanelContext().GetValue("bullet_damage", m_bullet_damage);
    GetPanelContext().GetValue("ultimate_cooldown", m_ultimate_cooldown);
}
