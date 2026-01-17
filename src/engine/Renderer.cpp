//
// Created by Sami Sharif  on 12/23/25.
//

#include "engine/Renderer.hpp"

#include <ostream>
#include <SDL2/SDL_video.h>

#include "engine/VisionManager.hpp"


Renderer::Renderer(const int depthWidth, const int depthHeight)
    : topShader(nullptr)
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

void Renderer::initVertexObjects() {
    // topVertices = topographyVertices;

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
        sizeof(TopographyVertex) * DataLoader::DEPTH_WIDTH * DataLoader::DEPTH_HEIGHT,
        nullptr,
        GL_DYNAMIC_DRAW
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

void Renderer::renderTopography(const TopographyVertices& topVertices, bool makeTransparent) {
    topShader->use();
    if (makeTransparent)
        topShader->setFloat("transparency", 0.2f);
    else
        topShader->setFloat("transparency", 1.0f);
    // markerShader->use();
    glBindVertexArray(topVAO);

    glBindBuffer(GL_ARRAY_BUFFER, topVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TopographyVertex) * topVertices.size(), topVertices.data());

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
        if (!enemy->isActive()) continue;
        auto sprite = enemy->getSprite();
        auto type = sprite.getType();
        sprite.pushVertices(spriteBatches[type].vertices);
        spriteBatches[type].textureID = sprite.textureID;
    }
}

void Renderer::streamProjectiles(const std::vector<std::unique_ptr<Projectile>>& projectiles) {
    // Clear batches
    for (auto& [type, batch] : spriteBatches) {
        if (isProjectile(type)) {
            batch.vertices.clear();
        }
    }

    // Push vertices into correct batch
    for (const auto& projectile : projectiles) {
        if (!projectile->isActive()) continue;
        auto sprite = projectile->getSprite();
        auto type = sprite.getType();
        sprite.pushVertices(spriteBatches[type].vertices);
        spriteBatches[type].textureID = sprite.textureID;
    }
}

void Renderer::streamTowerData(const std::vector<std::unique_ptr<Tower>>& towers) {
    // Clear batches
    for (auto& [type, batch] : spriteBatches) {
        if (isTowerData(type)) {
            batch.vertices.clear();
        }
    }

    // Push vertices into correct batch
    for (const auto& tower : towers) {
        auto sprite = tower->getAttackSprite();
        if (!sprite.getVisible()) continue;
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
    int vertexByteOffset = 0;
    int currentVertexCount = 0;
    for (auto& [type, batch] : spriteBatches) {
        if (batch.vertices.empty()) continue;

        int numVertices = batch.vertices.size();
        int batchByteSize = sizeof(SpriteVertex) * numVertices;

        glBufferSubData(GL_ARRAY_BUFFER, vertexByteOffset, batchByteSize, batch.vertices.data());

        int numIndicesToDraw = (numVertices / vertsPerSprite) * indicesPerSprite;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, batch.textureID);
        glDrawElementsBaseVertex(GL_TRIANGLES, numIndicesToDraw, GL_UNSIGNED_INT, nullptr, currentVertexCount);

        vertexByteOffset += batchByteSize;
        currentVertexCount += numVertices;
    }

    glBindVertexArray(0);
}

void Renderer::renderHealthBar(float percent, glm::vec2 position, float vertOffset) {
    // 1. Safety Clamp (0.0 to 1.0)
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 1.0f) percent = 1.0f;

    // --- Static Resources (Initialized only once) ---
    static GLuint uiProgram = 0;
    static GLuint uiVAO = 0;

    if (uiProgram == 0) {
        // A. Create Empty VAO (Required for Core Profile, even if not using VBOs)
        glGenVertexArrays(1, &uiVAO);

        // B. Define Simple Shader for Colored Rectangles
        const char* vsSrc = R"(
            #version 330 core
            uniform vec2 uPos;
            uniform vec2 uSize;

            void main() {
                // Generate a 0.0 to 1.0 quad using VertexID
                // 0:(0,0), 1:(1,0), 2:(0,1), 3:(1,1)
                vec2 verts[4] = vec2[](vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1));
                vec2 v = verts[gl_VertexID];

                // Scale and Translate to NDC coordinates (-1 to 1)
                vec2 finalPos = uPos + (v * uSize);
                gl_Position = vec4(finalPos, 0.0, 1.0);
            }
        )";

        const char* fsSrc = R"(
            #version 330 core
            out vec4 FragColor;
            uniform vec4 uColor;

            void main() {
                FragColor = uColor;
            }
        )";

        // C. Compile and Link
        auto compile = [](GLenum type, const char* src) {
            GLuint s = glCreateShader(type);
            glShaderSource(s, 1, &src, nullptr);
            glCompileShader(s);
            return s;
        };
        GLuint vs = compile(GL_VERTEX_SHADER, vsSrc);
        GLuint fs = compile(GL_FRAGMENT_SHADER, fsSrc);
        uiProgram = glCreateProgram();
        glAttachShader(uiProgram, vs);
        glAttachShader(uiProgram, fs);
        glLinkProgram(uiProgram);
        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    // --- RENDER STATE ---
    glUseProgram(uiProgram);
    glBindVertexArray(uiVAO);

    // Save previous state just in case
    GLboolean wasDepthEnabled = glIsEnabled(GL_DEPTH_TEST);

    // Disable depth so UI draws on top of everything
    glDisable(GL_DEPTH_TEST);
    // Also disable blending if you don't need transparency, to avoid mixing with game world
    // glEnable(GL_BLEND);

    // --- Settings ---
    float fullWidth = 0.3f;
    float height = 0.03f;
    float startX = (position.x * 2) - 1 - fullWidth / 2;
    float startY = 1 - (position.y * 2) + vertOffset;

    // 1. Draw Background (Black)
    glUniform2f(glGetUniformLocation(uiProgram, "uPos"), startX, startY);
    glUniform2f(glGetUniformLocation(uiProgram, "uSize"), fullWidth, height);
    glUniform4f(glGetUniformLocation(uiProgram, "uColor"), 0.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 2. Draw Foreground (Green)
    if (percent > 0.0f) {
        glUniform2f(glGetUniformLocation(uiProgram, "uSize"), fullWidth * percent, height);
        glUniform4f(glGetUniformLocation(uiProgram, "uColor"), 0.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // --- CLEANUP ---
    glBindVertexArray(0);
    glUseProgram(0);

    // RESTORE Depth Test for the next frame's geometry
    if (wasDepthEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
}

void Renderer::DEBUG_rengerMat(const cv::Mat& inputMat) {
    if (inputMat.empty()) return;

    // --- 1. Static Resources (Initialized Once) ---
    static GLuint debugTex = 0;
    static GLuint debugVAO = 0;
    static GLuint debugShader = 0;

    if (debugTex == 0) {
        // A. Create Texture
        glGenTextures(1, &debugTex);
        glBindTexture(GL_TEXTURE_2D, debugTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // B. Create Empty VAO (We use gl_VertexID to generate coords, no VBO needed)
        glGenVertexArrays(1, &debugVAO);

        // C. Create Simple Shader
        const char* vsSrc = R"(
            #version 330 core
            out vec2 TexCoord;
            void main() {
                // Generate a full screen triangle strip
                // ID: 0 -> (-1, -1), 1 -> (1, -1), 2 -> (-1, 1), 3 -> (1, 1)
                vec2 verts[4] = vec2[](vec2(-1, -1), vec2(1, -1), vec2(-1, 1), vec2(1, 1));
                // Flip Y for OpenCV (OpenCV 0,0 is Top-Left, OpenGL is Bottom-Left)
                vec2 uvs[4]   = vec2[](vec2(0, 1),   vec2(1, 1),   vec2(0, 0),   vec2(1, 0));

                gl_Position = vec4(verts[gl_VertexID], 0.0, 1.0);
                TexCoord = uvs[gl_VertexID];
            }
        )";

        const char* fsSrc = R"(
            #version 330 core
            in vec2 TexCoord;
            out vec4 FragColor;
            uniform sampler2D screenTexture;
            void main() {
                FragColor = texture(screenTexture, TexCoord);
                // Optional: Visualize Red channel as Grayscale if input is 1-channel
                // FragColor = vec4(vec3(texture(screenTexture, TexCoord).r), 1.0);
            }
        )";

        auto compile = [](GLenum type, const char* src) {
            GLuint s = glCreateShader(type);
            glShaderSource(s, 1, &src, nullptr);
            glCompileShader(s);
            // (Add error checking here if needed)
            return s;
        };
        GLuint vs = compile(GL_VERTEX_SHADER, vsSrc);
        GLuint fs = compile(GL_FRAGMENT_SHADER, fsSrc);
        debugShader = glCreateProgram();
        glAttachShader(debugShader, vs);
        glAttachShader(debugShader, fs);
        glLinkProgram(debugShader);
        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    // --- 2. Prepare Data ---
    // OpenCV rows might not be 4-byte aligned. Tell OpenGL to read byte-by-byte.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLenum format = GL_BGR; // Standard OpenCV color format
    GLenum internalFormat = GL_RGB;

    if (inputMat.channels() == 1) {
        format = GL_RED;
        internalFormat = GL_RED;
        // Or use GL_LUMINANCE for legacy / swizzle mask for modern to make it look B&W
        GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    } else {
        // Reset swizzle for color images
        GLint swizzleMask[] = {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    }

    // --- 3. Upload & Draw ---
    glUseProgram(debugShader);

    // Bind Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, debugTex);

    // Upload Mat Data
    // Note: Re-allocating every frame is fine for debugging, but for prod use glTexSubImage2D
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, inputMat.cols, inputMat.rows,
                 0, format, GL_UNSIGNED_BYTE, inputMat.data);

    glBindVertexArray(debugVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Cleanup state
    glBindVertexArray(0);
    glUseProgram(0);
}