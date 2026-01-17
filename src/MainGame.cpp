//
// Created by Sami Sharif  on 12/23/25.
//

#include "MainGame.hpp"

#include <SDL_image.h>

#include "InputManager.hpp"
#include "engine/DataLoader.hpp"
#include "engine/Renderer.hpp"
#include "engine/VisionManager.hpp"

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

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        gameQuit = true;
        return;
    }

    SDL_GL_SetSwapInterval(1);

    // DataLoader& dataLoader = DataLoader::getInstance();
    // const TopographyVertices& topVertices = dataLoader.processTopographyVertices("tests/test_data/depth_readings/KinectDepthData-04-05-49.csv"); // Flat
    // const TopographyVertices& topVertices = dataLoader.processTopographyVertices("tests/test_data/depth_readings/KinectDepthData-04-08-15.csv"); // Single ramp
    // const TopographyVertices& topVertices = dataLoader.processTopographyVertices("tests/test_data/depth_readings/KinectDepthData-04-11-03.csv"); // Single mountain
    // const TopographyVertices& topVertices = dataLoader.processTopographyVertices("tests/test_data/depth_readings/KinectDepthData-04-13-00.csv"); // Double mountain

    renderer = std::make_unique<Renderer>(DataLoader::DEPTH_WIDTH, DataLoader::DEPTH_HEIGHT);
    renderer->initVertexObjects();

    waveManager.initSystems();

    kinect.initialize();
}

void MainGame::run() {
    initSystems();

    glm::vec2 basePosition = waveManager.getBase()->getSprite().getPosition();
    glm::vec2 windowSize = glm::vec2{WINDOW_WIDTH, WINDOW_HEIGHT};

    waveManager.addEnemy(EnemyType::Worm, {100, 300}, waveManager.getBase());
    waveManager.addEnemy(EnemyType::Beetle, {1100, 750}, waveManager.getBase());
    // waveManager.addEnemy(EnemyType::Bee, {300, 400}, waveManager.getBase());

    waveManager.addTower(TowerType::Sprayer, {400, 400});
    waveManager.addTower(TowerType::Mortar, {400, 400});
    waveManager.addTower(TowerType::Frog, {300, 400});

    float prevTicks = SDL_GetTicks();

    // TODO move this inside the loop using multithreading
    //DataLoader& dataLoader = DataLoader::getInstance();
    // const TopographyVertices& topVertices = dataLoader.processTopographyVertices("tests/test_data/depth_readings/KinectDepthData-04-11-03.csv"); // Single mountain
    // const TopographyVertices& topVertices = dataLoader.processTopographyVertices("tests/test_data/depth_readings/KinectDepthData-04-08-15.csv"); // Single ramp
    //const TopographyVertices& topVertices = dataLoader.processTopographyVertices("tests/test_data/depth_readings/KinectDepthData-04-13-00.csv"); // Double mountain

    while (!gameQuit) {

        InputManager& inputManager = InputManager::getInstance();
        InputResult inputResult = inputManager.processInput();
        if (inputResult == InputResult::QUIT) {
            gameQuit = true;
        }

        float currentTicks = SDL_GetTicks();
        float dt = currentTicks - prevTicks;
        prevTicks = currentTicks;

		kinect.update();
        const TopographyVertices& topVertices = kinect.getDepthData();
		const std::vector<std::uint8_t>& colorMat = kinect.getColorData();

        renderer->clearBuffer();

#ifdef DEBUG_MODE
            // renderer->DEBUG_rengerMat(Vision::findHills(topVertices));
#else

        renderer->renderTopography(topVertices);

        waveManager.update(topVertices, colorMat, dt / 1000.f);
        renderer->streamBase(waveManager.getBase());
        renderer->streamEnemies(waveManager.getEnemies());
        renderer->streamProjectiles(waveManager.getProjectiles());
        renderer->streamTowerData(waveManager.getTowers());
        renderer->renderSprites();
        renderer->renderHealthBar(
            float(waveManager.getBase()->getHealth()) / waveManager.getBase()->getMaxHealth(),
            basePosition / windowSize,
            waveManager.getBase()->getVertOffset() / WINDOW_HEIGHT);
#endif

        SDL_GL_SwapWindow(window);


        // std::cout << calculateFPS() << std::endl;
    }
}

float MainGame::calculateFPS() {
    static const int NUM_SAMPLES = 10;
    static float frames[NUM_SAMPLES];
    static int currentFrame = 0;
    static float prevTicks = SDL_GetTicks();
    static float tickSums = 0;

    float currentTicks = SDL_GetTicks();

    tickSums -= frames[currentFrame];
    frames[currentFrame] = currentTicks - prevTicks;
    tickSums += frames[currentFrame];

    prevTicks = currentTicks;
    currentFrame = (currentFrame + 1) % NUM_SAMPLES;

    return 1000 / (tickSums / NUM_SAMPLES);
}
