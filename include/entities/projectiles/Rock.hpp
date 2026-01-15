//
// Created by Sami Sharif  on 1/14/26.
//

#pragma once
#include "Projectile.hpp"

class Rock : public Projectile {
public:
    explicit Rock(glm::vec2 pos, ITargetable* target);

private:
    static constexpr const char* spriteFilePath = "data/textures/rock_spritesheet.png";
    static constexpr glm::vec2 spriteSize = {150, 120};

    static constexpr float SPEED = 200.f;
    static constexpr int DAMAGE = 20;
    static constexpr float SPLASH_RANGE = 50.f;

    // int getDamage() const override { return DAMAGE; }
    void attack(const std::vector<std::unique_ptr<Enemy>>& enemies) override;
    float getSpeed() const override;
    void updateAnimation() override;
    void updateSize() override;
    void followPath(float dt) override;
};
