//
// Created by Sami Sharif  on 1/14/26.
//
#pragma once

#include "Tower.hpp"
#include "glm/detail/type_vec.hpp"

class Frog : public Tower {
public:
    explicit Frog(glm::vec2 position);
private:
    static constexpr const char* spriteFilePath = "data/textures/frogtongue_spritesheet.png";
    static constexpr glm::vec2 spriteSize = {150, 120};

    static constexpr float ATTACK_COOLDOWN = 0.75f;
    static constexpr int ATTACK_RANGE = 90;
    static constexpr int DAMAGE = 30;

    float getAttackCooldown() const override { return ATTACK_COOLDOWN; }
    // void updateAnimation() override;
    // int getAttackRange() const override { return ATTACK_RANGE; }
    void updateOrientation() override;
    void findEnemy(const std::vector<std::unique_ptr<Enemy>>& enemies) override;
    bool enemyValid() const override;
    void attack(const std::vector<std::unique_ptr<Enemy>>& enemies, std::vector<std::unique_ptr<Projectile>>& projectiles) override;
};
