//
// Created by Sami Sharif  on 1/7/26.
//
#pragma once

#include "Enemy.hpp"
#include "glm/detail/type_vec2.hpp"

class Bee: public Enemy {
public:
    Bee();
    explicit Bee(glm::vec2 pos, glm::vec2 targetPosition);

private:
    static constexpr const char* spriteFilePath = "data/textures/bee_spritesheet.png";
    static constexpr glm::vec2 spriteSize = {0.2, 0.2};

    static constexpr float SPEED = 0.05f;
    static constexpr int MAX_HEALTH = 100;

    int getMaxHealth() const override;
    float getSpeed() const override;
    void calculateWaypoints(const TopographyVertices& topVertices) override;
    bool validAttackPosition(const TopographyVertices &topVertices) override;
};
