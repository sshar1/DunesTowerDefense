//
// Created by Sami Sharif  on 12/26/25.
//

#include "WaveManager.hpp"

#include <iostream>
#include <ostream>
#include <random>

#include "entities/DuneWorm.hpp"
#include "entities/DungBeetle.hpp"

WaveManager::WaveManager()
{
    // Randomize base position
    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> distribution(-0.1f, 0.1f);
    gameStats.basePosition = glm::vec2{distribution(gen), distribution(gen)};
}

void WaveManager::addEnemy(SpriteType type, glm::vec2 position) {
    switch (type) {
        case SpriteType::Beetle:
            gameStats.enemies.push_back(std::make_unique<DungBeetle>(position));
            return;
        case SpriteType::Worm:
            gameStats.enemies.push_back(std::make_unique<DuneWorm>(position));
            return;
        default:
            std::cout << "This is not an enemy, canot add it" << std::endl;
    }
}

void WaveManager::update(float dt) {
    for (const auto& enemy : gameStats.enemies) {
        enemy->update(dt);
    }
}

std::vector<std::unique_ptr<Enemy>>& WaveManager::getEnemies() {
    return gameStats.enemies;
}