//
// Created by Sami Sharif  on 1/2/26.
//

#pragma once
#include "Sprite.hpp"
#include "util/ITargetable.hpp"

class Base : public ITargetable {
public:
    Base(glm::vec2 position);

    Sprite getSprite() const;

    // ITargetable interface
    void takeDamage(int damage) override { health -= damage; }
    glm::vec2 getPosition() const override { return getSprite().getPosition(); }
    bool isActive() const override { return this->health > 0; }

    // Health management
    int getHealth() const { return health; }
    int getMaxHealth() const { return MAX_HEALTH; }
    float getVertOffset() const { return spriteSize.y; }
    void heal(int amount);  // Heals up to MAX_HEALTH
    bool isDestroyed() const { return health <= 0; }

private:
    static constexpr const char* spriteFilePath = "data/textures/base_sprite.png";
    static constexpr glm::vec2 spriteSize = {0.2*1200, 0.2*900};
    static constexpr int MAX_HEALTH = 100;

    Sprite sprite;
    int health;
};
