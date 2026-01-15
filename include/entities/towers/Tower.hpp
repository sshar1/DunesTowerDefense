//
// Created by Sami Sharif  on 1/10/26.
//

#pragma once
#include "../Enemy.hpp"
#include "../Sprite.hpp"

class Tower {
public:
    enum class State {
        Idle,
        Attacking,
    };

    Tower(const char* filePath, SpriteType type, glm::vec2 pos, glm::vec2 size);
    virtual ~Tower() = default;

    Sprite getAttackSprite();

    void update(std::vector<std::unique_ptr<Enemy>>& enemies, std::vector<std::unique_ptr<Projectile>>& projectiles, float dt);
private:
    virtual void updateAnimation();
    virtual float getAttackCooldown() const = 0;
    virtual void attack(std::vector<std::unique_ptr<Projectile>>& projectiles) = 0;
    virtual void updateOrientation();
    // virtual int getAttackRange() const = 0;
    virtual void findEnemy(const std::vector<std::unique_ptr<Enemy>>& enemies) = 0;
    virtual bool enemyValid() const = 0;

    void setState(State newState);
protected:
    Sprite attackSprite;
    State state;
    Enemy* target;
    glm::vec2 pos;

    float elapsedAttackTime = 0.f;
};

// Attack cooldown
// Attack damage
// Range
// Current target