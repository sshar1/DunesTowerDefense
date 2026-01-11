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

    void Manager::evaluateHills() {
        static constexpr int hillThreshold = 200;
        static constexpr int minHillArea = 200;
        static const cv::Size kernelSize{5, 5};

        hills.clear();

        cv::Mat minMaxMask = (warpedDepth > 100);
        double minVal, maxVal;
        cv::minMaxLoc(warpedDepth, &minVal, &maxVal, nullptr, nullptr, minMaxMask);

        if (minVal < MIN_DEPTH_VAL) minVal = MIN_DEPTH_VAL;
        if (maxVal > MAX_DEPTH_VAL) maxVal = MAX_DEPTH_VAL;

        cv::Mat hillImg;
        cv::max(warpedDepth, minVal, hillImg);
        cv::min(hillImg, maxVal, hillImg);

        cv::normalize(hillImg, hillImg, 0, 255, cv::NORM_MINMAX, CV_8UC1);
        cv::threshold(hillImg, hillImg, hillThreshold, 255, cv::THRESH_BINARY_INV);

        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, kernelSize);
        cv::morphologyEx(hillImg, hillImg, cv::MORPH_OPEN, kernel);

        cv::copyMakeBorder(hillImg, hillImg, 1, 1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar(0));

        cv::Mat debugImg;
        cv::cvtColor(hillImg, debugImg, cv::COLOR_GRAY2BGR);
        cv::Scalar color(255, 0, 0);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(hillImg, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        for (int i = 0; i < contours.size(); i++) {
            const auto& contour = contours[i];
            cv::Rect bounds = cv::boundingRect(contour);

            bool touchesBorder = (bounds.x <= 1 || bounds.y <= 1 ||
                                bounds.x + bounds.width >= dsize.width - 2 ||
                                bounds.y + bounds.height >= dsize.height - 2);

            if (touchesBorder) continue;

            double area = cv::contourArea(contour);
            if (area > minHillArea) {
                cv::drawContours(debugImg, contours, i, color, 2);
                cv::Moments moment = cv::moments(contour);
                if (moment.m00 != 0) {
                    float centerX = moment.m10 / moment.m00;
                    float centerY = moment.m01 / moment.m00;

                    hills.emplace_back(centerX * MainGame::WINDOW_WIDTH / dsize.width, centerY * MainGame::WINDOW_HEIGHT / dsize.height);
                }
            }
        }
    }

    void Manager::calculateWarpMatrix(const TopographyVertices& topVertices) {
        static auto normToPixel = [](std::array<cv::Point2f, 4> normCoords) -> std::array<cv::Point2f, 4> {
            std::array<cv::Point2f, 4> pixelCoords;
            std::transform(normCoords.begin(), normCoords.end(), pixelCoords.begin(), [](cv::Point2f normPoint) {
                return cv::Point2f(DataLoader::DEPTH_WIDTH * normPoint.x, DataLoader::DEPTH_HEIGHT * normPoint.y);
            });

            return pixelCoords;
        };

        static const std::array<cv::Point2f, 4> outputPointsPixel = normToPixel(outputPointsNorm);
        static const std::array<cv::Point2f, 4> inputPointsPixel = normToPixel(inputPointsNorm);

        cv::Mat rawData = cv::Mat(DataLoader::DEPTH_HEIGHT, DataLoader::DEPTH_WIDTH, CV_16UC1, (void*)topVertices.data());

        cv::Mat warpMat = cv::getPerspectiveTransform(inputPointsPixel, outputPointsPixel);
        cv::warpPerspective(rawData, warpedDepth,warpMat, dsize);
    }

    glm::vec2 cartesianToNDC(const glm::vec2 point) {
        // Cartesian:
        // (0,0)---------(1,0)
        // ------------------
        // ------------------
        // (0,1)---------(1,1)

        // NDC:
        // (-1,1)---------(1,1)
        // ------------------
        // ------------------
        // (-1,-1)---------(1,-1)

        // x: [0,1] -> [-1, 1]
        // y: [0,1] -> [1, -1]

        return glm::vec2{point.x * 2 - 1, -(point.y * 2 - 1)};
    }
}