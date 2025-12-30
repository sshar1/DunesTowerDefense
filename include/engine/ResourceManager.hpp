//
// Created by Sami Sharif  on 12/30/25.
//

#pragma once
#include <string_view>
#include <unordered_map>

#include "glad.h"

class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(const ResourceManager&&) = delete;
    ResourceManager& operator=(const ResourceManager&&) = delete;

    GLuint loadTexture(const char* filePath);

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    std::unordered_map<std::string_view, GLuint> textureCache{};
};

