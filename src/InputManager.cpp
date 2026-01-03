//
// Created by Sami Sharif  on 12/23/25.
//

#include "InputManager.hpp"

#include <iostream>
#include <ostream>
#include <SDL2/SDL_events.h>

InputResult InputManager::processInput() {
    SDL_Event event;
    int mouseX, mouseY;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return InputResult::QUIT;
        }
    }

    // SDL_GetMouseState(&mouseX, &mouseY);
    // std::cout << "mouseX: " << mouseX << " mouseY: " << mouseY << std::endl;

    return InputResult::NONE;
}

// 300, 108
// 926, 139
// 915, 589
// 292, 541