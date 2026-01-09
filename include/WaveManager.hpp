//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once
#include <vector>

#include "engine/DataLoader.hpp"
#include "entities/Base.hpp"
#include "entities/Enemy.hpp"
#include "entities/Projectile.hpp"

enum class GameState {
    PreWave,
    InWave,
    GameOver,
};

struct GameStats {
    std::vector<std::unique_ptr<Enemy>> enemies{};
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

    void addEnemy(SpriteType type, glm::vec2 position, glm::vec2 targetPosition);
    void startWave();

    void update(const TopographyVertices& topVertices, float dt);

    std::vector<std::unique_ptr<Enemy>>& getEnemies();
    std::vector<std::unique_ptr<Projectile>>& getProjectiles();
    Base* getBase() const;

private:
    GameStats gameStats;
};
