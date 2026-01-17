//
// Created by Sami Sharif  on 1/14/26.
//

#include "../../../include/entities/projectiles/Rock.hpp"

#include <iostream>
#include <ostream>

#include "entities/Base.hpp"
#include "entities/Enemy.hpp"
#include "glm/detail/func_geometric.hpp"
#include "glm/gtc/constants.hpp"

Rock::Rock(glm::vec2 pos, ITargetable* target)
    : Projectile(spriteFilePath, SpriteType::Rock, pos, spriteSize, target)
{
}

float Rock::getSpeed() const {
    return SPEED;
}

void Rock::updateAnimation()
{
}

void Rock::updateSize() {
    float dist = glm::distance(originPosition, lastKnownPosition);
    float traveled = glm::distance(sprite.getPosition(), lastKnownPosition);

    float progress = traveled / dist;
    float multiplier = cos(glm::pi<float>() * (progress - 0.5));

    sprite.setSize(spriteSize * multiplier);
}

void Rock::followPath(float dt) {
    Projectile::followPath(dt);
}

void Rock::attack(const std::vector<std::unique_ptr<Enemy> > &enemies) {
    for (const auto& enemy : enemies) {
        if (!enemy->isActive()) continue;
        if (glm::distance(enemy->getSprite().getPosition(), sprite.getPosition()) <= SPLASH_RANGE) {
            enemy->takeDamage(DAMAGE);
        }
    }
}
