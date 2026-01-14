//
// Created by Sami Sharif  on 1/3/26.
//

#pragma once

#include <opencv2/opencv.hpp>

#include "DataLoader.hpp"
#include "glm/detail/type_mat.hpp"
#include "glm/vec2.hpp"

namespace Vision {
    static const std::array<cv::Point2f, 4> outputPointsNorm = {
        {
            {0.f, 0.f},
            {1.f, 0.f},
            {0.f, 1.f},
            {1.f, 1.f}
        }
    };

    static const std::array<cv::Point2f, 4> inputPointsNorm = {
        {
            {0.25f, 0.03f},     // Top left
            {0.9f, 0.03f},      // Top right
            {0.25f, 0.604f},    // Bottom left
            {0.9f, 0.604f}      // Bottom right
        }
    };

    enum class TowerType {
        CYAN,
        GRAY,
        UNKNOWN
    };

    struct DetectedTower {
        TowerType type;
        glm::vec2 screenPos; // window coordinates
    };


    static constexpr int MAX_DEPTH_VAL = 1500;
    static constexpr int MIN_DEPTH_VAL = 500;

    glm::mat3 calculateWarpMatrix();

    glm::vec2 cartesianToNDC(const glm::vec2 point);

    class Manager {
    public:
        Manager(Manager const&) = delete;
        Manager& operator= (Manager const&) = delete;
        Manager(Manager const&&) = delete;
        Manager& operator= (Manager const&&) = delete;

        static Manager& getInstance() {
            static Manager instance;
            return instance;
        }

        void calculateWarpMatrix(const TopographyVertices& topVertices);
        void evaluateHills();
        
        std::vector<DetectedTower> detectTowers(const cv::Mat& colorImg);
        std::vector<glm::vec2>& getHills() { return hills; };
        const cv::Mat& getWarpedDepth() { return warpedDepth; };
    private:
        Manager() = default;
        ~Manager() = default;

        static inline const cv::Size dsize{DataLoader::DEPTH_WIDTH, DataLoader::DEPTH_HEIGHT};

        std::vector<glm::vec2> hills{};
        cv::Mat warpedDepth;
    };
}
