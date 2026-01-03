//
// Created by Sami Sharif  on 12/23/25.
//

#include "engine/Renderer.hpp"

#include <ostream>
#include <SDL2/SDL_video.h>

#include "engine/VisionManager.hpp"


Renderer::Renderer(const int depthWidth, const int depthHeight)
    : topVertices(nullptr)
    , topShader(nullptr)
    , markerShader(nullptr)
    , spriteVertices(nullptr)
    , spriteShader(nullptr)
{
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    topShader = std::make_unique<Shader>(TOP_VERT_PATH, TOP_FRAG_PATH);
    topShader->use();
    topShader->setInt("gridWidth", depthWidth);
    topShader->setInt("gridHeight", depthHeight);
    topShader->setInt("tallestDepth", TALLEST_DEPTH);
    topShader->setInt("midDepth", MID_DEPTH);
    topShader->setInt("shortestDepth", SHORTEST_DEPTH);
    topShader->setMat3("warpMatrix", Vision::calculateWarpMatrix());

    markerShader = std::make_unique<Shader>(MARKER_VERT_PATH, MARKER_FRAG_PATH);
    markerShader->use();
    markerShader->setInt("gridWidth", depthWidth);
    markerShader->setInt("gridHeight", depthHeight);
    markerShader->setInt("boxDepth", BOX_DEPTH);

    spriteShader = std::make_unique<Shader>(SPRITE_VERT_PATH, SPRITE_FRAG_PATH);
    spriteShader->use();
    spriteShader->setInt("tex", 0);
}

void Renderer::initVertexObjects(const TopographyVertices* topographyVertices) {
    topVertices = topographyVertices;

    GLuint topEBO;
    glGenVertexArrays(1, &topVAO);
    glGenBuffers(1, &topEBO);
    glGenBuffers(1, &topVBO);

    GLuint spriteEBO;
    glGenVertexArrays(1, &spriteVAO);
    glGenBuffers(1, &spriteEBO);
    glGenBuffers(1, &spriteVBO);

    /* Topography VAO */
    glBindVertexArray(topVAO);

    glBindBuffer(GL_ARRAY_BUFFER, topVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(TopographyVertex) * topographyVertices->size(),
        topographyVertices->data(),
        GL_STATIC_DRAW
    );
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_SHORT, sizeof(TopographyVertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Create indices array for topography
    std::vector<GLuint> topIndices;
    topIndices.reserve(TOP_INDICES_LEN);
    for (int i = 0; i < DataLoader::DEPTH_HEIGHT - 1; i++) {
        for (int j = 0; j < DataLoader::DEPTH_WIDTH - 1; j++) {
            topIndices.push_back(i * DataLoader::DEPTH_WIDTH + j);                 // Top left
            topIndices.push_back(i * DataLoader::DEPTH_WIDTH + (j + 1));           // Top right
            topIndices.push_back((i + 1) * DataLoader::DEPTH_WIDTH + j);           // Bottom left

            topIndices.push_back(i * DataLoader::DEPTH_WIDTH + (j + 1));           // Top right
            topIndices.push_back((i + 1) * DataLoader::DEPTH_WIDTH + j);           // Bottom left
            topIndices.push_back((i + 1) * DataLoader::DEPTH_WIDTH + (j + 1));     // Bottom right
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * topIndices.size(),
        topIndices.data(),
        GL_STATIC_DRAW
    );
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    /* Sprite VAO */
    glBindVertexArray(spriteVAO);

    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(SpriteVertex) * MAX_SPRITE_VERTICES,
        nullptr,
        GL_DYNAMIC_DRAW
    );
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)offsetof(SpriteVertex, u));
    glEnableVertexAttribArray(1);

    // Create indices array for sprites
    std::vector<GLuint> spriteIndices(MAX_SPRITE_INDICES);
    int offset = 0;
    for (int i = 0; i < MAX_SPRITE_INDICES; i += 6) {
        spriteIndices[i+0] = offset + 0;
        spriteIndices[i+1] = offset + 1;
        spriteIndices[i+2] = offset + 2;

        spriteIndices[i+3] = offset + 1;
        spriteIndices[i+4] = offset + 2;
        spriteIndices[i+5] = offset + 3;

        offset += 4;
    }


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * spriteIndices.size(),
        spriteIndices.data(),
        GL_STATIC_DRAW
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::clearBuffer() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::renderTopography() {
    topShader->use();
    // markerShader->use();
    glBindVertexArray(topVAO);

    glBindBuffer(GL_ARRAY_BUFFER, topVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TopographyVertex) * topVertices->size(), topVertices->data());

    glDrawElements(GL_TRIANGLES, TOP_INDICES_LEN, GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // glDrawArrays(GL_POINTS, 0, topVertices->size());
    glBindVertexArray(0);
}

void Renderer::renderSprite(const Sprite& sprite) {
    spriteShader->use();
    glBindVertexArray(spriteVAO);

    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);

    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITE_VERTICES * sizeof(SpriteVertex), nullptr, GL_DYNAMIC_DRAW);
    SpriteVertices vertices = sprite.getVertices();
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(SpriteVertices),
        &vertices
    );

    int numIndicesToDraw = 6;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite.textureID);
    glDrawElements(GL_TRIANGLES, numIndicesToDraw, GL_UNSIGNED_INT, nullptr);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
}

void Renderer::streamEnemies(const std::vector<std::unique_ptr<Enemy>>& enemies) {
    // Clear batches
    for (auto& [type, batch] : spriteBatches) {
        if (isEnemy(type)) {
            batch.vertices.clear();
        }
    }

    // Push vertices into correct batch
    for (const auto& enemy : enemies) {
        auto sprite = enemy->getSprite();
        auto type = sprite.getType();
        sprite.pushVertices(spriteBatches[type].vertices);
        spriteBatches[type].textureID = sprite.textureID;
    }
}

void Renderer::streamBase(const Base* base) {
    spriteBatches[SpriteType::Base].vertices.clear();

    auto sprite = base->getSprite();
    auto type = sprite.getType();
    sprite.pushVertices(spriteBatches[type].vertices);
    spriteBatches[type].textureID = sprite.textureID;
}

void Renderer::renderSprites() {
    static constexpr int vertsPerSprite = 4;
    static constexpr int indicesPerSprite = 6;

    spriteShader->use();
    glBindVertexArray(spriteVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);

    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITE_VERTICES * sizeof(SpriteVertex), nullptr, GL_DYNAMIC_DRAW);

    // Draw batches
    for (auto& [type, batch] : spriteBatches) {
        if (batch.vertices.empty()) continue;

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SpriteVertex) * batch.vertices.size(), batch.vertices.data());

        int numIndicesToDraw = (batch.vertices.size() / vertsPerSprite) * indicesPerSprite;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, batch.textureID);
        glDrawElements(GL_TRIANGLES, numIndicesToDraw, GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
}