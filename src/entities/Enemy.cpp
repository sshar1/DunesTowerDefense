//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/Enemy.hpp"

#include <iostream>
#include <ostream>

#include "glm/gtc/epsilon.hpp"

Enemy::Enemy(const char* filePath, int health, SpriteType type)
    : sprite(filePath, type)
    , health(health)
    , state(State::WALKING)
{
    setState(State::WALKING);
}

Enemy::Enemy(const char* filePath, int health, SpriteType type, glm::vec2 pos, glm::vec2 size, glm::vec2 targetPosition)
    : sprite(filePath, type, pos, size)
    , health(health)
    , state(State::WALKING)
    , targetPosition(targetPosition)
{
    setState(State::WALKING);
}

void Enemy::update(const TopographyVertices& topVertices, float dt) {
    sprite.update(dt);

    switch (state) {
        case State::WALKING: {
            if (waypoints.empty()) {
                calculateWaypoints(topVertices);
            }

            followPath(topVertices, dt);

            if (currentWaypointIdx >= waypoints.size() - 1) {
                setState(State::ATTACKING);
            }

            break;
        }
        case State::ATTACKING:
            // Continue attacking
            break;
        case State::DYING:
            // Finish dying then switch to dead
            break;
    }
}

void Enemy::followPath(const TopographyVertices& topVertices, float dt) {
    glm::vec2 currentTarget = waypoints[currentWaypointIdx + 1];

    auto atTarget = glm::epsilonEqual(sprite.getPosition(), targetPosition, glm::epsilon<float>());
    if (glm::all(atTarget)) return;
    if (currentWaypointIdx >= waypoints.size() - 1) return;

    glm::vec2 directionVector = glm::normalize(currentTarget - sprite.getPosition());
    sprite.setLookVector(directionVector);

    float distanceToTarget = glm::distance(sprite.getPosition(), currentTarget);
    glm::vec2 approxFinalPosition = sprite.getPosition() + directionVector * getSpeed() * dt;
    float speed = getSpeedOverPoints(topVertices, sprite.getPosition(), approxFinalPosition);
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

float Enemy::getSpeedOverPoints(const TopographyVertices& topVertices, glm::vec2 from, glm::vec2 to) {
    return getSpeed();
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
