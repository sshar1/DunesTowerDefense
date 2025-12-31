//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/Enemy.hpp"

#include <iostream>
#include <ostream>

Enemy::Enemy(const char* filePath)
    : sprite(filePath)
{
}

void Enemy::takeDamage(int damage) {
    health -= damage;

    if (health <= 0) {
        std::cout << "Enemy died!" << std::endl;
    }
}

Sprite Enemy::getSprite() {
    return sprite;
}
