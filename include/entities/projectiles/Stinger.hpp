//
// Created by Sami Sharif  on 1/9/26.
//

#pragma once
#include "Projectile.hpp"

class Stinger : public Projectile {
public:
    explicit Stinger(glm::vec2 pos, ITargetable* target);

private:
    static constexpr const char* spriteFilePath = "data/textures/beeprojectile_sprite.png";
    static inline const glm::vec2 spriteSize = {60, 27};

    static constexpr float SPEED = 400.f;
    static constexpr int DAMAGE = 1;

    // int getDamage() const override { return DAMAGE; }
    void attack(const std::vector<std::unique_ptr<Enemy>>& enemies) override;
    float getSpeed() const override;
    void updateAnimation() override;
    void updateSize() override;
};
