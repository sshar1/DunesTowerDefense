//
// Created by Sami Sharif  on 1/3/26.
//

#pragma once

#include <opencv2/opencv.hpp>

#include "DataLoader.hpp"
#include "glm/detail/type_mat.hpp"
#include "glm/detail/type_vec.hpp"

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

    static constexpr int MAX_DEPTH_VAL = 1500;
    static constexpr int MIN_DEPTH_VAL = 500;

    glm::mat3 calculateWarpMatrix();
    // std::vector<glm::vec2> findHills(const TopographyVertices& topVertices);
    cv::Mat findHills(const TopographyVertices& topVertices);
}
