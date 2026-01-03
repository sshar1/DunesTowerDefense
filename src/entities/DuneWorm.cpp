//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/DuneWorm.hpp"

DuneWorm::DuneWorm()
    : Enemy(spriteFilePath, MAX_HEALTH, SpriteType::Worm)
{
}

DuneWorm::DuneWorm(glm::vec2 pos)
    : Enemy(spriteFilePath, MAX_HEALTH, SpriteType::Worm, pos, spriteSize)
{
}

int DuneWorm::getMaxHealth() const {
    return MAX_HEALTH;
}
