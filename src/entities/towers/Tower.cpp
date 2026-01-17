//
// Created by Sami Sharif  on 1/10/26.
//

#include "../../../include/entities/towers/Tower.hpp"

#include "MainGame.hpp"
#include "engine/VisionManager.hpp"

Tower::Tower(const char* filePath, SpriteType type, glm::vec2 pos, glm::vec2 size)
    : attackSprite(filePath, type, pos, size)
    , state(State::Idle)
    , target(nullptr)
    , pos(pos)
{
	attackSprite.setVisible(false);
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
            updateOrientation();
            elapsedAttackTime += dt;
            if (elapsedAttackTime >= getAttackCooldown()) {
                elapsedAttackTime = 0;
                attack(projectiles);
            }

            if (!enemyValid()) {
                setState(State::Idle);
            }
            break;
    }
}

void Tower::setState(State newState) {
    if (state != newState) {
        state = newState;
        // updateAnimation();
    }

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

void Tower::updateOrientation()
{
}
