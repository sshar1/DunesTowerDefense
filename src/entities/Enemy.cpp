//
// Created by Sami Sharif  on 12/26/25.
//

#include "../../include/entities/Enemy.hpp"

Enemy::Enemy(const char* filePath)
    : sprite(filePath)
{

}

Sprite Enemy::getSprite() {
    return sprite;
}
