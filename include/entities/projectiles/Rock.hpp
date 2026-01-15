//
// Created by Sami Sharif  on 1/14/26.
//

#pragma once
#include "Projectile.hpp"

class Rock : public Projectile {
public:
    explicit Rock(glm::vec2 pos, glm::vec2 targetPosition);

private:
    static constexpr const char* spriteFilePath = "data/textures/rock_spritesheet.png";
    static constexpr glm::vec2 spriteSize = {150, 120};

    static constexpr float SPEED = 200.f;

    float getSpeed() const override;
    void updateAnimation() override;
    void updateSize() override;
    void followPath(float dt) override;
};
