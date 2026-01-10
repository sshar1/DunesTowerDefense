//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once

#include "Enemy.hpp"
#include "glm/detail/type_vec2.hpp"

class DungBeetle : public Enemy {
public:
    DungBeetle();
    explicit DungBeetle(glm::vec2 pos, glm::vec2 targetPosition);

private:
    static constexpr const char* spriteFilePath = "data/textures/dungbeetle_spritesheet.png";
    static constexpr glm::vec2 spriteSize = {1200/20, 900/20};

    static constexpr float SPEED = 30.f;
    static constexpr int MAX_HEALTH = 100;
    static constexpr float ATTACK_COOLDOWN = 2.f;

    int getMaxHealth() const override;
    float getAttackCooldown() const override;
    float getSpeed() const override;
    void calculateWaypoints(const TopographyVertices& topVertices) override;
    bool validAttackPosition(const TopographyVertices &topVertices) override;
    void attack(glm::vec2 targetPosition, std::vector<std::unique_ptr<Projectile>>& projectiles) override;
};
