//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once

#include <string_view>

#include "glad.h"
#include "glm/detail/type_vec2.hpp"

#include <vector>

struct SpriteVertex {
    glm::vec2 position;
    float u;
    float v;
};

struct SpriteVertices {
    SpriteVertex topLeft;
    SpriteVertex topRight;
    SpriteVertex bottomLeft;
    SpriteVertex bottomRight;
};

enum class SpriteType {
    FirstEnemy,
        Beetle,
        Worm,
        Bee,
    LastEnemy,

    Stinger,
    Rock,

    FirstTowerData,
        Gas,
        Tongue,
        Lizard,
    LastTowerData,
    Base,
};

inline bool isEnemy(SpriteType type) { return type > SpriteType::FirstEnemy && type < SpriteType::LastEnemy; }
inline bool isProjectile(SpriteType type) { return type == SpriteType::Stinger || type == SpriteType::Rock; }
inline bool isTowerData(SpriteType type) { return type > SpriteType::FirstTowerData && type < SpriteType::LastTowerData; }

// Maybe replace this with bitwise flags?
inline bool isAnimatable(SpriteType type) {
    switch (type) {
        case SpriteType::Stinger:
        case SpriteType::Base:
            return false;
        default:
            return true;
    }
}

/* u, v is coords of top left corner */
struct UVRect {
    float u, v, w, h;
};

class Sprite {
public:
    Sprite(const char* filePath, SpriteType type);
    Sprite(const char* filePath, SpriteType type, glm::vec2 pos, glm::vec2 size);

    GLuint textureID;

    const SpriteVertices getVertices() const;
    void pushVertices(std::vector<SpriteVertex>& vertexBuffer) const;
    void setAnimType(int animType);

    // friend void Enemy::updateAnimation();
    SpriteType getType() const;
    glm::vec2 getPosition() const;
    glm::vec2 getLookVector() const;
    bool getVisible() const { return visible; }
    void setVisible(bool vis) { visible = vis; }
    void setPosition(glm::vec2 position);
    void setLookVector(glm::vec2 lookVector);
    void setSize(glm::vec2 newSize) { size = newSize; };

    void update(float dt);
    void playAnimation(bool loop);
    void resetAnimation();

private:
    constexpr static float TIME_BETWEEN_ANIMATE_FRAMES = 0.1; // in s
    constexpr static int ANIM_FRAMES = 4;

    int currentAnimateFrame;
    float elapsedAnimateTime;
    int animType;
    UVRect uvRect;
    bool loopAnimation;
    bool visible;

    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 lookVector;
    SpriteType spriteType;

    void updateUVRect();
};
