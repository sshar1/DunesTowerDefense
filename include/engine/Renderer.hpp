//
// Created by Sami Sharif  on 12/23/25.
//

#pragma once

#include <map>

#include "DataLoader.hpp"
#include "shader_s.h"
#include "entities/Enemy.hpp"
#include "entities/Sprite.hpp"

struct SpriteBatch {
    GLuint textureID;
    std::vector<SpriteVertex> vertices;
};

class Renderer {
public:
    Renderer(int depthWidth, int depthHeight);
    ~Renderer() = default;

    // TODO temporarily take a vector of data from kinect and render it
    void renderTopography();
    void initVertexObjects(const TopographyVertices* topographyVertices);
    void clearBuffer();

    // For debugging only. In release, always use renderSpriteBatch
    void renderSprite(const Sprite& sprite);

    void streamEnemies(const std::vector<std::unique_ptr<Enemy>>& enemies);
    void renderSprites();

private:
    static constexpr const char* TOP_VERT_PATH = "data/shaders/topology.vert";
    static constexpr const char* TOP_FRAG_PATH = "data/shaders/topology.frag";

    static constexpr const char* SPRITE_VERT_PATH = "data/shaders/sprite.vert";
    static constexpr const char* SPRITE_FRAG_PATH = "data/shaders/sprite.frag";

    static constexpr std::uint16_t TALLEST_DEPTH = 750;
    static constexpr std::uint16_t MID_DEPTH = 830;
    static constexpr std::uint16_t SHORTEST_DEPTH = 960;

    GLuint topVAO{};
    GLuint topVBO{};
    GLuint spriteVAO{};
    GLuint spriteVBO{};

    // Topography
    const TopographyVertices* topVertices;
    std::unique_ptr<Shader> topShader;
    static constexpr int TOP_INDICES_LEN = 6 * (DataLoader::DEPTH_WIDTH - 1) * (DataLoader::DEPTH_HEIGHT - 1);

    // Sprites
    const SpriteVertices* spriteVertices;
    std::unique_ptr<Shader> spriteShader;

    static constexpr int MAX_SPRITES = 1000;
    static constexpr int MAX_SPRITE_VERTICES = MAX_SPRITES * 4;
    static constexpr int MAX_SPRITE_INDICES = MAX_SPRITES * 6;

    std::map<SpriteType, SpriteBatch> spriteBatches;
};
