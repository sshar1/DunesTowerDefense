//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once

#include "Enemy.hpp"
#include "glm/detail/type_vec2.hpp"

class DuneWorm : public Enemy {
public:
    DuneWorm();
    explicit DuneWorm(glm::vec2 pos);

private:
    static constexpr const char* spriteFilePath = "data/textures/duneworm_spritesheet.png";

    static constexpr int SPEED = 10;
    static constexpr int MAX_HEALTH = 100;

    int getMaxHealth() const override;
};
