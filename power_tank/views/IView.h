#pragma once
#include <memory>
#include "SFML/Graphics/RenderWindow.hpp"
class IView
{
public:
    using Ptr = std::shared_ptr<IView>;

    IView() = default;
    virtual ~IView() = default;

    virtual void Init() = 0;
    virtual void Update(float dt) = 0;
    virtual void Draw(sf::RenderWindow& window) = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;
};
