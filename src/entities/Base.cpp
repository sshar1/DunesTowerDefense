//
// Created by Sami Sharif  on 1/2/26.
//

#include "entities/Base.hpp"

Base::Base(glm::vec2 position)
    : sprite(spriteFilePath, SpriteType::Base, position, spriteSize)
    , health(MAX_HEALTH)
{
}

Sprite Base::getSprite() const {
    return sprite;
}
