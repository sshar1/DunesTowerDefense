//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/DuneWorm.hpp"

DuneWorm::DuneWorm()
    : Enemy(spriteFilePath, MAX_HEALTH, SpriteType::Worm)
{
}

DuneWorm::DuneWorm(glm::vec2 pos, glm::vec2 targetPosition)
    : Enemy(spriteFilePath, MAX_HEALTH, SpriteType::Worm, pos, spriteSize, targetPosition)
{
}

int DuneWorm::getMaxHealth() const {
    return MAX_HEALTH;
}

float DuneWorm::getSpeed() const {
    return SPEED;
}

void DuneWorm::calculateWaypoints() {
    static constexpr float baseRadius = 0.2f;

    waypoints.clear();
    currentWaypointIdx = 0;

    glm::vec2 origin = getSprite().getPosition();

    glm::vec2 directionVector = targetPosition - origin;
    glm::vec2 adjustedTarget = targetPosition - directionVector * baseRadius;

    waypoints.push_back(origin);
    waypoints.push_back(adjustedTarget);
}