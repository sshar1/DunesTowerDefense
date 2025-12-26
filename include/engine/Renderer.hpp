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

    static constexpr std::uint16_t TALLEST_DEPTH = 750;
    static constexpr std::uint16_t SHORTEST_DEPTH = 1000;

    GLuint topVAO = 0;

    // Topography
    const TopographyVertices* topVertices;
    std::unique_ptr<Shader> topShader;
    const std::unique_ptr<std::vector<GLuint>> topIndices;
};
