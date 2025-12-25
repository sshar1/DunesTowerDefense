//
// Created by Sami Sharif  on 12/23/25.
//

#include "InputManager.hpp"

#include <SDL2/SDL_events.h>

InputResult InputManager::processInput() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return InputResult::QUIT;
        }
    }

    return InputResult::NONE;
}
