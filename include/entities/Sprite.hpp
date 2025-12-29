//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once

#include <string_view>

#include "glad.h"
#include "glm/detail/type_vec2.hpp"

#include <vector>

struct SpriteVertex {
    float x;
    float y;
    float u;
    float v;
};
/* These MUST be in the form top left, top right, bottom right, bottom left */
// using SpriteVertices = std::array<SpriteVertex, 4>;
struct SpriteVertices {
    SpriteVertex topLeft;
    SpriteVertex topRight;
    SpriteVertex bottomLeft;
    SpriteVertex bottomRight;
};

class Sprite {
public:
    Sprite(const char* filePath);

    GLuint texture;

    const SpriteVertices getVertices() const;

private:
    glm::vec2 position;
    glm::vec2 size;
};
