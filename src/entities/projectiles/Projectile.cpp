//
// Created by Sami Sharif  on 1/9/26.
//

#include "../../../include/entities/projectiles/Projectile.hpp"

#include <iostream>
#include <ostream>

#include "glm/gtc/epsilon.hpp"

Projectile::Projectile(const char* filePath, SpriteType type, glm::vec2 pos, glm::vec2 size, glm::vec2 targetPosition)
    : sprite(filePath, type, pos, size)
    , targetPosition(targetPosition)
    , originPosition(pos)
{
    setState(State::Firing);
}

Sprite Projectile::getSprite() {
    return sprite;
}

void Projectile::update(const std::vector<std::unique_ptr<Enemy>>& enemies, float dt) {
    if (state == State::Landed) return;
    sprite.update(dt);

    switch (state) {
        case State::Firing:
            followPath(dt);
            return;
        case State::Landing:
            // I know this is dumb. We only do this because there is no longer
            // a landing animation, so we just go ahead and say the thing has
            // landed.
            // TODO do damage to surrounding enemies
            setState(State::Landed);
        case State::Landed:
            return;
    }
}

void Projectile::setState(State newState) {
    if (state != newState) {
        state = newState;
        updateAnimation();
    }
}

void Projectile::followPath(float dt) {
    auto atTarget = glm::epsilonEqual(sprite.getPosition(), targetPosition, glm::epsilon<float>());
    if (glm::all(atTarget)) {
        setState(State::Landing);
        return;
    }

    glm::vec2 directionVector = glm::normalize(targetPosition - sprite.getPosition());
    sprite.setLookVector(directionVector);

    float distanceToTarget = glm::distance(sprite.getPosition(), targetPosition);
    float travelDistance = getSpeed() * dt;

    glm::vec2 finalPosition;
    if (travelDistance >= distanceToTarget) {
        finalPosition = targetPosition;
    }
    else {
        finalPosition = sprite.getPosition() + directionVector * travelDistance;
    }

    sprite.setPosition(finalPosition);
    updateSize();
}