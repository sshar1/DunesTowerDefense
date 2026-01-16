//
// Created by Sami Sharif  on 1/9/26.
//

#include "../../../include/entities/projectiles/Stinger.hpp"

Stinger::Stinger(glm::vec2 pos, ITargetable* target)
    : Projectile(spriteFilePath, SpriteType::Stinger, pos, spriteSize, target)
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

void Stinger::attack(const std::vector<std::unique_ptr<Enemy> > &enemies) {
    target->takeDamage(DAMAGE);
}
