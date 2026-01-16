//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once

#include "Enemy.hpp"
#include "glm/detail/type_vec2.hpp"

class DuneWorm : public Enemy {
public:
    DuneWorm();
    explicit DuneWorm(glm::vec2 pos, Base* base);

private:
    static constexpr const char* spriteFilePath = "data/textures/duneworm_spritesheet.png";
    static constexpr glm::vec2 spriteSize = {1200/20, 900/20};

    static constexpr float SPEED = 60.f;
    static constexpr int MAX_HEALTH = 100;
    static constexpr int DAMAGE = 1;
    static constexpr float ATTACK_COOLDOWN = 0.75f;

    int getMaxHealth() const override;
    float getAttackCooldown() const override;
    float getSpeed() const override;
    void calculateWaypoints(const TopographyVertices& topVertices) override;
    bool validAttackPosition(const TopographyVertices &topVertices) override;
    void attack(std::vector<std::unique_ptr<Projectile>>& projectiles) override;
};
