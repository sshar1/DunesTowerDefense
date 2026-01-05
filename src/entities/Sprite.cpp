//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/Sprite.hpp"
#include "engine/ResourceManager.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"
#include <glm/gtx/vector_angle.hpp>

float lookVectorToOrientation(const glm::vec2& lookVector) {
    if (lookVector.x < 0) {
        return glm::angle(lookVector, glm::vec2(-1, 0));
    }

    return glm::angle(lookVector, glm::vec2(1, 0));
}

Sprite::Sprite(const char* filePath, SpriteType type)
    : currentAnimateFrame(0)
    , elapsedAnimateTime(0)
    , animType(0)
    , uvRect(0, 0, 1, 1)
    , lookVector(1, 0)
    , spriteType(type)
{
    position = {0, 0};
    size = {0.3, 0.3};
    textureID = ResourceManager::getInstance().loadTexture(filePath);
}

Sprite::Sprite(const char* filePath, SpriteType type, glm::vec2 pos, glm::vec2 size)
    : currentAnimateFrame(0)
    , elapsedAnimateTime(0)
    , animType(0)
    , uvRect(0, 0, 1, 1)
    , position(pos)
    , size(size)
    , lookVector(1, 0)
    , spriteType(type)
{
    textureID = ResourceManager::getInstance().loadTexture(filePath);
}

void Sprite::update(float dt) {
    elapsedAnimateTime += dt;

    if (elapsedAnimateTime >= TIME_BETWEEN_ANIMATE_FRAMES) {
        currentAnimateFrame++;
        currentAnimateFrame %= ANIM_FRAMES;

        elapsedAnimateTime = 0;
    }

    if (isAnimatable(spriteType)) {
        updateAnimation();
    }
}

void Sprite::setAnimType(int newAnimType) {
    if (animType != newAnimType) {
        animType = newAnimType;
        currentAnimateFrame = 0;
    }
}

SpriteType Sprite::getType() const {
    return spriteType;
}

glm::vec2 Sprite::getPosition() const {
    return position;
}

void Sprite::setPosition(glm::vec2 position) {
    this->position = position;
}

void Sprite::setLookVector(glm::vec2 lookVector) {
    this->lookVector = lookVector;
}

void Sprite::updateAnimation() {
    static constexpr float spriteWidth = 0.243;
    static constexpr float spriteHeight = 0.33;

    uvRect.u = spriteWidth * currentAnimateFrame;
    uvRect.v = spriteHeight * animType;
    uvRect.w = spriteWidth;
    uvRect.h = spriteHeight;
}

const SpriteVertices Sprite::getVertices() const {

    const float halfW = size.x / 2.0f;
    const float halfH = size.y / 2.0f;

    // Transformation matrices are in reverse order. First, we rotate the
    // vectors. Then, we bring them to their position
    const float angle = -lookVectorToOrientation(lookVector);
    glm::mat3 rotationMat = glm::mat3(1.f);
    rotationMat = glm::translate(rotationMat, position);
    if (lookVector.x < 0) {
        rotationMat = glm::rotate(rotationMat, -angle);
        rotationMat = glm::scale(rotationMat, glm::vec2(-1, 1));
    }
    else {
        rotationMat = glm::rotate(rotationMat, angle);
    }

    glm::vec2 corners[4] = {
        {-halfW, halfH},
        {halfW, halfH},
        {-halfW, -halfH},
        {halfW, -halfH}
    };

    SpriteVertices vertices;
    auto applyTransformation = [&rotationMat](const glm::vec2& vertex) {
        return glm::vec2(rotationMat * glm::vec3(vertex, 1.f));
    };

    vertices.topLeft.position = applyTransformation(corners[0]);
    vertices.topRight.position = applyTransformation(corners[1]);
    vertices.bottomLeft.position = applyTransformation(corners[2]);
    vertices.bottomRight.position = applyTransformation(corners[3]);

    vertices.topLeft.u      = uvRect.u;
    vertices.topLeft.v      = uvRect.v;
    vertices.topRight.u     = uvRect.u + uvRect.w;
    vertices.topRight.v     = uvRect.v;
    vertices.bottomLeft.u   = uvRect.u;
    vertices.bottomLeft.v   = uvRect.v + uvRect.h;
    vertices.bottomRight.u  = uvRect.u + uvRect.w;
    vertices.bottomRight.v  = uvRect.v + uvRect.h;

    return vertices;
}

void Sprite::pushVertices(std::vector<SpriteVertex> &vertexBuffer) const {
    SpriteVertices vertices = getVertices();

    vertexBuffer.push_back(vertices.topLeft);
    vertexBuffer.push_back(vertices.topRight);
    vertexBuffer.push_back(vertices.bottomLeft);
    vertexBuffer.push_back(vertices.bottomRight);
}
