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

    // Log initial state
    std::cout << "[WaveManager] Initialized - Wave " << gameStats.waveNumber
              << ", State: PreWave, Timer: " << gameStats.preWaveTimer << "s"
              << ", Tower allowance: " << gameStats.towerAllowance << std::endl;
}

void WaveManager::addEnemy(EnemyType type, glm::vec2 position, Base* base) {
    switch (type) {
        case EnemyType::Beetle:
            gameStats.enemies.push_back(std::make_unique<DungBeetle>(position, base));
            return;
        case EnemyType::Worm:
            gameStats.enemies.push_back(std::make_unique<DuneWorm>(position, base));
            return;
        case EnemyType::Bee:
            gameStats.enemies.push_back(std::make_unique<Bee>(position, base));
            return;
        default:
            std::cout << "This is not an enemy, cannot add it" << std::endl;
    }
}

void WaveManager::addTower(TowerType type, glm::vec2 position) {
    // Check tower allowance
    if (!canPlaceTower()) {
        std::cout << "[WaveManager] Cannot place tower - allowance exceeded ("
                  << getTowersPlaced() << "/" << getTowerAllowance() << ")" << std::endl;
        return;
    }

    switch (type) {
        case TowerType::Sprayer:
            gameStats.towers.push_back(std::make_unique<Sprayer>(position));
            break;
        case TowerType::Frog:
            gameStats.towers.push_back(std::make_unique<Frog>(position));
            break;
        case TowerType::Mortar:
            gameStats.towers.push_back(std::make_unique<Mortar>(position));
            break;
        default:
            std::cout << "[WaveManager] Unknown tower type, cannot add" << std::endl;
            return;
    }

    std::cout << "[WaveManager] Tower placed (" << getTowersPlaced() << "/"
              << getTowerAllowance() << "), " << getTowersRemaining() << " remaining" << std::endl;
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

// ============================================================================
// STATE GETTERS
// ============================================================================

GameState WaveManager::getGameState() const {
    return gameStats.gameState;
}

unsigned int WaveManager::getWaveNumber() const {
    return gameStats.waveNumber;
}

float WaveManager::getPreWaveTimer() const {
    return gameStats.preWaveTimer;
}

// ============================================================================
// TOWER ALLOWANCE
// ============================================================================

unsigned int WaveManager::getTowerAllowance() const {
    return gameStats.towerAllowance;
}

unsigned int WaveManager::getTowersPlaced() const {
    return gameStats.towers.size();
}

unsigned int WaveManager::getTowersRemaining() const {
    unsigned int placed = getTowersPlaced();
    if (placed >= gameStats.towerAllowance) return 0;
    return gameStats.towerAllowance - placed;
}

bool WaveManager::canPlaceTower() const {
    return getTowersPlaced() < gameStats.towerAllowance;
}

// ============================================================================
// STATE TRANSITIONS
// ============================================================================

void WaveManager::setState(GameState newState) {
    if (gameStats.gameState == newState) return;

    GameState oldState = gameStats.gameState;
    gameStats.gameState = newState;

    // Log state transition for debugging
    std::cout << "[WaveManager] State: ";
    switch (oldState) {
        case GameState::PreWave: std::cout << "PreWave"; break;
        case GameState::InWave: std::cout << "InWave"; break;
        case GameState::GameOver: std::cout << "GameOver"; break;
    }
    std::cout << " -> ";
    switch (newState) {
        case GameState::PreWave: std::cout << "PreWave"; break;
        case GameState::InWave: std::cout << "InWave"; break;
        case GameState::GameOver: std::cout << "GameOver"; break;
    }
    std::cout << " (Wave " << gameStats.waveNumber << ")" << std::endl;
}

void WaveManager::startWave() {
    if (gameStats.gameState != GameState::PreWave) {
        std::cout << "[WaveManager] Cannot start wave - not in PreWave state" << std::endl;
        return;
    }

    std::cout << "[WaveManager] Starting wave " << gameStats.waveNumber << std::endl;
    setState(GameState::InWave);

    // Spawn enemies for this wave
    spawnWaveEnemies();
}

glm::vec2 WaveManager::getRandomEdgePosition() {
    const float w = MainGame::WINDOW_WIDTH;
    const float h = MainGame::WINDOW_HEIGHT;
    const float margin = 50.0f;  // Spawn slightly inside the edge

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

void WaveManager::spawnWaveEnemies() {
    const WaveDefinition& wave = WAVE_DEFINITIONS[gameStats.waveNumber - 1];
    glm::vec2 targetPosition = gameStats.base->getSprite().getPosition();

    int totalEnemies = wave.beetles + wave.worms + wave.bees;

    std::cout << "[WaveManager] Spawning " << totalEnemies << " enemies: "
              << wave.beetles << " beetles, "
              << wave.worms << " worms, "
              << wave.bees << " bees" << std::endl;

    // Spawn beetles
    for (int i = 0; i < wave.beetles; i++) {
        addEnemy(EnemyType::Beetle, getRandomEdgePosition(), targetPosition);
    }

    // Spawn worms
    for (int i = 0; i < wave.worms; i++) {
        addEnemy(EnemyType::Worm, getRandomEdgePosition(), targetPosition);
    }

    // Spawn bees
    for (int i = 0; i < wave.bees; i++) {
        addEnemy(EnemyType::Bee, getRandomEdgePosition(), targetPosition);
    }
}

void WaveManager::onWaveComplete() {
    std::cout << "[WaveManager] Wave " << gameStats.waveNumber << " complete!" << std::endl;

    // Check if this was the last wave
    if (gameStats.waveNumber >= TOTAL_WAVES) {
        std::cout << "[WaveManager] All waves completed! Victory!" << std::endl;
        setState(GameState::GameOver);
        return;
    }

    // Move to next wave
    transitionToNextWave();
}

void WaveManager::transitionToNextWave() {
    // Get the bonus towers for the COMPLETED wave (before incrementing)
    int bonusTowers = TOWERS_PER_WAVE[gameStats.waveNumber - 1];
    gameStats.towerAllowance += bonusTowers;

    // Get health boost for the COMPLETED wave (before incrementing)
    int healthBoost = HEALTH_BOOST_PER_WAVE[gameStats.waveNumber - 1];
    int oldHealth = gameStats.base->getHealth();
    gameStats.base->heal(healthBoost);
    int newHealth = gameStats.base->getHealth();

    gameStats.waveNumber++;
    gameStats.preWaveTimer = PRE_WAVE_DURATION;  // Reset timer for next pre-wave

    setState(GameState::PreWave);
    std::cout << "[WaveManager] Preparing for wave " << gameStats.waveNumber
              << " - " << gameStats.preWaveTimer << "s to place towers"
              << " (+" << bonusTowers << " towers, total allowance: " << gameStats.towerAllowance << ")"
              << " (+" << healthBoost << " health: " << oldHealth << " -> " << newHealth << ")" << std::endl;
}