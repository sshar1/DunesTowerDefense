//
// Created by Sami Sharif  on 1/13/26.
//

#pragma once
#include "Tower.hpp"
#include "glm/detail/type_vec.hpp"

class Mortar : public Tower {
public:
    explicit Mortar(glm::vec2 position);
private:
    static constexpr const char* spriteFilePath = "data/textures/rock_spritesheet.png";
    static inline const glm::vec2 spriteSize = {150, 120};

    static constexpr float ATTACK_COOLDOWN = 3.f;
    static constexpr int ATTACK_MAX_RANGE = 700;
    static constexpr int ATTACK_MIN_RANGE = 250;

    float getAttackCooldown() const override { return ATTACK_COOLDOWN; }
    void updateAnimation() override;
    // int getAttackRange() const override { return ATTACK_RANGE; }
    void findEnemy(const std::vector<std::unique_ptr<Enemy>>& enemies) override;
    bool enemyValid() const override;
    void attack(const std::vector<std::unique_ptr<Enemy>>& enemies, std::vector<std::unique_ptr<Projectile>>& projectiles) override;
};
