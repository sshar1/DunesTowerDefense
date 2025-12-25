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
{
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
        // std::cout << "Failed to initialize GLAD" << std::endl;
    }

    topShader = std::make_unique<Shader>(TOP_VERT_PATH, TOP_FRAG_PATH);
    topShader->use();
    topShader->setInt("gridWidth", depthWidth);
    topShader->setInt("gridHeight", depthHeight);
}

void Renderer::initVertexObjects(const TopographyVertices* topographyVertices) {
    topVertices = topographyVertices;

    GLuint topVBO;
    glGenVertexArrays(1, &topVAO);
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
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::clearBuffer() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::renderTopography() {
    topShader->use();
    glBindVertexArray(topVAO);
    glDrawArrays(GL_POINTS, 0, topVertices->size());
    glBindVertexArray(0);
}
