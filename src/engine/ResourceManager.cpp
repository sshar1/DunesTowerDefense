//
// Created by Sami Sharif  on 12/30/25.
//

#include "../../include/engine/ResourceManager.hpp"

#include <iostream>
#include <ostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

GLuint ResourceManager::loadTexture(const char* filePath) {
    GLuint textureID;

    auto searchResult = textureCache.find(filePath);

    if (searchResult == textureCache.end()) {
        SDL_Surface* surface = IMG_Load(filePath);

        if (surface == nullptr) {
            std::cout << "Could not load image file: " << filePath << std::endl;
            return 0;
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels); // RBGA USED BECAUSE OF PNG
        glBindTexture(GL_TEXTURE_2D, 0);
        SDL_FreeSurface(surface);

        textureCache[filePath] = textureID;

    } else {
        textureID = searchResult->second;
    }

    return textureID;
}
