//
// Created by Sami Sharif  on 12/23/25.
//

#include "MainGame.hpp"
#include "InputManager.hpp"
#include "engine/DataLoader.hpp"
#include "engine/Renderer.hpp"

MainGame::MainGame()
    : window(nullptr)
    , gl_context(nullptr)
    , renderer(nullptr)
    , gameQuit(false)
{
}

MainGame::~MainGame() {
    SDL_DestroyWindow(window);
}

void MainGame::initSystems() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        gameQuit = true;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    if (window == nullptr) {
        gameQuit = true;
        return;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        gameQuit = true;
        return;
    }

    DataLoader& dataLoader = DataLoader::getInstance();
    const TopographyVertices& topVertices = dataLoader.processTopographyVertices("tests/test_data/depth_readings/KinectDepthData-04-05-49.csv");

    renderer = std::make_unique<Renderer>(DataLoader::DEPTH_WIDTH, DataLoader::DEPTH_HEIGHT);
    renderer->initVertexObjects(&topVertices);
}

void MainGame::run() {
    initSystems();

    while (!gameQuit) {
        InputManager& inputManager = InputManager::getInstance();
        InputResult inputResult = inputManager.processInput();
        if (inputResult == InputResult::QUIT) {
            gameQuit = true;
        }

        renderer->clearBuffer();
        renderer->renderTopography();

        SDL_GL_SwapWindow(window);
    }
}