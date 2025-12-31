//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/Enemy.hpp"

#include <iostream>
#include <ostream>

Enemy::Enemy(const char* filePath, int health)
    : sprite(filePath)
    , state(State::WALKING)
    , health(health)
{
    // setState(State::DYING);
}

void Enemy::update(float dt) {
    // updateAnimation();

    sprite.update(dt);
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
