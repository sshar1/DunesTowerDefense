//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/Enemy.hpp"

#include <iostream>
#include <ostream>

#include "MainGame.hpp"
#include "engine/VisionManager.hpp"
#include "glm/gtc/epsilon.hpp"

Enemy::Enemy(const char* filePath, int health, SpriteType type)
    : sprite(filePath, type)
    , health(health)
    , state(State::WALKING)
{
    setState(State::WALKING);
}

Enemy::Enemy(const char* filePath, int health, SpriteType type, glm::vec2 pos, glm::vec2 size, Base* base)
    : sprite(filePath, type, pos, size)
    , health(health)
    , state(State::WALKING)
    , base(base)
{
    setState(State::WALKING);
}

void Enemy::update(const TopographyVertices& topVertices, std::vector<std::unique_ptr<Projectile>>& projectiles, float dt) {
    if (state == State::DEAD) return;
    sprite.update(dt);

    if (health <= 0 && state != State::DYING && state != State::DEAD) {
        setState(State::DYING);
    }

    switch (state) {
        case State::WALKING: {
            calculateWaypoints(topVertices);
            followPath(dt);
            if (currentWaypointIdx >= waypoints.size() - 1 || validStartAttackPos()) {
                setState(State::ATTACKING);
            }
            break;
        }
        case State::ATTACKING:
            sprite.setLookVector(glm::normalize(base->getPosition() - sprite.getPosition()));

            if (!validAttackPosition(topVertices)) {
                setState(State::WALKING);
                break;
            }

            elapsedAttackTime += dt;
            if (elapsedAttackTime >= getAttackCooldown()) {
                sprite.playAnimation(false);
                elapsedAttackTime = 0;
                attack(projectiles);
            }

            break;
        case State::DYING:
            if (sprite.animDone()) {
                setState(State::DEAD);
            }
            break;
        case State::DEAD:
            return;
    }
}

void Enemy::followPath(float dt) {
    glm::vec2 currentTarget = waypoints[currentWaypointIdx + 1];
    glm::vec2 finalTarget = waypoints[waypoints.size() - 1];

    /*auto atTarget = glm::epsilonEqual(sprite.getPosition(), finalTarget, glm::epsilon<float>());
    if (glm::all(atTarget)) return;*/
    //if (glm::distance(sprite.getPosition(), finalTarget) <= getPositionThresh()) return;
    
    if (currentWaypointIdx >= waypoints.size() - 1) return;
    if (validStartAttackPos()) return;

    glm::vec2 directionVector = glm::normalize(currentTarget - sprite.getPosition());
    sprite.setLookVector(directionVector);

    float distanceToTarget = glm::distance(sprite.getPosition(), currentTarget);
    float speed = getDirectionalSpeed(sprite.getPosition(), directionVector);
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

bool Enemy::validStartAttackPos() {
    glm::vec2 finalTarget = waypoints[waypoints.size() - 1];

    return glm::distance(sprite.getPosition(), finalTarget) <= getValidStartAttackThresh();
}

float Enemy::getDirectionalSpeed(glm::vec2 from, glm::vec2 direction) {
    // To get the directional speed, we sample the destination point to be
    // 5% ahead
    static constexpr float range = 0.05f;
    static constexpr int sampleFinalPoints = 5;
    static constexpr int sampleInitialPoints = 9;

    // Takes a point in [0,1] x [0,1] coord system and returns the depth value
    // at that point
    static auto getSafeDepth = [&](glm::vec2 p) -> int {
        const cv::Mat& warpedDepth = Vision::Manager::getInstance().getWarpedDepth();

        int xIdx = p.x * DataLoader::DEPTH_WIDTH;
        int yIdx = p.y * DataLoader::DEPTH_HEIGHT;

        xIdx = std::clamp(xIdx, 0, DataLoader::DEPTH_WIDTH - 1);
        yIdx = std::clamp(yIdx, 0, DataLoader::DEPTH_HEIGHT - 1);

        return warpedDepth.at<uint16_t>(yIdx, xIdx);
    };

    glm::vec2 fromNorm = glm::vec2{from.x / MainGame::WINDOW_WIDTH, from.y / MainGame::WINDOW_HEIGHT};

    int initialDepthAvg = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            float w = i * 0.01f;
            float h = j * 0.01f;
            glm::vec2 point = glm::vec2(w, h) + fromNorm;
            initialDepthAvg += getSafeDepth(point);
        }
    }
    initialDepthAvg /= sampleInitialPoints;

    int finalDepthAvg = 0;
    for (int i = 1; i <= sampleFinalPoints; i++) {
        float vectorMultiplier = range * (float(i) / sampleFinalPoints);
        glm::vec2 destPoint = fromNorm + direction * vectorMultiplier;

        finalDepthAvg += getSafeDepth(destPoint);
    }
    finalDepthAvg /= sampleFinalPoints;

    float speedMultiplier = ((finalDepthAvg - initialDepthAvg) / speedDampCoefficient) + 1;
    speedMultiplier = std::clamp(speedMultiplier, 0.2f, 1.5f);

    return speedMultiplier * getSpeed();
}

void Enemy::updateAnimation() {
    switch (state) {
        case State::WALKING:
            sprite.setAnimType(WALK_ANIM_TYPE);
            sprite.playAnimation(true);
            return;
        case State::ATTACKING:
            sprite.setAnimType(ATTACKING_ANIM_TYPE);
            sprite.playAnimation(false);
            return;
        case State::DYING:
            sprite.setAnimType(DYING_ANIM_TYPE);
            sprite.playAnimation(false);
        case State::DEAD:
            return;
    }
}

void Enemy::setState(State newState) {
    if (state != newState) {
        state = newState;
        updateAnimation();

        if (state == State::DEAD) {
            if (onDeath) {
                onDeath();
                onDeath = nullptr;
            }
        }
    }
}

Sprite Enemy::getSprite() const {
    return sprite;
}