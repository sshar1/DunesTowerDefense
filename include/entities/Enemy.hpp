//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once
#include <functional>

#include "Base.hpp"
#include "projectiles/Projectile.hpp"
#include "Sprite.hpp"
#include "engine/DataLoader.hpp"
#include "util/ITargetable.hpp"


class Enemy : public ITargetable {
    enum class State {
        WALKING,
        ATTACKING,
        DYING,
        DEAD,
    };
public:
    Enemy(const char* filePath, int health, SpriteType type);
    Enemy(const char* filePath, int health, SpriteType type, glm::vec2 pos, glm::vec2 size, Base* base);
    virtual ~Enemy() = default;

    Sprite getSprite() const;
    bool isActive() const override { return state != State::DEAD; }
    void takeDamage(int damage) override { health -= damage; }

    void update(const TopographyVertices& topVertices, std::vector<std::unique_ptr<Projectile>>& projectiles, float dt);

    std::function<void()> onDeath = nullptr;
private:
    static constexpr int WALK_ANIM_TYPE = 0;
    static constexpr int ATTACKING_ANIM_TYPE = 1;
    static constexpr int DYING_ANIM_TYPE = 2;

    static constexpr float speedDampCoefficient = 15.f; // 30

    virtual int getMaxHealth() const = 0;
    virtual float getAttackCooldown() const = 0;
    virtual float getSpeed() const = 0;
    virtual void calculateWaypoints(const TopographyVertices& topVertices) = 0;
    virtual bool validAttackPosition(const TopographyVertices& topVertices) = 0;
    virtual void attack(std::vector<std::unique_ptr<Projectile>>& projectiles) = 0;

    glm::vec2 getPosition() const override { return getSprite().getPosition(); }

    void followPath(float dt);
    virtual float getDirectionalSpeed(glm::vec2 from, glm::vec2 direction);
    bool validStartAttackPos();
    virtual int getValidStartAttackThresh() { return 4; }

    void updateAnimation();
    void setState(State newState);

protected:
    Sprite sprite;
    int health;
    State state;
    std::vector<glm::vec2> waypoints;
    Base* base;
    int currentWaypointIdx;

    float elapsedAttackTime = 0.f;
};
