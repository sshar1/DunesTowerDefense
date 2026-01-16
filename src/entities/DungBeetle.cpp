//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/DungBeetle.hpp"

#include "glm/gtx/norm.hpp"

DungBeetle::DungBeetle()
    : Enemy(spriteFilePath, MAX_HEALTH, SpriteType::Beetle)
{
}

DungBeetle::DungBeetle(glm::vec2 pos, Base* base)
    : Enemy(spriteFilePath, MAX_HEALTH, SpriteType::Beetle, pos, spriteSize, base)
{
}

int DungBeetle::getMaxHealth() const {
    return MAX_HEALTH;
}

float DungBeetle::getSpeed() const {
    return SPEED;
}

void DungBeetle::calculateWaypoints(const TopographyVertices& topVertices) {
    static constexpr float baseRadius = 130.f;

    waypoints.clear();
    currentWaypointIdx = 0;

    glm::vec2 origin = getSprite().getPosition();

    glm::vec2 directionVector = glm::normalize(base->getPosition() - origin);
    glm::vec2 adjustedTarget = base->getPosition() - directionVector * baseRadius;

    waypoints.push_back(origin);
    waypoints.push_back(adjustedTarget);
}

bool DungBeetle::validAttackPosition(const TopographyVertices &topVertices) {
    static constexpr float maxDistSquared = 40000.f;

    return glm::distance2(getSprite().getPosition(), base->getPosition()) < maxDistSquared;
}

float DungBeetle::getAttackCooldown() const {
    return ATTACK_COOLDOWN;
}

void DungBeetle::attack(std::vector<std::unique_ptr<Projectile> > &projectiles) {
    base->takeDamage(DAMAGE);
}
