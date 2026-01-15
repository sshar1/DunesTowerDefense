//
// Created by Sami Sharif  on 1/9/26.
//

#include "../../../include/entities/projectiles/Stinger.hpp"

Stinger::Stinger(glm::vec2 pos, glm::vec2 targetPosition)
    : Projectile(spriteFilePath, SpriteType::Stinger, pos, spriteSize, targetPosition)
{
}

float Stinger::getSpeed() const {
    return SPEED;
}

void Stinger::updateAnimation()
{
}

void Stinger::updateSize()
{
    sprite.setSize(spriteSize);
}
