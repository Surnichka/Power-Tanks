#include "PanelView.h"
#include "../Window.h"
#include "../PanelContext.h"
#include "../SignalDefinitions.h"
#include "../libs/Binder/Binder.h"
#include "../libs/GenericMsg/Msg.h"
#include "../SharedContext.h"
#include <iomanip>
#include <sstream>

namespace
{
    static constexpr float TEXT_SCALE = 0.7f;
    static sf::Vector2f HEALTH_POS = {Window::width / 2 - 110, Window::height - 90};
    static sf::Vector2f SCORE_POS = {Window::width / 2 - 110, Window::height - 40};
    static sf::Vector2f CRIT_CHANCE_POS = {320, Window::height - 90};
    static sf::Vector2f CRIT_DAMAGE_POS = {320, Window::height - 40};
    static sf::Vector2f BULLET_DAMAGE_POS = {50, Window::height - 90};
    static sf::Vector2f FIRE_RATE_POS = {50, Window::height - 40};
    static sf::Vector2f ULTIMATE_POS = {Window::width / 2 + 300, Window::height - 65};
}

void PanelView::Draw(sf::RenderWindow &window)
{
    int high_score_points = GetSharedContext().Get(Property::Highscore);
    int fire_rate = GetSharedContext().Get(Property::BulletFireRate);
    int crit_chance = GetSharedContext().Get(Property::CriticalChance);

    float health = GetSharedContext().Get<float>(Property::Health);
    float crit_dmg = GetSharedContext().Get<float>(Property::CriticalDamage);
    float bullet_damage = GetSharedContext().Get<float>(Property::BulletDamage);
    int ultimate_cooldown = GetSharedContext().Get(Property::UltimateCooldown);

    drawText(HEALTH_POS, "HEALTH: " + ToStr(health), window, {220, 0, 0});
    drawText(SCORE_POS, "POINTS: " + ToStr(high_score_points), window, {0, 153, 255});
    drawText(CRIT_CHANCE_POS, "CRIT CHANCE: " + ToStr(crit_chance), window);
    drawText(CRIT_DAMAGE_POS, "CRIT Damage: x" + ToStr(crit_dmg), window);
    drawText(BULLET_DAMAGE_POS, "BULLET DAMAGE: " + ToStr(bullet_damage), window);
    drawText(FIRE_RATE_POS, "FIRE RATE: 0." + ToStr(fire_rate), window);

    {
        std::string title = "ULTIMATE CD: " + ToStr(ultimate_cooldown);
        sf::Color color = sf::Color(153, 51, 153);
        if( ultimate_cooldown == 0 )
        {
            title = "ULTIMATE READY!";
            color = sf::Color::Magenta;
        }
        drawText(ULTIMATE_POS, title, window, color);
    }

    lvlCount.Draw(window);
}

void PanelView::Init()
{
    GetBinder().ConnectSlot(Signal::Enemy::Died, [this]()
    {
        lvlCount.GainExp();
    });
}

void PanelView::drawText(const sf::Vector2f &pos,
                           const std::string &title,
                           sf::RenderWindow &window,
                           const sf::Color &color)
{
    auto& font = FontMgr::Get().GetFont();

    sf::Text text;
    text.setScale(TEXT_SCALE, TEXT_SCALE);
    text.setFont(font);
    text.setFillColor(color);
    text.setString(title);
    text.setPosition(pos);
    window.draw(text);
}

std::string PanelView::ToStr(float value)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(1) << value;
    return stream.str();
}

std::string PanelView::ToStr(int value)
{
    return std::to_string(value);
}
