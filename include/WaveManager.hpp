//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once
#include <vector>

#include "entities/Base.hpp"
#include "entities/Enemy.hpp"

enum class GameState {
    PreWave,
    InWave,
    GameOver,
};

struct GameStats {
    std::vector<std::unique_ptr<Enemy>> enemies{};
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

    void update(float dt);

    std::vector<std::unique_ptr<Enemy>>& getEnemies();
    Base* getBase() const;

private:
    GameStats gameStats;
};
