//
// Created by Sami Sharif  on 1/3/26.
//

#include "engine/VisionManager.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>

#include "MainGame.hpp"
#include "engine/DataLoader.hpp"
#include "glm/detail/type_mat3x3.hpp"

namespace Vision {
    glm::mat3 calculateWarpMatrix() {
        cv::Mat cvMat = cv::getPerspectiveTransform(inputPointsNorm, outputPointsNorm);

        glm::mat3 warpMat;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                warpMat[i][j] = cvMat.at<double>(i, j);
            }
        }

        return warpMat;
    }

    cv::Mat findHills(const TopographyVertices& topVertices) {
        static constexpr int hillThreshold = 200;
        static const cv::Size dsize{DataLoader::DEPTH_WIDTH, DataLoader::DEPTH_HEIGHT};

        static auto normToPixel = [](std::array<cv::Point2f, 4> normCoords) -> std::array<cv::Point2f, 4> {
            std::array<cv::Point2f, 4> pixelCoords;
            std::transform(normCoords.begin(), normCoords.end(), pixelCoords.begin(), [](cv::Point2f normPoint) {
                return cv::Point2f(DataLoader::DEPTH_WIDTH * normPoint.x, DataLoader::DEPTH_HEIGHT * normPoint.y);
            });

            return pixelCoords;
        };

        static const std::array<cv::Point2f, 4> outputPointsPixel = normToPixel(outputPointsNorm);
        static const std::array<cv::Point2f, 4> inputPointsPixel = normToPixel(inputPointsNorm);

        std::vector<glm::vec2> hills;

        cv::Mat rawData = cv::Mat(DataLoader::DEPTH_HEIGHT, DataLoader::DEPTH_WIDTH, CV_16UC1, (void*)topVertices.data());

        cv::Mat warped;
        cv::Mat warpMat = cv::getPerspectiveTransform(inputPointsPixel, outputPointsPixel);
        cv::warpPerspective(rawData, warped,warpMat, dsize);

        cv::Mat minMaxMask = (warped > 100);
        double minVal, maxVal;
        cv::minMaxLoc(warped, &minVal, &maxVal, nullptr, nullptr, minMaxMask);

        if (minVal < MIN_DEPTH_VAL) minVal = MIN_DEPTH_VAL;
        if (maxVal > MAX_DEPTH_VAL) maxVal = MAX_DEPTH_VAL;

        cv::Mat clampedData;
        cv::max(warped, minVal, clampedData);
        cv::min(clampedData, maxVal, clampedData);

        cv::Mat viewableImg;
        cv::normalize(clampedData, viewableImg, 0, 255, cv::NORM_MINMAX, CV_8UC1);

        cv::Mat masked;
        cv::threshold(viewableImg, masked, hillThreshold, 255, cv::THRESH_BINARY);

        return masked;
    }
}
