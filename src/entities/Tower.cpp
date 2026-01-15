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
    // setState(State::Idle);
}

void Tower::update(std::vector<std::unique_ptr<Enemy>>& enemies, std::vector<std::unique_ptr<Projectile>>& projectiles, float dt) {
    attackSprite.update(dt);

    switch (state) {
        case State::Idle: {
            findEnemy(enemies);
            if (target != nullptr) {
                setState(State::Attacking);
            }
            break;
        }
        case State::Attacking:
            elapsedAttackTime += dt;
            if (elapsedAttackTime >= getAttackCooldown()) {
                // attackSprite.playAnimation(true);
                elapsedAttackTime = 0;
                attack(projectiles);
            }

            if (!enemyValid()) {
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

    // if (state == State::Idle) attackSprite.setVisible(false);
    // else if (state == State::Attacking) {
    //     attackSprite.setVisible(true);
    //     attackSprite.playAnimation(true);
    // }
    updateAnimation();
}

void Tower::updateAnimation() {
    if (state == State::Idle) attackSprite.setVisible(false);
    else if (state == State::Attacking) {
        attackSprite.setVisible(true);
        attackSprite.playAnimation(true);
    }
}

Sprite Tower::getAttackSprite() {
    return attackSprite;
}