//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once

#include "Enemy.hpp"
#include "glm/detail/type_vec2.hpp"

class DungBeetle : Enemy {
public:
    DungBeetle();

private:
    static constexpr int SPEED = 10;
    static constexpr int HEALTH = 100;
};
