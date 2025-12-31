//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once

#include "Enemy.hpp"
#include "glm/detail/type_vec2.hpp"

class DungBeetle : public Enemy {
public:
    DungBeetle();

private:
    static constexpr const char* spriteFilePath = "data/textures/dungbeetle_spritesheet.png";

    static constexpr int SPEED = 10;
    static constexpr int MAX_HEALTH = 100;

    int getMaxHealth() const override;
};
