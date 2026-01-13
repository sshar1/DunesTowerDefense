//
// Created by Sami Sharif  on 1/9/26.
//

#pragma once
#include "Projectile.hpp"

class Stinger : public Projectile {
public:
    explicit Stinger(glm::vec2 pos, glm::vec2 targetPosition);

private:
    static constexpr const char* spriteFilePath = "data/textures/beeprojectile_sprite.png";
    static inline const glm::vec2 spriteSize = {60, 27};

    static constexpr float SPEED = 400.f;

    float getSpeed() const override;
    void updateAnimation() override;
};
