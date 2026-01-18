//
// Created by Sami Sharif  on 1/3/26.
//

#pragma once

#include <opencv2/opencv.hpp>

#include "DataLoader.hpp"
#include "glm/detail/type_mat.hpp"
#include "glm/vec2.hpp"
#include "engine/KinectSensor.hpp"
#include "WaveManager.hpp"

namespace Vision {
    static const std::array<cv::Point2f, 4> outputPointsNorm = {
        {
            {0.f, 0.f},
            {1.f, 0.f},
            {0.f, 1.f},
            {1.f, 1.f}
        }
    };

    //static const std::array<cv::Point2f, 4> inputPointsNorm = {
    //    {
    //        {0.25f, 0.03f},     // Top left
    //        {0.9f, 0.03f},      // Top right
    //        {0.25f, 0.604f},    // Bottom left
    //        {0.9f, 0.604f}      // Bottom right
    //    }
    //};
    static const std::array<cv::Point2f, 4> inputPointsNorm = {
        {
            {0.3f, 0.14f},
            {0.86f, 0.16f},
            {0.25f, 0.64f},
            {0.88f, 0.7f}
        }
    };

    static const std::array<cv::Point2f, 4> inputColorPointsNorm = {
        {
            {0.4f, 0.1f},
            {0.8f, 0.12f},
            {0.36f, 0.68f},
            {0.81f, 0.7f}
        }
    };

    static constexpr int MAX_DEPTH_VAL = 1300;
    static constexpr int MIN_DEPTH_VAL = 800;
    static constexpr float BOTTOM_SCALE = 1.09f;

    glm::mat3 calculateWarpMatrix();

    glm::vec2 cartesianToNDC(const glm::vec2 point);

    struct DetectedTower {
        TowerType type;
        glm::vec2 position; // Normalized (0.0 - 1.0)
    };

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
		void calculateColorWarpMatrix(const std::vector<std::uint8_t>& colorMat);
        void evaluateHills();

        std::vector<glm::vec2>& getHills() { return hills; };
        const cv::Mat& getWarpedDepth() { return warpedDepth; };
        const cv::Mat& getWarpedColor() { return warpedColor; };
        std::vector<DetectedTower> detectTowers();
        cv::Mat getHillMask();
        void applyDepthGradient();
        cv::Mat DEBUG_showDetectedTowers();
        cv::Mat DEBUG_showHills();
    private:
        Manager() = default;
        ~Manager() = default;

        static inline const cv::Size dsize{DataLoader::DEPTH_WIDTH, DataLoader::DEPTH_HEIGHT};
        static inline const cv::Size dsizeColor{KinectSensor::COLOR_WIDTH, KinectSensor::COLOR_HEIGHT};

        std::vector<glm::vec2> hills{};
        cv::Mat warpedDepth;
        cv::Mat warpedColor;
    };
}
