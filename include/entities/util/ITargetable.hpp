//
// Created by Sami Sharif  on 1/15/26.
//

#pragma once
// #include <glm/glm.hpp>

class ITargetable {
public:
    virtual ~ITargetable() = default;

    // Pure virtual: Children MUST implement this
    virtual glm::vec2 getPosition() const = 0;

    // Optional: Useful to check if the target is "dead" (hp <= 0)
    virtual bool isActive() const = 0;

    virtual void takeDamage(int damage) = 0;
};