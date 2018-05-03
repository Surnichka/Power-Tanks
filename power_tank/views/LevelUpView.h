#pragma once
#include "IView.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "../menus/LevelUpMenu.h"
#include "../menus/level_up_menu/LevelUpAbility.h"
#include "../libs/GenericMsg/Msg.h"

class LevelUpView : public IView
{
public:
    LevelUpView();
    void Init() final override;
    void Update(float dt) final override;
    void Draw(sf::RenderWindow& window) final override;
    void Show() final override;
    void Hide() final override;
    void OnEvent(const sf::Event& event) final override;
private:
    AbilityButton::uPtr CreateDamage(const sf::Vector2f& pos);
    AbilityButton::uPtr CreateAttackSpeed(const sf::Vector2f& pos);
    AbilityButton::uPtr CreateCritical(const sf::Vector2f& pos);
    AbilityButton::uPtr CreateLifesteal(const sf::Vector2f& pos);

    int m_abilityPoint = 1;
    sf::RectangleShape background;
    std::vector<AbilityButton::uPtr> m_abilityBtns;
};
