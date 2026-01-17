//
// Created by Sami Sharif  on 1/10/26.
//

#include "../../../include/entities/towers/Sprayer.hpp"

#include <iostream>
#include <ostream>

#include "glm/detail/func_geometric.inl"
#include "glm/detail/type_vec2.hpp"

Sprayer::Sprayer(glm::vec2 position)
    : Tower(spriteFilePath, SpriteType::Gas, position, spriteSize)
{
}

void Sprayer::attack(std::vector<std::unique_ptr<Projectile>>& projectiles) {
    // attackSprite.playAnimation(false);
}

void Sprayer::findEnemy(const std::vector<std::unique_ptr<Enemy>>& enemies) {
    auto foundEnemy = std::find_if(enemies.begin(), enemies.end(), [&](const auto& enemy) {
        return enemy->isActive() && glm::distance(enemy->getSprite().getPosition(), pos) <= ATTACK_RANGE;
    });
    if (foundEnemy != enemies.end()) {
        target = (*foundEnemy).get();
    }
    else {
        target = nullptr;
    }
}

bool Sprayer::enemyValid() const {
    if (target == nullptr) return false;
    return target->isActive() && glm::distance(target->getSprite().getPosition(), pos) <= ATTACK_RANGE;
}
