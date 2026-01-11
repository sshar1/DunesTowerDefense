//
// Created by Sami Sharif  on 1/10/26.
//

#pragma once
#include "Enemy.hpp"
#include "Sprite.hpp"

class Tower {
    enum class State {
        Idle,
        Attacking,
    };
public:
    Tower(const char* filePath, SpriteType type, glm::vec2 pos, glm::vec2 size);
    virtual ~Tower() = default;

    Sprite getAttackSprite();

    void update(std::vector<std::unique_ptr<Enemy>>& enemies, float dt);
private:
    // virtual void updateAnimation() = 0;
    virtual float getAttackCooldown() const = 0;
    virtual void attack() const = 0;

    void setState(State newState);
protected:
    Sprite attackSprite;
    State state;
    Enemy* target;

    float elapsedAttackTime = 0.f;
};

// Attack cooldown
// Attack damage
// Range
// Current target