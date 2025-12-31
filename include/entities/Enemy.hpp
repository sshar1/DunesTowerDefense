//
// Created by Sami Sharif  on 12/26/25.
//

#pragma once
#include "Sprite.hpp"

class Enemy {
public:
    Enemy(const char* filePath);

    Sprite getSprite();

    void takeDamage(int damage);


private:
    virtual int getMaxHealth() const = 0;

protected:
    Sprite sprite;
    int health;
};
