//
// Created by Sami Sharif  on 12/23/25.
//

#pragma once

#include <SDL2/SDL.h>

#include "WaveManager.hpp"
#include "engine/Renderer.hpp"

class MainGame {
public:
    MainGame();
    ~MainGame();

    void run();

private:
    SDL_Window* window;
    SDL_GLContext gl_context;
    std::unique_ptr<Renderer> renderer;
    WaveManager waveManager;
    bool gameQuit;

    static constexpr const char* WINDOW_TITLE = "Dunes Tower Defense";
    static constexpr int WINDOW_WIDTH = 1200;
    static constexpr int WINDOW_HEIGHT = 900;

    void initSystems();
    float calculateFPS();
};

