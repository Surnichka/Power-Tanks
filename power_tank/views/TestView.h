#pragma once
#include "IView.h"
#include "SFML/Graphics.hpp"
#include "../boids/Boid.h"

class TestView : public IView
{
public:
    void Init() final override;
    void Update(float dt) final override;
    void Draw(sf::RenderWindow& window) final override;
    void Show() final override;
    void Hide() final override;
private:
    glm::vec2 m_mousePos;
    bool seek  = false;
    bool flee  = false;
    bool pause = false;
    std::vector<Boid> m_boids;
    std::vector<sf::CircleShape> m_circles;
};
