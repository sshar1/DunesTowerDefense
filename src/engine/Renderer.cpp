//
// Created by Sami Sharif  on 12/23/25.
//

#include "engine/Renderer.hpp"

#include <iostream>
#include <ostream>
#include <SDL2/SDL_video.h>


Renderer::Renderer(const int depthWidth, const int depthHeight)
    : topVertices(nullptr)
    , topShader(nullptr)
    , topIndices(std::make_unique<std::vector<GLuint>>())
{
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
        // std::cout << "Failed to initialize GLAD" << std::endl;
    }

    topShader = std::make_unique<Shader>(TOP_VERT_PATH, TOP_FRAG_PATH);
    topShader->use();
    topShader->setInt("gridWidth", depthWidth);
    topShader->setInt("gridHeight", depthHeight);

    // Create indices array for topography
    constexpr int topIndicesLen = 6 * (DataLoader::DEPTH_WIDTH - 1) * (DataLoader::DEPTH_HEIGHT - 1);
    topIndices->reserve(topIndicesLen);
    for (int i = 0; i < DataLoader::DEPTH_HEIGHT - 1; i++) {
        for (int j = 0; j < DataLoader::DEPTH_WIDTH - 1; j++) {
            topIndices->push_back(i * DataLoader::DEPTH_WIDTH + j);                 // Top left
            topIndices->push_back(i * DataLoader::DEPTH_WIDTH + (j + 1));           // Top right
            topIndices->push_back((i + 1) * DataLoader::DEPTH_WIDTH + j);           // Bottom left

            topIndices->push_back(i * DataLoader::DEPTH_WIDTH + (j + 1));           // Top right
            topIndices->push_back((i + 1) * DataLoader::DEPTH_WIDTH + j);           // Bottom left
            topIndices->push_back((i + 1) * DataLoader::DEPTH_WIDTH + (j + 1));     // Bottom right
        }
    }
}

void Renderer::initVertexObjects(const TopographyVertices* topographyVertices) {
    topVertices = topographyVertices;

    GLuint topVBO, topEBO;
    glGenVertexArrays(1, &topVAO);
    glGenBuffers(1, &topEBO);
    glGenBuffers(1, &topVBO);

    /* Topography VAO */
    glBindVertexArray(topVAO);

    glBindBuffer(GL_ARRAY_BUFFER, topVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(TopographyVertex) * topographyVertices->size(),
        topographyVertices->data(),
        GL_DYNAMIC_DRAW
    );
    glVertexAttribPointer(0, 1, GL_INT, GL_FALSE, sizeof(TopographyVertex), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * topIndices->size(),
        topIndices->data(),
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
    glBindVertexArray(topVAO);
    glDrawElements(GL_TRIANGLES, topIndices->size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
