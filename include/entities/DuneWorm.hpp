//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once

#include "Enemy.hpp"
#include "glm/detail/type_vec2.hpp"

class DuneWorm : public Enemy {
public:
    DuneWorm();
    explicit DuneWorm(glm::vec2 pos, glm::vec2 targetPosition);

private:
    static constexpr const char* spriteFilePath = "data/textures/duneworm_spritesheet.png";
    static constexpr glm::vec2 spriteSize = {0.1, 0.1};

    static constexpr float SPEED = 0.1f;
    static constexpr int MAX_HEALTH = 100;

    int getMaxHealth() const override;
    float getSpeed() const override;
    void calculateWaypoints(const TopographyVertices& topVertices) override;
};
