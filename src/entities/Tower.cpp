//
// Created by Sami Sharif  on 1/10/26.
//

#include "entities/Tower.hpp"

#include "MainGame.hpp"
#include "engine/VisionManager.hpp"

Tower::Tower(const char* filePath, SpriteType type, glm::vec2 pos, glm::vec2 size)
    : attackSprite(filePath, type, pos, size)
    , state(State::Idle)
    , target(nullptr)
    , pos(pos)
{
    setState(State::Idle);
}

void Tower::update(std::vector<std::unique_ptr<Enemy>>& enemies, float dt) {
    attackSprite.update(dt);

    auto foundEnemy = std::find_if(enemies.begin(), enemies.end(), [&](const auto& enemy) {
        return glm::distance(enemy->getSprite().getPosition(), pos) <= getAttackRange();
    });

    switch (state) {
        case State::Idle: {
            if (foundEnemy != enemies.end()) {
                target = (*foundEnemy).get();
                setState(State::Attacking);
            }

            break;
        }
        case State::Attacking:
            elapsedAttackTime += dt;
            if (elapsedAttackTime >= getAttackCooldown()) {
                // attackSprite.playAnimation(true);
                elapsedAttackTime = 0;
                attack();
            }

            if (foundEnemy == enemies.end() || target == nullptr) {
                setState(State::Idle);
            }

            break;
    }
}

// void Enemy::updateAnimation() {
//     switch (state) {
//         case State::WALKING:
//             sprite.setAnimType(WALK_ANIM_TYPE);
//             sprite.playAnimation(true);
//             return;
//         case State::ATTACKING:
//             sprite.setAnimType(ATTACKING_ANIM_TYPE);
//             sprite.playAnimation(false);
//             return;
//         case State::DYING:
//         case State::DEAD:
//             sprite.setAnimType(DYING_ANIM_TYPE);
//     }
// }

void Tower::setState(State newState) {
    if (state != newState) {
        state = newState;
        // updateAnimation();
    }

    if (state == State::Idle) attackSprite.setVisible(false);
    else if (state == State::Attacking) attackSprite.setVisible(true);
}

Sprite Tower::getAttackSprite() {
    return attackSprite;
}