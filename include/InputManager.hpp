//
// Created by Sami Sharif  on 12/23/25.
//

#pragma once

enum class InputResult {
    NONE,
    QUIT,
};

class InputManager {
public:
    static InputManager& getInstance() {
        static InputManager instance;
        return instance;
    }

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    InputManager(InputManager&&) = delete;
    InputManager& operator=(InputManager&&) = delete;

    InputResult processInput();

private:
    InputManager() = default;
    ~InputManager() = default;
};
