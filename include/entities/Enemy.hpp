//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once
#include "Sprite.hpp"
#include "engine/DataLoader.hpp"

enum class State {
    WALKING,
    ATTACKING,
    DYING,
    DEAD,
};

class Enemy {
public:
    Enemy(const char* filePath, int health, SpriteType type);
    Enemy(const char* filePath, int health, SpriteType type, glm::vec2 pos, glm::vec2 size, glm::vec2 targetPosition);
    virtual ~Enemy() = default;

    Sprite getSprite();

    void takeDamage(int damage);

    void update(const TopographyVertices& topVertices, float dt);

private:
    static constexpr int WALK_ANIM_TYPE = 0;
    static constexpr int ATTACKING_ANIM_TYPE = 1;
    static constexpr int DYING_ANIM_TYPE = 2;

    virtual int getMaxHealth() const = 0;
    virtual float getSpeed() const = 0;
    virtual void calculateWaypoints(const TopographyVertices& topVertices) = 0;

    void followPath(const TopographyVertices& topVertices, float dt);
    // float getSpeedOverPoints(const TopographyVertices& topVertices, glm::vec2 from, glm::vec2 to);
    float getDirectionalSpeed(const TopographyVertices& topVertices, glm::vec2 from, glm::vec2 direction);

    void updateAnimation();
    void setState(State newState);

protected:
    Sprite sprite;
    int health;
    State state;
    std::vector<glm::vec2> waypoints;
    glm::vec2 targetPosition;
    int currentWaypointIdx;
};
