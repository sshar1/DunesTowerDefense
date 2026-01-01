//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/DungBeetle.hpp"

DungBeetle::DungBeetle()
    : Enemy(spriteFilePath, MAX_HEALTH, SpriteType::Beetle)
{
}

DungBeetle::DungBeetle(glm::vec2 pos)
    : Enemy(spriteFilePath, MAX_HEALTH, SpriteType::Beetle, pos)
{
}

int DungBeetle::getMaxHealth() const {
    return MAX_HEALTH;
}
