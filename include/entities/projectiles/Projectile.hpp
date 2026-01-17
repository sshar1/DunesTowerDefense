//
// Created by Sami Sharif  on 1/9/26.
//

#pragma once
#include "../Sprite.hpp"
#include "entities/util/ITargetable.hpp"
// #include "entities/Enemy.hpp"

class Enemy;

class Projectile {
    enum class State {
        Firing,
        Landing,
        Landed,
    };
public:
    Projectile(const char* filePath, SpriteType type, glm::vec2 pos, glm::vec2 size, ITargetable* target);
    virtual ~Projectile() = default;

    Sprite getSprite();

    void update(const std::vector<std::unique_ptr<Enemy>>& enemies, float dt);
    bool isActive() const { return state != State::Landed; }
private:
    virtual float getSpeed() const = 0;
    // virtual int getDamage() const = 0;
    virtual void attack(const std::vector<std::unique_ptr<Enemy>>& enemies) = 0;
    virtual void updateAnimation() = 0;
    virtual void updateSize() = 0;

    void setState(State newState);
protected:
    static constexpr int FIRING_ANIM_TYPE = 0;
    static constexpr int LANDED_ANIM_TYPE = 1;

    Sprite sprite;
    State state;
    ITargetable* target;
    glm::vec2 lastKnownPosition;
    glm::vec2 originPosition;

    virtual void followPath(float dt);
};
