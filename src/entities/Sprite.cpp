//
// Created by Sami Sharif  on 12/26/25.
//

#include "../../include/entities/Sprite.hpp"

#include <iostream>
#include <ostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

Sprite::Sprite(const char* filePath) {
    SDL_Surface *surface = IMG_Load(filePath);

    position = {0, 0};
    size = {0.1, 0.1};

    // TODO create texture as test
    if (surface == nullptr) {
        std::cout << "Could not intialize texture" << std::endl;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels); // RBGA USED BECAUSE OF PNG
    glBindTexture(GL_TEXTURE_2D, 0);
    SDL_FreeSurface(surface);
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
