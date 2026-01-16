//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once
#include <vector>
#include <array>

#include "engine/DataLoader.hpp"
#include "entities/Base.hpp"
#include "entities/Enemy.hpp"
#include "entities/projectiles/Projectile.hpp"
#include "entities/towers/Tower.hpp"

// ============================================================================
// WAVE SYSTEM CONFIGURATION
// ============================================================================

// Total number of waves in the game
static constexpr int TOTAL_WAVES = 5;

// Time (in seconds) players have between waves to place towers
static constexpr float PRE_WAVE_DURATION = 10.0f;

// Tower allowance: how many towers can be placed
static constexpr int STARTING_TOWER_ALLOWANCE = 3;

// Additional towers allowed per wave (indexed by wave number - 1)
// Wave 1: +2, Wave 2: +2, Wave 3: +3, Wave 4: +3, Wave 5: +4
static constexpr std::array<int, TOTAL_WAVES> TOWERS_PER_WAVE = {2, 2, 3, 3, 4};

// Base health configuration
static constexpr int BASE_STARTING_HEALTH = 100;

// Health boost per wave (indexed by wave number - 1, wave 1 gives no boost)
// After wave 1: +10, after wave 2: +15, after wave 3: +20, after wave 4: +25
static constexpr std::array<int, TOTAL_WAVES> HEALTH_BOOST_PER_WAVE = {0, 10, 15, 20, 25};

// Maximum health the base can have (never exceeds this even with boosts)
static constexpr int BASE_MAX_HEALTH = 100;

// ============================================================================
// GAME STATE ENUMS
// ============================================================================

enum class GameState {
    PreWave,    // Between waves, players can place towers
    InWave,     // Wave is active, enemies are spawning/attacking
    GameOver,   // Base destroyed or all waves completed
};

enum class TowerType {
    Sprayer,
    Frog,
    Mortar,
};

enum class EnemyType {
    Beetle,
    Worm,
    Bee,
};

struct GameStats {
    std::vector<std::unique_ptr<Enemy>> enemies{};
    std::vector<std::unique_ptr<Tower>> towers{};
    std::vector<std::unique_ptr<Projectile>> projectiles{};
    std::unique_ptr<Base> base;
    unsigned int baseHealth{100};
    unsigned int waveNumber{1};
    GameState gameState{GameState::PreWave};
    unsigned int aliveEnemies{0};
};

class WaveManager {
public:
    WaveManager();

    void initSystems();

    void addEnemy(EnemyType type, glm::vec2 position, Base* base);
    void addTower(TowerType type, glm::vec2 position);
    void startWave();

    void update(const TopographyVertices& topVertices, float dt);

    std::vector<std::unique_ptr<Enemy>>& getEnemies();
    std::vector<std::unique_ptr<Projectile>>& getProjectiles();
    std::vector<std::unique_ptr<Tower>>& getTowers();
    Base* getBase() const;

private:
    GameStats gameStats;
};
