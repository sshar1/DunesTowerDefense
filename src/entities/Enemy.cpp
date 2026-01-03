//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/Enemy.hpp"

#include <iostream>
#include <ostream>

Enemy::Enemy(const char* filePath, int health, SpriteType type)
    : sprite(filePath, type)
    , health(health)
    , state(State::WALKING)
{
    setState(State::WALKING);
}

Enemy::Enemy(const char* filePath, int health, SpriteType type, glm::vec2 pos, glm::vec2 size)
    : sprite(filePath, type, pos, size)
    , health(health)
    , state(State::WALKING)
{
    setState(State::WALKING);
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
