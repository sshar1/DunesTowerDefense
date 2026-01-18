//
// Created by Sami Sharif  on 12/23/25.
//

#pragma once

#include <map>
#include <opencv2/core/mat.hpp>

#include "DataLoader.hpp"
#include "shader_s.h"
#include "entities/Base.hpp"
#include "entities/Enemy.hpp"
#include "../entities/projectiles/Projectile.hpp"
#include "entities/Sprite.hpp"
#include "../entities/towers/Tower.hpp"

struct SpriteBatch {
    GLuint textureID;
    std::vector<SpriteVertex> vertices;
};

class Renderer {
public:
    Renderer(int depthWidth, int depthHeight);
    ~Renderer() = default;

    // TODO temporarily take a vector of data from kinect and render it
    void renderTopography(const TopographyVertices& topVertices, bool makeTransparent);
    void initVertexObjects();
    void clearBuffer();

    // For debugging only. In release, always use renderSpriteBatch
    void renderSprite(const Sprite& sprite);

    void streamEnemies(const std::vector<std::unique_ptr<Enemy>>& enemies);
    void streamProjectiles(const std::vector<std::unique_ptr<Projectile>>& projectiles);
    void streamTowerData(const std::vector<std::unique_ptr<Tower>>& towers);
    void streamBase(const Base* base);
    void renderSprites();
    void renderHealthBar(float percent, glm::vec2 position, float vertOffset);
    void DEBUG_rengerMat(const cv::Mat& inputMat);
    void renderHUD(unsigned int waveNumber, unsigned int totalWaves,
                   bool isPreWave, unsigned int towersRemaining, unsigned int towerAllowance,
                   bool isGameOver, bool isVictory);

private:
    static constexpr const char* TOP_VERT_PATH = "data/shaders/topology.vert";
    static constexpr const char* TOP_FRAG_PATH = "data/shaders/topology.frag";

    static constexpr const char* MARKER_VERT_PATH = "data/shaders/box_marker.vert";
    static constexpr const char* MARKER_FRAG_PATH = "data/shaders/box_marker.frag";

    static constexpr const char* SPRITE_VERT_PATH = "data/shaders/sprite.vert";
    static constexpr const char* SPRITE_FRAG_PATH = "data/shaders/sprite.frag";

    static constexpr std::uint16_t TALLEST_DEPTH = 850;
    static constexpr std::uint16_t SHORTEST_DEPTH = 1010;
    static constexpr float IDLE_TRANSPARENCY = 0.1f;

    static constexpr std::uint16_t BOX_DEPTH = 750;

    GLuint topVAO{};
    GLuint topVBO{};
    GLuint spriteVAO{};
    GLuint spriteVBO{};

    // Topography
    std::unique_ptr<Shader> topShader;
    static constexpr int TOP_INDICES_LEN = 6 * (DataLoader::DEPTH_WIDTH - 1) * (DataLoader::DEPTH_HEIGHT - 1);

    // Vision marker
    std::unique_ptr<Shader> markerShader;

    // Sprites
    const SpriteVertices* spriteVertices;
    std::unique_ptr<Shader> spriteShader;

    static constexpr int MAX_SPRITES = 1000;
    static constexpr int MAX_SPRITE_VERTICES = MAX_SPRITES * 4;
    static constexpr int MAX_SPRITE_INDICES = MAX_SPRITES * 6;

    std::map<SpriteType, SpriteBatch> spriteBatches;
};
