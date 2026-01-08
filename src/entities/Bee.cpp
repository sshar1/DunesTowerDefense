//
// Created by Sami Sharif  on 1/7/26.
//

#include "entities/Bee.hpp"

#include "engine/VisionManager.hpp"
#include "glm/detail/func_geometric.inl"
#include "glm/gtx/norm.inl"

Bee::Bee()
    : Enemy(spriteFilePath, MAX_HEALTH, SpriteType::Bee)
{
}

Bee::Bee(glm::vec2 pos, glm::vec2 targetPosition)
    : Enemy(spriteFilePath, MAX_HEALTH, SpriteType::Bee, pos, spriteSize, targetPosition)
{
}

int Bee::getMaxHealth() const {
    return MAX_HEALTH;
}

float Bee::getSpeed() const {
    return SPEED;
}

void Bee::calculateWaypoints(const TopographyVertices& topVertices) {
    static constexpr float baseRadius = 0.2f;

    waypoints.clear();
    currentWaypointIdx = 0;

    glm::vec2 origin = getSprite().getPosition();
    glm::vec2 targetPosition;

    std::vector<glm::vec2>& hills = Vision::Manager::getInstance().getHills();

    // There will only be a couple hills, so don't need to do anything fancy
    glm::vec2* closestHill = nullptr;
    auto closestIt = std::min_element(hills.begin(), hills.end(), [&](const glm::vec2& a, const glm::vec2& b) {
        return glm::distance2(a, origin) < glm::distance2(b, origin);
    });
    if (closestIt != hills.end()) {
        closestHill = &(*closestIt);
    }

    if (closestHill != nullptr) {
        targetPosition = *closestHill;
    }
    else {
        glm::vec2 directionVector = glm::normalize(targetPosition - origin);
        targetPosition = basePosition - directionVector * baseRadius;
    }

    waypoints.push_back(origin);
    waypoints.push_back(targetPosition);
}

bool Bee::validAttackPosition(const TopographyVertices &topVertices) {
    static constexpr float epsilon = 0.01;

    std::vector<glm::vec2>& hills = Vision::Manager::getInstance().getHills();

    return std::any_of(hills.begin(), hills.end(), [&](const glm::vec2& hill) {
        return glm::distance2(hill, sprite.getPosition()) < epsilon;
    });
}