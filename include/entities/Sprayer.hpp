//
// Created by Sami Sharif  on 1/10/26.
//

#pragma once
#include "Tower.hpp"
#include "glm/detail/type_vec.hpp"

class Sprayer : public Tower {
public:
    explicit Sprayer(glm::vec2 position);
private:
    static constexpr const char* spriteFilePath = "data/textures/gas_spritesheet.png";
    static constexpr glm::vec2 spriteSize = {150, 120};

    static constexpr float ATTACK_COOLDOWN = 2.f;

    float getAttackCooldown() const override { return ATTACK_COOLDOWN; }
    void attack() const override;
};
