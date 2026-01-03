//
// Created by Sami Sharif  on 1/3/26.
//

#pragma once

#include <opencv2/opencv.hpp>

#include "glm/detail/type_mat.hpp"

namespace Vision {
    static const std::array<cv::Point2f, 4> outputPoints = {
        {
            {0.f, 0.f},
            {1.f, 0.f},
            {0.f, 1.f},
            {1.f, 1.f}
        }
    };

    static const std::array<cv::Point2f, 4> inputPoints = {
        {
            {0.25f, 0.03f},     // Top left
            {0.9f, 0.03f},      // Top right
            {0.25f, 0.604f},    // Bottom left
            {0.9f, 0.604f}      // Bottom right
        }
    };

    glm::mat3 calculateWarpMatrix();
}
