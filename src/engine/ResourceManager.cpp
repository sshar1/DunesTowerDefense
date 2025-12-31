//
// Created by Sami Sharif  on 12/30/25.
//

#include "engine/ResourceManager.hpp"

#include <iostream>
#include <ostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

GLuint ResourceManager::loadTexture(const char* filePath) {
    GLuint textureID;

    auto searchResult = textureCache.find(filePath);

    if (searchResult == textureCache.end()) {
        SDL_Surface* loadedSurface = IMG_Load(filePath);

        if (loadedSurface == nullptr) {
            std::cout << "Could not load image file: " << filePath << std::endl;
            return 0;
        }

        SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(loadedSurface);

        if (formattedSurface == nullptr) {
            std::cout << "Could not convert surface: " << SDL_GetError() << std::endl;
            return 0;
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formattedSurface->w, formattedSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, formattedSurface->pixels); // RBGA USED BECAUSE OF PNG
        glBindTexture(GL_TEXTURE_2D, 0);
        SDL_FreeSurface(formattedSurface);

        textureCache[filePath] = textureID;

    } else {
        textureID = searchResult->second;
    }

    return textureID;
}
