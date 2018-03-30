#pragma once
#include "IView.h"

class DefaultView : public IView
{
public:
    void Init() final override;
    void Update(float dt) final override;
    void Draw(sf::RenderWindow& window) final override;
    void Show() final override;
    void Hide() final override;
private:

};
