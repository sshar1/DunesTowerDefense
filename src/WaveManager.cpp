//
// Created by Sami Sharif  on 12/26/25.
//

#include "WaveManager.hpp"

#include <iostream>
#include <ostream>
#include <random>

#include "MainGame.hpp"
#include "engine/VisionManager.hpp"
#include "entities/Bee.hpp"
#include "entities/DuneWorm.hpp"
#include "entities/DungBeetle.hpp"
#include "../include/entities/towers/Mortar.hpp"
#include "../include/entities/towers/Sprayer.hpp"
#include "../include/entities/towers/Frog.hpp"

WaveManager::WaveManager()
{
}

void WaveManager::initSystems() {
    // Randomize base position
    const float w = MainGame::WINDOW_WIDTH;
    const float h = MainGame::WINDOW_HEIGHT;

    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> xDistribution(w/2 - 0.1 * w, w/2 + 0.1 * w);
    std::uniform_real_distribution<float> yDistribution(h/2 - 0.1 * h, h/2 + 0.1 * h);
    glm::vec2 basePosition = glm::vec2{xDistribution(gen), yDistribution(gen)};

    gameStats.base = std::make_unique<Base>(basePosition);
}

void WaveManager::addEnemy(EnemyType type, glm::vec2 position, glm::vec2 targetPosition) {
    switch (type) {
        case EnemyType::Beetle:
            gameStats.enemies.push_back(std::make_unique<DungBeetle>(position, targetPosition));
            return;
        case EnemyType::Worm:
            gameStats.enemies.push_back(std::make_unique<DuneWorm>(position, targetPosition));
            return;
        case EnemyType::Bee:
            gameStats.enemies.push_back(std::make_unique<Bee>(position, targetPosition));
            return;
        default:
            std::cout << "This is not an enemy, cannot add it" << std::endl;
    }
}

void WaveManager::addTower(TowerType type, glm::vec2 position) {
    switch (type) {
        case TowerType::Sprayer:
            gameStats.towers.push_back(std::make_unique<Sprayer>(position));
            return;
        case TowerType::Frog:
            gameStats.towers.push_back(std::make_unique<Frog>(position));
            return;
        case TowerType::Mortar:
            gameStats.towers.push_back(std::make_unique<Mortar>(position));
            return;
        default:
            std::cout << "This is not an enemy, cannot add it" << std::endl;
    }
}

void WaveManager::update(const TopographyVertices& topVertices, float dt) {
    Vision::Manager::getInstance().calculateWarpMatrix(topVertices);
    Vision::Manager::getInstance().evaluateHills();
    for (const auto& enemy : gameStats.enemies) {
        enemy->update(topVertices, gameStats.projectiles, dt);
    }
    for (const auto& projectile : gameStats.projectiles) {
        projectile->update(gameStats.enemies, dt);
    }
    for (const auto& tower : gameStats.towers) {
        tower->update(gameStats.enemies, gameStats.projectiles, dt);
    }
}

std::vector<std::unique_ptr<Enemy>>& WaveManager::getEnemies() {
    return gameStats.enemies;
}

std::vector<std::unique_ptr<Projectile>>& WaveManager::getProjectiles() {
    return gameStats.projectiles;
}

std::vector<std::unique_ptr<Tower>>& WaveManager::getTowers() {
    return gameStats.towers;
}

Base* WaveManager::getBase() const {
    return gameStats.base.get();
}