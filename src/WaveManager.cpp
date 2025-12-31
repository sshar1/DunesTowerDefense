//
// Created by Sami Sharif  on 12/26/25.
//

#include "WaveManager.hpp"

#include <random>

WaveManager::WaveManager()
{
    // Randomize base position
    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> distribution(-0.1f, 0.1f);
    gameStats.basePosition = glm::vec2{distribution(gen), distribution(gen)};
}

void WaveManager::addEnemy(std::unique_ptr<Enemy> enemy) {
    gameStats.enemies.push_back(std::move(enemy));
}
