//
// Created by Sami Sharif  on 12/23/25.
//

#pragma once

#include "DataLoader.hpp"
#include "shader_s.h"

class Renderer {
public:
    // static Renderer& getInstance() {
    //     static Renderer instance;
    //     return instance;
    // }
    //
    // Renderer(const Renderer&) = delete;
    // Renderer& operator=(const Renderer&) = delete;
    // Renderer(const Renderer&&) = delete;
    // Renderer& operator=(const Renderer&&) = delete;
    Renderer(int depthWidth, int depthHeight);
    ~Renderer() = default;

    // TODO temporarily take a vector of data from kinect and render it
    void renderTopography();
    void initVertexObjects(const TopographyVertices* topographyVertices);
    void clearBuffer();

private:
    // Renderer();
    // ~Renderer() = default;
    static constexpr const char* TOP_VERT_PATH = "data/shaders/topology.vert";
    static constexpr const char* TOP_FRAG_PATH = "data/shaders/topology.frag";

    GLuint topVAO = 0;

    const TopographyVertices* topVertices;

    // Topography
    std::unique_ptr<Shader> topShader;
};
