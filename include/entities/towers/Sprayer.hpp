//
// Created by Sami Sharif  on 1/10/26.
//

#pragma once
#include "Tower.hpp"
#include "glm/detail/type_vec.hpp"

class Sprayer : public Tower {
public:
    explicit Sprayer(glm::vec2 position);
private:
    static constexpr const char* spriteFilePath = "data/textures/gas_spritesheet.png";
    static constexpr glm::vec2 spriteSize = {200, 160};

    static constexpr float ATTACK_COOLDOWN = 0.5f;
    static constexpr int ATTACK_RANGE = 80;
    static constexpr int DAMAGE = 20;

    float getAttackCooldown() const override { return ATTACK_COOLDOWN; }
    // void updateAnimation() override;
    // int getAttackRange() const override { return ATTACK_RANGE; }
    void findEnemy(const std::vector<std::unique_ptr<Enemy>>& enemies) override;
    bool enemyValid() const override;
    void attack(std::vector<std::unique_ptr<Projectile>>& projectiles) override;
};
