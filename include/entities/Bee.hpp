//
// Created by Sami Sharif  on 1/7/26.
//
#pragma once

#include "Enemy.hpp"
#include "glm/detail/type_vec2.hpp"

class Bee: public Enemy {
public:
    Bee();
    explicit Bee(glm::vec2 pos, Base* base);

private:
    static constexpr const char* spriteFilePath = "data/textures/bee_spritesheet.png";
    static constexpr glm::vec2 spriteSize = {1200/10, 900/10};

    static constexpr float SPEED = 30.f;
    static constexpr int MAX_HEALTH = 100;
    static constexpr float ATTACK_COOLDOWN = 2.f;

    int getMaxHealth() const override;
    float getAttackCooldown() const override;
    float getSpeed() const override;
    void calculateWaypoints(const TopographyVertices& topVertices) override;
    bool validAttackPosition(const TopographyVertices &topVertices) override;
    void attack(std::vector<std::unique_ptr<Projectile>>& projectiles) override;

    float getDirectionalSpeed(const TopographyVertices &topVertices, glm::vec2 from, glm::vec2 direction) override;
};
