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

void WaveManager::addEnemy(EnemyType type, glm::vec2 position, Base* base) {
    std::unique_ptr<Enemy> newEnemy = nullptr;

    switch (type) {
        case EnemyType::Beetle:
            newEnemy = std::make_unique<DungBeetle>(position, base);
            break;
        case EnemyType::Worm:
            newEnemy = std::make_unique<DuneWorm>(position, base);
            break;
        case EnemyType::Bee:
            newEnemy = std::make_unique<Bee>(position, base);
            break;
        default:
            std::cout << "Invalid Enemy Type!" << std::endl;
            return;
    }

    newEnemy->onDeath = [this]() {
        this->gameStats.aliveEnemies--;

        std::cout << "Enemy Down! Remaining: " << this->gameStats.aliveEnemies << std::endl;

        if (this->gameStats.aliveEnemies <= 0) {
            std::cout << "--- WAVE COMPLETE ---" << std::endl;
        }
    };

    gameStats.aliveEnemies++;

    gameStats.enemies.push_back(std::move(newEnemy));
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

void WaveManager::startWave() {
    if (gameStats.gameState == GameState::PreWave) {
        gameStats.gameState = GameState::InWave;
        spawnWaveEnemies();
    }
}

void WaveManager::onWaveComplete() {
    gameStats.projectiles.clear();
    gameStats.enemies.clear();
    gameStats.towers.clear();
    if (gameStats.waveNumber >= TOTAL_WAVES) {
        std::cout << "[WaveManager] All waves completed! Victory!" << std::endl;
        setState(GameState::GameOver);
        return;
    }

    transitionToNextWave();
}

void WaveManager::transitionToNextWave() {
    gameStats.waveNumber++;
    setState(GameState::PreWave);
}

void WaveManager::update(const TopographyVertices& topVertices, const std::vector<std::uint8_t>& colorMat, float dt) {
    Vision::Manager::getInstance().calculateWarpMatrix(topVertices);

    if (!gameStats.base->isActive()) {
        gameStats.gameState = GameState::GameOver;
    }

    switch (gameStats.gameState) {
        case GameState::PreWave:
			Vision::Manager::getInstance().calculateColorWarpMatrix(colorMat);
            break;
        case GameState::InWave:
            if (gameStats.aliveEnemies == 0) {
                onWaveComplete();
            }
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
            break;
        case GameState::GameOver:
            break;
    }
}

void WaveManager::spawnWaveEnemies() {
    const WaveDefinition& wave = WAVE_DEFINITIONS[gameStats.waveNumber - 1];

    // Spawn worms
    for (int i = 0; i < wave.worms; i++) {
        addEnemy(EnemyType::Worm, getRandomEdgePosition(), gameStats.base.get());
    }

    // Spawn beetles
    for (int i = 0; i < wave.beetles; i++) {
        addEnemy(EnemyType::Beetle, getRandomEdgePosition(), gameStats.base.get());
    }

    // Spawn bees
    for (int i = 0; i < wave.bees; i++) {
        addEnemy(EnemyType::Bee, getRandomEdgePosition(), gameStats.base.get());
    }
}

glm::vec2 WaveManager::getRandomEdgePosition() {
    const float w = MainGame::WINDOW_WIDTH;
    const float h = MainGame::WINDOW_HEIGHT;
    const float margin = 10.0f;  // Spawn slightly inside the edge

    static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> edgeDist(0, 3);  // 0=top, 1=right, 2=bottom, 3=left
    std::uniform_real_distribution<float> xDist(margin, w - margin);
    std::uniform_real_distribution<float> yDist(margin, h - margin);

    int edge = edgeDist(gen);
    switch (edge) {
        case 0:  // Top edge
            return glm::vec2(xDist(gen), margin);
        case 1:  // Right edge
            return glm::vec2(w - margin, yDist(gen));
        case 2:  // Bottom edge
            return glm::vec2(xDist(gen), h - margin);
        case 3:  // Left edge
        default:
            return glm::vec2(margin, yDist(gen));
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