#pragma once
#include "glm/glm.hpp"

class IGun
{
public:
    IGun() = default;
    virtual ~IGun() = default;

    virtual void Shoot(glm::vec2 src, glm::vec2 dst) = 0;
};
