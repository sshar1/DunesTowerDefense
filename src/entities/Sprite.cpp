//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/Sprite.hpp"
#include "engine/ResourceManager.hpp"

Sprite::Sprite(const char* filePath, SpriteType type)
    : currentAnimateFrame(0)
    , elapsedAnimateTime(0)
    , animType(0)
    , uvRect(0, 0, 1, 1)
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

    SpriteVertices vertices;
    vertices.topLeft.x      = position.x - halfW;
    vertices.topLeft.y      = position.y + halfH;
    vertices.topRight.x     = position.x + halfW;
    vertices.topRight.y     = position.y + halfH;
    vertices.bottomLeft.x   = position.x - halfW;
    vertices.bottomLeft.y   = position.y - halfH;
    vertices.bottomRight.x  = position.x + halfW;
    vertices.bottomRight.y  = position.y - halfH;

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

    // vertexBuffer.insert(vertexBuffer.end(), vertices.begin(), vertices.end());
}
