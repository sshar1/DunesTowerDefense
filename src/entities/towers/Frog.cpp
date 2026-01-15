//
// Created by Sami Sharif  on 1/14/26.
//

#include "../../../include/entities/towers/Frog.hpp"

#include "glm/detail/func_geometric.inl"
#include "glm/detail/type_vec2.hpp"

Frog::Frog(glm::vec2 position)
    : Tower(spriteFilePath, SpriteType::Tongue, position, spriteSize)
{
}

void Frog::attack(std::vector<std::unique_ptr<Projectile>>& projectiles) {
    attackSprite.playAnimation(false);
}

void Frog::updateOrientation() {
    if (target != nullptr)
        attackSprite.setLookVector((target->getSprite().getPosition() - attackSprite.getPosition()) * glm::vec2{-1, -1});
}

void Frog::findEnemy(const std::vector<std::unique_ptr<Enemy>>& enemies) {
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

bool Frog::enemyValid() const {
    return target == nullptr || glm::distance(target->getSprite().getPosition(), pos) <= ATTACK_RANGE;
}
