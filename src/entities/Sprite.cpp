//
// Created by Sami Sharif  on 12/26/25.
//

#include "../../include/entities/Sprite.hpp"

#include <iostream>
#include <ostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

#include "engine/ResourceManager.hpp"

Sprite::Sprite(const char* filePath) {
    position = {0, 0};
    size = {0.1, 0.1};
    texture = ResourceManager::getInstance().loadTexture(filePath);
}

const SpriteVertices Sprite::getVertices() const {
    SpriteVertices vertices;
    vertices.topLeft.x = position.x - (size.x / 2.0f);
    vertices.topLeft.y = position.y + (size.y / 2.0f);
    vertices.topLeft.u = 0;
    vertices.topLeft.v = 0;

    vertices.topRight.x = position.x + (size.x / 2.0f);
    vertices.topRight.y = position.y + (size.y / 2.0f);
    vertices.topRight.u = 1;
    vertices.topRight.v = 0;

    vertices.bottomLeft.x = position.x - (size.x / 2.0f);
    vertices.bottomLeft.y = position.y - (size.y / 2.0f);
    vertices.bottomLeft.u = 0;
    vertices.bottomLeft.v = 1;

    vertices.bottomRight.x = position.x + (size.x / 2.0f);
    vertices.bottomRight.y = position.y - (size.y / 2.0f);
    vertices.bottomRight.u = 1;
    vertices.bottomRight.v = 1;

    return vertices;
}
