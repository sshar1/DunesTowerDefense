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

    cv::Mat Manager::getHillMask() {
        static constexpr int hillThreshold = 150;
        static const cv::Size kernelSize{ 5, 5 };

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

        static cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, kernelSize);
        cv::morphologyEx(hillImg, hillImg, cv::MORPH_OPEN, kernel);
        cv::copyMakeBorder(hillImg, hillImg, 1, 1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar(0));

        cv::flip(hillImg, hillImg, 1);

        return hillImg;
    }

    void Manager::evaluateHills() {
        static constexpr int minHillArea = 200;

        hills.clear();

        cv::Mat hillMask = getHillMask();

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(hillMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (const auto& contour : contours) {
            double area = cv::contourArea(contour);
            if (area <= minHillArea) continue;

            cv::Rect bounds = cv::boundingRect(contour);

            bool touchesBorder = (bounds.x <= 1 || bounds.y <= 1 ||
                bounds.x + bounds.width >= hillMask.cols - 1 ||
                bounds.y + bounds.height >= hillMask.rows - 1);

            if (touchesBorder) continue;

            cv::Moments moment = cv::moments(contour);
            if (moment.m00 != 0) {
                float maskX = static_cast<float>(moment.m10 / moment.m00);
                float maskY = static_cast<float>(moment.m01 / moment.m00);

                float realX = maskX - 1.0f;
                float realY = maskY - 1.0f;

                float screenX = realX * MainGame::WINDOW_WIDTH / dsize.width;
                float screenY = realY * MainGame::WINDOW_HEIGHT / dsize.height;

                hills.emplace_back(screenX, screenY);
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
        cv::warpPerspective(rawData, warpedDepth, warpMat, dsize);

        applyDepthGradient();
    }

    void Manager::calculateColorWarpMatrix(const std::vector<std::uint8_t>& colorMat) {
        static auto normToPixel = [](std::array<cv::Point2f, 4> normCoords) -> std::array<cv::Point2f, 4> {
            std::array<cv::Point2f, 4> pixelCoords;
            std::transform(normCoords.begin(), normCoords.end(), pixelCoords.begin(), [](cv::Point2f normPoint) {
                return cv::Point2f(KinectSensor::COLOR_WIDTH * normPoint.x, KinectSensor::COLOR_HEIGHT * normPoint.y);
                });

            return pixelCoords;
            };

        static const std::array<cv::Point2f, 4> outputPointsPixel = normToPixel(outputPointsNorm);
        static const std::array<cv::Point2f, 4> inputPointsPixel = normToPixel(inputColorPointsNorm);

        cv::Mat rawData = cv::Mat(KinectSensor::COLOR_HEIGHT, KinectSensor::COLOR_WIDTH, CV_8UC4, (void*)colorMat.data());

        cv::Mat warpMat = cv::getPerspectiveTransform(inputPointsPixel, outputPointsPixel);
        cv::warpPerspective(rawData, warpedColor, warpMat, dsizeColor);
    }

    std::vector<DetectedTower> Manager::detectTowers() {
        std::vector<DetectedTower> detectedTowers;

        static cv::Mat bgr, smallBgr, hsv, mask;

        cv::cvtColor(warpedColor, bgr, cv::COLOR_RGBA2BGR);

        float scaleFactor = 0.25f;
        cv::resize(bgr, smallBgr, cv::Size(), scaleFactor, scaleFactor);
        cv::cvtColor(smallBgr, hsv, cv::COLOR_BGR2HSV);

        float areaScale = scaleFactor * scaleFactor;
        int minArea = static_cast<int>(2000 * areaScale);
        int maxArea = static_cast<int>(50000 * areaScale);

        struct ColorRange {
            TowerType type;
            cv::Scalar lower;
            cv::Scalar upper;
        };

        static const std::vector<ColorRange> colorRanges = {
            // Cyan tower -> Draws Red Dot
            { TowerType::Frog,    cv::Scalar(75, 60, 100), cv::Scalar(105, 255, 255)},

            // Pink tower -> Draws Green Dot
            { TowerType::Mortar,  cv::Scalar(140, 50, 100),     cv::Scalar(170, 255, 255)},

            // Black tower -> Draws Yellow Dot
            { TowerType::Sprayer, cv::Scalar(0, 0, 0),      cv::Scalar(180, 255, 140)}
        };

        static const cv::Size kernelSize{ 3, 3 }; // Smaller kernel for smaller image
        static cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, kernelSize);

        for (const auto& range : colorRanges) {
            cv::inRange(hsv, range.lower, range.upper, mask);

            cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
            cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            for (const auto& contour : contours) {
                double area = cv::contourArea(contour);
                if (area < minArea || area > maxArea) continue;

                cv::RotatedRect rotRect = cv::minAreaRect(contour);
                float w = rotRect.size.width;
                float h = rotRect.size.height;

                float aspectRatio = (h > 0 && w > 0) ? std::max(w, h) / std::min(w, h) : 0.0f;
                if (aspectRatio > 2.0f) continue;

                cv::Moments m = cv::moments(contour);
                if (m.m00 == 0) continue;

                float smallCx = static_cast<float>(m.m10 / m.m00);
                float smallCy = static_cast<float>(m.m01 / m.m00);

                DetectedTower tower;
                tower.type = range.type;

                tower.position.x = smallCx / static_cast<float>(smallBgr.cols);
                tower.position.y = smallCy / static_cast<float>(smallBgr.rows);

                detectedTowers.push_back(tower);
            }
        }

        return detectedTowers;
    }

    void Manager::applyDepthGradient() {
        static cv::Mat scaleMat;
        if (scaleMat.empty() || scaleMat.size() != warpedDepth.size()) {
            scaleMat = cv::Mat(warpedDepth.size(), CV_32F);

            for (int y = 0; y < scaleMat.rows; ++y) {
                float ratio = static_cast<float>(y) / scaleMat.rows;
                float factor = 1.0f + (ratio * (BOTTOM_SCALE - 1.0f));
                scaleMat.row(y).setTo(cv::Scalar(factor));
            }
        }


        cv::Mat floatDepth;
        warpedDepth.convertTo(floatDepth, CV_32F);
        cv::multiply(floatDepth, scaleMat, floatDepth);
        floatDepth.convertTo(warpedDepth, warpedDepth.type());
    }

    cv::Mat Manager::DEBUG_showDetectedTowers() {
        static cv::Mat bgr, smallBgr, hsv, mask, smallOutput;

        cv::cvtColor(warpedColor, bgr, cv::COLOR_RGBA2BGR);

        float scaleFactor = 0.25f;
        cv::resize(bgr, smallBgr, cv::Size(), scaleFactor, scaleFactor);
        cv::cvtColor(smallBgr, hsv, cv::COLOR_BGR2HSV);

        float areaScale = scaleFactor * scaleFactor;
        int minArea = static_cast<int>(2000 * areaScale);
        int maxArea = static_cast<int>(50000 * areaScale);

        struct ColorRange {
            TowerType type;
            cv::Scalar lower;
            cv::Scalar upper;
            cv::Scalar dotColor; // Added color for the debug dot (B, G, R)
        };

        static const std::vector<ColorRange> colorRanges = {
            // Cyan tower -> Draws Red Dot
            { TowerType::Frog,    cv::Scalar(75, 60, 100), cv::Scalar(105, 255, 255), cv::Scalar(0, 0, 255) },

            // Pink tower -> Draws Green Dot
            { TowerType::Mortar,  cv::Scalar(140, 50, 100),     cv::Scalar(170, 255, 255),  cv::Scalar(0, 255, 0) },

            // Black tower -> Draws Yellow Dot
            { TowerType::Sprayer, cv::Scalar(0, 0, 0),      cv::Scalar(180, 255, 140),  cv::Scalar(0, 255, 255) }

        };

        // We draw on the BIG image, so clone the original BGR
        cv::Mat outputImage = bgr.clone();

        static const cv::Size kernelSize{ 3, 3 }; // Smaller kernel for smaller image
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, kernelSize);

        for (const auto& range : colorRanges) {
            // Process the SMALL image
            cv::inRange(hsv, range.lower, range.upper, mask);
            cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
            cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            for (const auto& contour : contours) {
                double area = cv::contourArea(contour);
                if (area < minArea || area > maxArea) continue;

                cv::RotatedRect rotRect = cv::minAreaRect(contour);
                float w = rotRect.size.width;
                float h = rotRect.size.height;
                float aspectRatio = (h > 0 && w > 0) ? std::max(w, h) / std::min(w, h) : 0.0f;
                if (aspectRatio > 2.0f) continue;

                cv::Moments m = cv::moments(contour);
                if (m.m00 == 0) continue;

                float smallCx = static_cast<float>(m.m10 / m.m00);
                float smallCy = static_cast<float>(m.m01 / m.m00);

                int finalCx = static_cast<int>(smallCx / scaleFactor);
                int finalCy = static_cast<int>(smallCy / scaleFactor);

                // Draw on the BIG image
                cv::circle(outputImage, cv::Point(finalCx, finalCy), 10, range.dotColor, -1);
            }
        }

        cv::Mat finalOutput;
        cv::cvtColor(outputImage, finalOutput, cv::COLOR_BGR2RGBA);
        return finalOutput;
    }

    cv::Mat Manager::DEBUG_showHills() {
        cv::Mat hillMask = getHillMask();

        cv::Mat debugImg;
        cv::cvtColor(hillMask, debugImg, cv::COLOR_GRAY2BGR);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(hillMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        static constexpr int minHillArea = 200;

        for (int i = 0; i < contours.size(); i++) {
            const auto& contour = contours[i];

            if (cv::contourArea(contour) <= minHillArea) continue;

            cv::Rect bounds = cv::boundingRect(contour);
            bool touchesBorder = (bounds.x <= 1 || bounds.y <= 1 ||
                bounds.x + bounds.width >= hillMask.cols - 1 ||
                bounds.y + bounds.height >= hillMask.rows - 1);

            if (touchesBorder) {
                cv::drawContours(debugImg, contours, i, cv::Scalar(100, 100, 100), 2);
                continue;
            }

            cv::drawContours(debugImg, contours, i, cv::Scalar(0, 255, 0), 2);

            cv::Moments m = cv::moments(contour);
            if (m.m00 != 0) {
                int cx = static_cast<int>(m.m10 / m.m00);
                int cy = static_cast<int>(m.m01 / m.m00);

                cv::circle(debugImg, cv::Point(cx, cy), 5, cv::Scalar(0, 0, 255), -1);
            }
        }

        return debugImg;
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