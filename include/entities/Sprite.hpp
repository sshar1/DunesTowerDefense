//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once

#include <string_view>

#include "glad.h"
#include "glm/detail/type_vec2.hpp"

#include <vector>

// #include "Enemy.hpp"

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
    void update(float dt);
    void setAnimType(int animType);

    // friend void Enemy::updateAnimation();

private:
    constexpr static float TIME_BETWEEN_ANIMATE_FRAMES = 0.1; // in s
    constexpr static int ANIM_FRAMES = 4;

    int currentAnimateFrame;
    float elapsedAnimateTime;
    int animType;

    glm::vec2 position;
    glm::vec2 size;
};
