//
// Created by Sami Sharif  on 1/13/26.
//

#include "../../../include/entities/towers/Mortar.hpp"

#include <iostream>

#include "entities/projectiles/Rock.hpp"
#include "glm/detail/func_geometric.hpp"

Mortar::Mortar(glm::vec2 position)
    : Tower(spriteFilePath, SpriteType::Gas, position, spriteSize)
{
    // attackSprite.setVisible(false);
}

void Mortar::attack(std::vector<std::unique_ptr<Projectile>>& projectiles) {
    // attackSprite.playAnimation(true);

    if (target != nullptr)
        projectiles.push_back(std::make_unique<Rock>(attackSprite.getPosition(), target->getSprite().getPosition()));
}

void Mortar::findEnemy(const std::vector<std::unique_ptr<Enemy>>& enemies) {
    auto foundEnemy = std::find_if(enemies.begin(), enemies.end(), [&](const auto& enemy) {
        const float dist = glm::distance(enemy->getSprite().getPosition(), pos);
        return dist <= ATTACK_MAX_RANGE && dist >= ATTACK_MIN_RANGE;
    });
    if (foundEnemy != enemies.end()) {
        target = (*foundEnemy).get();
    }
    else {
        target = nullptr;
    }
}

bool Mortar::enemyValid() const {
    if (target == nullptr) return false;

    const float dist = glm::distance(target->getSprite().getPosition(), pos);
    return dist <= ATTACK_MAX_RANGE && dist >= ATTACK_MIN_RANGE;
}

void Mortar::updateAnimation() {
}
