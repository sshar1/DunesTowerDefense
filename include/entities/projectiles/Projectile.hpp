//
// Created by Sami Sharif  on 1/9/26.
//

#pragma once
#include "../Sprite.hpp"
// #include "entities/Enemy.hpp"

class Enemy;

class Projectile {
    enum class State {
        Firing,
        Landing,
        Landed,
    };
public:
    Projectile(const char* filePath, SpriteType type, glm::vec2 pos, glm::vec2 size, glm::vec2 targetPosition);
    virtual ~Projectile() = default;

    Sprite getSprite();

    void update(const std::vector<std::unique_ptr<Enemy>>& enemies, float dt);
private:
    virtual float getSpeed() const = 0;
    virtual void updateAnimation() = 0;
    virtual void updateSize() = 0;

    void setState(State newState);
protected:
    static constexpr int FIRING_ANIM_TYPE = 0;
    static constexpr int LANDED_ANIM_TYPE = 1;

    Sprite sprite;
    State state;
    glm::vec2 targetPosition;
    glm::vec2 originPosition;

    virtual void followPath(float dt);
};
