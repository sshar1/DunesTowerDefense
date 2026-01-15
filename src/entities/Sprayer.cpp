//
// Created by Sami Sharif  on 1/10/26.
//

#include "entities/Sprayer.hpp"

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
        return glm::distance(enemy->getSprite().getPosition(), pos) <= ATTACK_RANGE;
    });
    if (foundEnemy != enemies.end()) {
        target = (*foundEnemy).get();
    }
    else {
        target = nullptr;
    }
}

bool Sprayer::enemyValid() const {
    return target == nullptr || glm::distance(target->getSprite().getPosition(), pos) <= ATTACK_RANGE;
}

// void Sprayer::updateAnimation() {
//     if (state == State::Idle) attackSprite.setVisible(false);
//     else if (state == State::Attacking) {
//         attackSprite.setVisible(true);
//         attackSprite.playAnimation(true);
//     }
// }
