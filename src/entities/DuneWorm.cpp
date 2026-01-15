//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/DuneWorm.hpp"

#include "glm/gtx/norm.hpp"

DuneWorm::DuneWorm()
    : Enemy(spriteFilePath, MAX_HEALTH, SpriteType::Worm)
{
}

DuneWorm::DuneWorm(glm::vec2 pos, Base* base)
    : Enemy(spriteFilePath, MAX_HEALTH, SpriteType::Worm, pos, spriteSize, base)
{
}

int DuneWorm::getMaxHealth() const {
    return MAX_HEALTH;
}

float DuneWorm::getSpeed() const {
    return SPEED;
}

void DuneWorm::calculateWaypoints(const TopographyVertices& topVertices) {
    static constexpr float baseRadius = 130.f;

    waypoints.clear();
    currentWaypointIdx = 0;

    glm::vec2 origin = getSprite().getPosition();

    glm::vec2 directionVector = glm::normalize(base->getPosition() - origin);
    glm::vec2 adjustedTarget = base->getPosition() - directionVector * baseRadius;

    waypoints.push_back(origin);
    waypoints.push_back(adjustedTarget);
}

bool DuneWorm::validAttackPosition(const TopographyVertices &topVertices) {
    static constexpr float maxDistSquared = 40000.f;

    return glm::distance2(getSprite().getPosition(), base->getPosition()) < maxDistSquared;
}

float DuneWorm::getAttackCooldown() const {
    return ATTACK_COOLDOWN;
}

void DuneWorm::attack(std::vector<std::unique_ptr<Projectile> > &projectiles) {

}