//
// Created by Sami Sharif  on 1/10/26.
//

#include "entities/Sprayer.hpp"

#include <iostream>
#include <ostream>

#include "glm/detail/type_vec2.hpp"

Sprayer::Sprayer(glm::vec2 position)
    : Tower(spriteFilePath, SpriteType::Gas, position, spriteSize)
{
}

void Sprayer::attack() {
    attackSprite.playAnimation(true);
}
