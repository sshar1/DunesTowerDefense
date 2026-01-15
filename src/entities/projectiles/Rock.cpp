//
// Created by Sami Sharif  on 1/14/26.
//

#include "../../../include/entities/projectiles/Rock.hpp"

#include "entities/Base.hpp"
#include "glm/detail/func_geometric.hpp"
#include "glm/gtc/constants.hpp"

Rock::Rock(glm::vec2 pos, glm::vec2 targetPosition)
    : Projectile(spriteFilePath, SpriteType::Rock, pos, spriteSize, targetPosition)
{
}

float Rock::getSpeed() const {
    return SPEED;
}

void Rock::updateAnimation()
{
}

void Rock::updateSize() {
    float dist = glm::distance(originPosition, targetPosition);
    float traveled = glm::distance(sprite.getPosition(), targetPosition);

    float progress = traveled / dist;
    float multiplier = cos(glm::pi<float>() * (progress - 0.5));

    sprite.setSize(spriteSize * multiplier);
}

void Rock::followPath(float dt) {
    Projectile::followPath(dt);
}
