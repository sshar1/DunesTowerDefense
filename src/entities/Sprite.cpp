//
// Created by Sami Sharif  on 12/26/25.
//

#include "entities/Sprite.hpp"
#include "engine/ResourceManager.hpp"

Sprite::Sprite(const char* filePath, SpriteType type)
    : currentAnimateFrame(0)
    , elapsedAnimateTime(0)
    , animType(0)
    , spriteType(type)
{
    position = {0, 0};
    size = {0.1, 0.1};
    textureID = ResourceManager::getInstance().loadTexture(filePath);
}

Sprite::Sprite(const char* filePath, SpriteType type, glm::vec2 pos)
    : currentAnimateFrame(0)
    , elapsedAnimateTime(0)
    , animType(0)
    , position(pos)
    , spriteType(type)
{
    size = {0.1, 0.1};
    textureID = ResourceManager::getInstance().loadTexture(filePath);
}

void Sprite::update(float dt) {
    elapsedAnimateTime += dt;

    if (elapsedAnimateTime >= TIME_BETWEEN_ANIMATE_FRAMES) {
        currentAnimateFrame++;
        currentAnimateFrame %= ANIM_FRAMES;

        elapsedAnimateTime = 0;
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

const SpriteVertices Sprite::getVertices() const {
    static constexpr float uMult = 0.243;
    static constexpr float vMult = 0.33;

    SpriteVertices vertices;
    vertices.topLeft.x = position.x - (size.x / 2.0f);
    vertices.topLeft.y = position.y + (size.y / 2.0f);
    vertices.topLeft.u = currentAnimateFrame * uMult + 0;
    vertices.topLeft.v = animType * vMult + 0;

    vertices.topRight.x = position.x + (size.x / 2.0f);
    vertices.topRight.y = position.y + (size.y / 2.0f);
    vertices.topRight.u = currentAnimateFrame * uMult + uMult;
    vertices.topRight.v = animType * vMult + 0;

    vertices.bottomLeft.x = position.x - (size.x / 2.0f);
    vertices.bottomLeft.y = position.y - (size.y / 2.0f);
    vertices.bottomLeft.u = currentAnimateFrame * uMult + 0;
    vertices.bottomLeft.v = animType * vMult + 0.3;

    vertices.bottomRight.x = position.x + (size.x / 2.0f);
    vertices.bottomRight.y = position.y - (size.y / 2.0f);
    vertices.bottomRight.u = currentAnimateFrame * uMult + uMult;
    vertices.bottomRight.v = animType * vMult + 0.3;

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
