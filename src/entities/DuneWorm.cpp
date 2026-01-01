//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/DuneWorm.hpp"

DuneWorm::DuneWorm()
    : Enemy(spriteFilePath, MAX_HEALTH)
{
}

int DuneWorm::getMaxHealth() const {
    return MAX_HEALTH;
}
