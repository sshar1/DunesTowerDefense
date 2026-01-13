//
// Created by Sami Sharif  on 1/2/26.
//

#pragma once
#include "Sprite.hpp"

class Base {
public:
    Base(glm::vec2 position);

    Sprite getSprite() const;

private:
    static constexpr const char* spriteFilePath = "data/textures/base_sprite.png";
    static inline const glm::vec2 spriteSize = {0.2*1200, 0.2*900};
    static constexpr int MAX_HEALTH = 100;

    Sprite sprite;
    int health;
};

