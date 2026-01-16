//
// Created by Sami Sharif  on 12/23/25.
//

#include "MainGame.hpp"

#include <SDL2/SDL_image.h>

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
}

void MainGame::run() {
    initSystems();

    glm::vec2 basePosition = waveManager.getBase()->getSprite().getPosition();

    waveManager.addEnemy(EnemyType::Worm, {100, 200}, basePosition);
    waveManager.addEnemy(EnemyType::Beetle, {1100, 750}, basePosition);
    waveManager.addEnemy(EnemyType::Bee, {300, 400}, basePosition);

    waveManager.addTower(TowerType::Sprayer, {500, 300});

    float prevTicks = SDL_GetTicks();

    // TODO move this inside the loop using multithreading
    DataLoader& dataLoader = DataLoader::getInstance();
    // const TopographyVertices& topVertices = dataLoader.processTopographyVertices("tests/test_data/depth_readings/KinectDepthData-04-11-03.csv"); // Single mountain
    // const TopographyVertices& topVertices = dataLoader.processTopographyVertices("tests/test_data/depth_readings/KinectDepthData-04-08-15.csv"); // Single ramp
    const TopographyVertices& topVertices = dataLoader.processTopographyVertices("tests/test_data/depth_readings/KinectDepthData-04-13-00.csv"); // Double mountain

    cv::VideoCapture cap(0); 
    cv::Mat frame;
    Vision::Manager& visionManager = Vision::Manager::getInstance();

    while (!gameQuit) {

        cap >> frame;
        

        InputManager& inputManager = InputManager::getInstance();
        InputResult inputResult = inputManager.processInput();
        if (inputResult == InputResult::QUIT) {
            gameQuit = true;
        }

        float currentTicks = SDL_GetTicks();
        float dt = currentTicks - prevTicks;
        prevTicks = currentTicks;

        renderer->clearBuffer();

// #ifdef DEBUG_MODE
            renderer->DEBUG_rengerMat(frame);
            std::vector<Vision::DetectedTower> towers = visionManager.detectTowers(frame);

            // cv::Mat cyanMask = visionManager.detectTowers(frame);
            // renderer->DEBUG_rengerMat(cyanMask);

            //std::vector<Vision::DetectedTower> towers = visionManager.detectTowers(frame);
            
            // cv::Mat debugFrame = frame.clone();
            
            // // Draw detected towers on the frame
            // for (const auto& tower : towers) {
            //     // Convert from window coordinates back to image coordinates
            //     float imgX = tower.screenPos.x * frame.cols / WINDOW_WIDTH;
            //     float imgY = tower.screenPos.y * frame.rows / WINDOW_HEIGHT;
            //     cv::Point center(static_cast<int>(imgX), static_cast<int>(imgY));
                
            //     // Choose color and label based on tower type
            //     cv::Scalar color;
            //     std::string label;
                
            //     if (tower.type == Vision::TowerType::CYAN) {
            //         color = cv::Scalar(255, 255, 0);  // Yellow (BGR)
            //         label = "CYAN";
            //     } else if (tower.type == Vision::TowerType::GRAY) {
            //         color = cv::Scalar(128, 128, 128); // Gray
            //         label = "GRAY";
            //     } else { // BLACK
            //         color = cv::Scalar(255, 255, 255); // White circle so it shows up
            //         label = "BLACK";
            //     }
                
            //     // Draw circle around detected tower
            //     cv::circle(debugFrame, center, 20, color, 3);
                
            //     // Draw label
            //     cv::putText(debugFrame, label, 
            //             cv::Point(center.x + 25, center.y - 10),
            //             cv::FONT_HERSHEY_SIMPLEX, 0.7, color, 2);
                
            //     // Draw crosshair at center
            //     cv::line(debugFrame, cv::Point(center.x - 10, center.y), 
            //             cv::Point(center.x + 10, center.y), color, 2);
            //     cv::line(debugFrame, cv::Point(center.x, center.y - 10), 
            //             cv::Point(center.x, center.y + 10), color, 2);
            // }
            
            // // Add tower count overlay
            // std::string countText = "Towers Detected: " + std::to_string(towers.size());
            // cv::putText(debugFrame, countText, cv::Point(10, 30),
            //         cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
            
            // // Print to console for debugging
            // if (towers.size() > 0) {
            //     std::cout << "Found " << towers.size() << " towers: ";
            //     for (const auto& t : towers) {
            //         std::cout << (t.type == Vision::TowerType::CYAN ? "CYAN " :
            //                     t.type == Vision::TowerType::GRAY ? "GRAY " : "BLACK ");
            //         std::cout << "@ (" << t.screenPos.x << "," << t.screenPos.y << ") ";
            //     }
            //     std::cout << std::endl;
            // }


            // renderer->clearBuffer();

            // // Render the debug frame with tower detection overlay
            // renderer->DEBUG_rengerMat(debugFrame);
// #else 
        // renderer->renderTopography(topVertices);

        // waveManager.update(topVertices, dt / 1000.f);
        // renderer->streamEnemies(waveManager.getEnemies());
        // renderer->streamProjectiles(waveManager.getProjectiles());
        // renderer->streamTowerData(waveManager.getTowers());
        // renderer->streamBase(waveManager.getBase());
        // renderer->renderSprites();
// #endif

        SDL_GL_SwapWindow(window);

        // std::cout << calculateFPS() << std::endl;
    }
    cap.release();
    //cv::destroyAllWindows();
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
