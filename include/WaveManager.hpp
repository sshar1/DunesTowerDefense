//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once
#include <vector>

#include "engine/DataLoader.hpp"
#include "entities/Base.hpp"
#include "entities/Enemy.hpp"
#include "entities/projectiles/Projectile.hpp"
#include "entities/towers/Tower.hpp"

enum class GameState {
    PreWave,
    InWave,
    GameOver,
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
    bool gameOver() { return gameStats.gameState == GameState::GameOver; }
    bool inPreWave() { return gameStats.gameState == GameState::PreWave; }

    void update(const TopographyVertices& topVertices, float dt);

    std::vector<std::unique_ptr<Enemy>>& getEnemies();
    std::vector<std::unique_ptr<Projectile>>& getProjectiles();
    std::vector<std::unique_ptr<Tower>>& getTowers();
    Base* getBase() const;

private:
    GameStats gameStats;

    /* Wave System Configuration */
    static constexpr int TOTAL_WAVES = 10;
    static constexpr int STARTING_TOWERS_ALLOWANCE = 10;

    // Defines which enemies spawn in a wave
    struct WaveDefinition {
        int worms;      // Number of DuneWorms
        int beetles;    // Number of DungBeetles
        int bees;       // Number of Bees
    };
    static constexpr std::array<WaveDefinition, TOTAL_WAVES> WAVE_DEFINITIONS = {{
        {3, 0, 0},       // Wave 1
        {6, 0, 0},      // Wave 2
        {5, 2, 0},      // Wave 3
        {8, 3, 0},      // Wave 4
        {10, 4, 2},     // Wave 5
        {15, 6, 2},     // Wave 6
        {15, 8, 5},     // Wave 7
        {30, 0, 0},     // Wave 8
        {15, 2, 9},     // Wave 9
        {30, 10, 7},     // Wave 10
    }};

    void onWaveComplete();
    void transitionToNextWave();
    void setState(GameState newState) { gameStats.gameState = newState; }
    int getTowerAllowance() { return gameStats.waveNumber - 1 + STARTING_TOWERS_ALLOWANCE; }
    void spawnWaveEnemies();
    glm::vec2 getRandomEdgePosition();
};
