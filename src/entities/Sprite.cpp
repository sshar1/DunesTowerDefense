//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/Sprite.hpp"
#include "engine/ResourceManager.hpp"
#include <glm/gtx/vector_angle.hpp>

#include "MainGame.hpp"
#include "glm/gtx/compatibility.hpp"
#include <numbers>

float lookVectorToOrientation(const glm::vec2& lookVector) {
    return glm::atan(lookVector.y, std::abs(lookVector.x));
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
    , loopAnimation(true)
    , vertFlip(false)
    , position(pos)
    , size(size)
    , lookVector(1, 0)
    , spriteType(type)
{
    textureID = ResourceManager::getInstance().loadTexture(filePath);

    if (isAnimatable(spriteType)) {
        updateUVRect();
    }
}

void Sprite::playAnimation(bool loop) {
    loopAnimation = loop;
    currentAnimateFrame = 0;
    elapsedAnimateTime = 0;
}

void Sprite::update(float dt) {
    if (!isAnimatable(spriteType)) return;

    elapsedAnimateTime += dt;

    if (elapsedAnimateTime >= TIME_BETWEEN_ANIMATE_FRAMES) {
        if (currentAnimateFrame < ANIM_FRAMES - 1) {
            currentAnimateFrame++;
        }
        else if (loopAnimation) {
            currentAnimateFrame = 0;
        }
        elapsedAnimateTime = 0;
    }
    updateUVRect();
}

void Sprite::resetAnimation() {
    currentAnimateFrame = 0;
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
    this->lookVector = glm::normalize(lookVector);
}

glm::vec2 Sprite::getLookVector() const {
    return lookVector;
}

void Sprite::updateUVRect() {
    static constexpr float spriteWidth = 0.243;
    static constexpr float spriteHeight = 0.33;

    uvRect.u = spriteWidth * currentAnimateFrame;
    uvRect.v = spriteHeight * animType;
    uvRect.w = spriteWidth;
    uvRect.h = spriteHeight;
}

const SpriteVertices Sprite::getVertices() const {
    static const glm::mat4 projectionMat = glm::ortho(0.0f, (float)MainGame::WINDOW_WIDTH,
                                         (float)MainGame::WINDOW_HEIGHT, 0.0f, -100.0f, 100.0f);

    const float halfW = size.x / 2.0f;
    const float halfH = size.y / 2.0f;

    // Transformation matrices are in reverse order. First, we rotate the
    // vectors. Then, we bring them to their position
    const float angle = lookVectorToOrientation(lookVector);
    glm::mat4 rotationMat = glm::mat4(1.f);
    rotationMat = glm::translate(rotationMat, glm::vec3(position, 0));
    if (lookVector.x < 0) {
        rotationMat = glm::rotate(rotationMat, -angle, glm::vec3(0, 0, 1));
        rotationMat = glm::scale(rotationMat, glm::vec3(-1, 1, 1));
    }
    else {
        rotationMat = glm::rotate(rotationMat, angle, glm::vec3(0, 0, 1));
    }

    glm::vec2 corners[4] = {
        {-halfW, -halfH},
        {halfW, -halfH},
        {-halfW, halfH},
        {halfW, halfH}
    };

    SpriteVertices vertices;
    auto applyTransformation = [&](const glm::vec2& vertex) {
        return glm::vec2(projectionMat * rotationMat * glm::vec4(vertex, 0, 1.f));
    };

    if (!vertFlip) {
        vertices.topLeft.position = applyTransformation(corners[0]);
        vertices.topRight.position = applyTransformation(corners[1]);
        vertices.bottomLeft.position = applyTransformation(corners[2]);
        vertices.bottomRight.position = applyTransformation(corners[3]);
    }
    else {
        vertices.topLeft.position = applyTransformation(corners[2]);
        vertices.topRight.position = applyTransformation(corners[3]);
        vertices.bottomLeft.position = applyTransformation(corners[0]);
        vertices.bottomRight.position = applyTransformation(corners[1]);
    }

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
