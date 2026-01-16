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

// Time (in seconds) players have between waves to place towers and shape terrain
static constexpr float PRE_WAVE_DURATION = 30.0f;

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
// WAVE DEFINITIONS - Enemy spawns per wave
// ============================================================================

// Defines which enemies spawn in a wave
struct WaveDefinition {
    int beetles;    // Number of DungBeetles
    int worms;      // Number of DuneWorms
    int bees;       // Number of Bees
};

// Enemy counts per wave (indexed by wave number - 1)
// Wave 1: 3 beetles (easy intro)
// Wave 2: 4 beetles, 1 worm (introduce worms)
// Wave 3: 3 beetles, 2 worms, 1 bee (introduce bees)
// Wave 4: 4 beetles, 3 worms, 2 bees (ramp up)
// Wave 5: 5 beetles, 4 worms, 3 bees (final challenge)
static constexpr std::array<WaveDefinition, TOTAL_WAVES> WAVE_DEFINITIONS = {{
    {3, 0, 0},  // Wave 1
    {4, 1, 0},  // Wave 2
    {3, 2, 1},  // Wave 3
    {4, 3, 2},  // Wave 4
    {5, 4, 3},  // Wave 5
}};

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
    unsigned int baseHealth{BASE_STARTING_HEALTH};
    unsigned int waveNumber{1};
    GameState gameState{GameState::PreWave};
    unsigned int aliveEnemies{0};

    // Wave timing
    float preWaveTimer{PRE_WAVE_DURATION};  // Countdown timer for pre-wave phase

    // Tower allowance
    unsigned int towerAllowance{STARTING_TOWER_ALLOWANCE};  // Max towers player can place
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

    // State getters
    GameState getGameState() const;
    unsigned int getWaveNumber() const;
    float getPreWaveTimer() const;
    bool isVictory() const { return gameStats.gameState == GameState::GameOver && gameStats.waveNumber > TOTAL_WAVES; }

    // Tower allowance getters
    unsigned int getTowerAllowance() const;
    unsigned int getTowersPlaced() const;
    unsigned int getTowersRemaining() const;
    bool canPlaceTower() const;

private:
    GameStats gameStats;

    // State transition helpers
    void setState(GameState newState);
    void transitionToNextWave();
    void onWaveComplete();

    // Enemy spawning
    void spawnWaveEnemies();
    glm::vec2 getRandomEdgePosition();  // Get random position on map edge

    // Wave completion
    bool areAllEnemiesDead() const;
};
