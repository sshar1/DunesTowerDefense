//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/Enemy.hpp"

#include <iostream>
#include <ostream>

#include "engine/VisionManager.hpp"
#include "glm/gtc/epsilon.hpp"

Enemy::Enemy(const char* filePath, int health, SpriteType type)
    : sprite(filePath, type)
    , health(health)
    , state(State::WALKING)
{
    setState(State::WALKING);
}

Enemy::Enemy(const char* filePath, int health, SpriteType type, glm::vec2 pos, glm::vec2 size, glm::vec2 basePosition)
    : sprite(filePath, type, pos, size)
    , health(health)
    , state(State::WALKING)
    , basePosition(basePosition)
{
    setState(State::WALKING);
}

void Enemy::update(const TopographyVertices& topVertices, float dt) {
    sprite.update(dt);

    switch (state) {
        case State::WALKING: {
            calculateWaypoints(topVertices);

            followPath(topVertices, dt);

            if (currentWaypointIdx >= waypoints.size() - 1) {
                setState(State::ATTACKING);
            }

            break;
        }
        case State::ATTACKING:
            sprite.setLookVector(glm::normalize(basePosition - sprite.getPosition()));

            if (!validAttackPosition(topVertices)) {
                setState(State::WALKING);
            }

            break;
        case State::DYING:
            // Finish dying then switch to dead
            break;
    }
}

void Enemy::followPath(const TopographyVertices& topVertices, float dt) {
    glm::vec2 currentTarget = waypoints[currentWaypointIdx + 1];
    glm::vec2 finalTarget = waypoints[waypoints.size() - 1];

    auto atTarget = glm::epsilonEqual(sprite.getPosition(), finalTarget, glm::epsilon<float>());
    if (glm::all(atTarget)) return;
    if (currentWaypointIdx >= waypoints.size() - 1) return;

    glm::vec2 directionVector = glm::normalize(currentTarget - sprite.getPosition());
    sprite.setLookVector(directionVector);

    float distanceToTarget = glm::distance(sprite.getPosition(), currentTarget);
    float speed = getDirectionalSpeed(topVertices, sprite.getPosition(), directionVector);
    float travelDistance = speed * dt;

    glm::vec2 finalPosition;
    if (travelDistance >= distanceToTarget) {
        finalPosition = currentTarget;
        currentWaypointIdx++;
    }
    else {
        finalPosition = sprite.getPosition() + directionVector * travelDistance;
    }

    sprite.setPosition(finalPosition);
}

float Enemy::getDirectionalSpeed(const TopographyVertices& topVertices, glm::vec2 from, glm::vec2 direction) {
    // To get the directional speed, we sample the destination point to be
    // 5% of screen dimensions
    static constexpr float range = 0.05f;
    static constexpr int sampleFinalPoints = 5;
    static constexpr int sampleInitialPoints = 9;

    // TODO find a better way for this
    static const glm::mat3 warpMat = Vision::calculateWarpMatrix();
    static const glm::mat3 unwarpMat = glm::inverse(warpMat);

    static auto getSafeDepth = [&](glm::vec2 p) -> int {
        // p is [-1, 1] so we must convert to [0, 1]
        float u = (p.x + 1.f) * 0.5f;
        float v = (1.f - p.y) * 0.5f;

        glm::vec3 unwarpedHomogenous = unwarpMat * glm::vec3(u, v, 1.f);
        if (unwarpedHomogenous.z == 0) return 0;
        glm::vec2 gridUV = glm::vec2(unwarpedHomogenous.x, unwarpedHomogenous.y) / unwarpedHomogenous.z;

        int c = std::clamp(static_cast<int>(DataLoader::DEPTH_WIDTH * gridUV.x), 0, DataLoader::DEPTH_WIDTH - 1);
        int r = std::clamp(static_cast<int>(DataLoader::DEPTH_HEIGHT * gridUV.y), 0, DataLoader::DEPTH_HEIGHT - 1);

        return topVertices[r * DataLoader::DEPTH_WIDTH + c];
    };

    // TODO this is kinda nasty but we go for it
    if (sprite.getType() == SpriteType::Bee) return getSpeed();

    int initialDepthAvg = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            float w = i * 0.01f;
            float h = j * 0.01f;
            glm::vec2 point = glm::vec2(w, h) + from;
            initialDepthAvg += getSafeDepth(point);
        }
    }
    initialDepthAvg /= sampleInitialPoints;

    int finalDepthAvg = 0;
    for (int i = 1; i <= sampleFinalPoints; i++) {
        float vectorMultiplier = range * (float(i) / sampleFinalPoints);
        glm::vec2 destPoint = from + direction * vectorMultiplier;

        finalDepthAvg += getSafeDepth(destPoint);
    }
    finalDepthAvg /= sampleFinalPoints;

    float speedMultiplier = (finalDepthAvg - initialDepthAvg) / 20.f + 1;
    speedMultiplier = std::clamp(speedMultiplier, 0.2f, 2.f);

    return speedMultiplier * getSpeed();
}

void Enemy::takeDamage(int damage) {
    health -= damage;

    if (health <= 0) {
        std::cout << "Enemy died!" << std::endl;
    }
}

void Enemy::updateAnimation() {
    switch (state) {
        case State::WALKING:
            sprite.setAnimType(WALK_ANIM_TYPE);
            return;
        case State::ATTACKING:
            sprite.setAnimType(ATTACKING_ANIM_TYPE);
            return;
        case State::DYING:
        case State::DEAD:
            sprite.setAnimType(DYING_ANIM_TYPE);
    }
}

void Enemy::setState(State newState) {
    if (state != newState) {
        state = newState;
        updateAnimation();
    }
}

Sprite Enemy::getSprite() {
    return sprite;
}
