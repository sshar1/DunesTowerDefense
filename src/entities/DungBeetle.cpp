//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/DungBeetle.hpp"

DungBeetle::DungBeetle()
    : Enemy(spriteFilePath, MAX_HEALTH)
{
}

int DungBeetle::getMaxHealth() const {
    return MAX_HEALTH;
}
