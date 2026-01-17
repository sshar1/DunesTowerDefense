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

    cv::Mat Manager::detectTowers() {
        // 1. Prepare Images
        cv::Mat hsv;
        cv::Mat bgr;

        // Convert RGBA -> BGR -> HSV
        // We keep 'bgr' so we can draw dots on a color image (mask is black/white)
        cv::cvtColor(warpedColor, bgr, cv::COLOR_RGBA2BGR);
        cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);

        // 2. Define Colors
        struct ColorRange {
            TowerType type;
            cv::Scalar lower;
            cv::Scalar upper;
            cv::Scalar dotColor; // Added color for the debug dot (B, G, R)
        };

        static const std::vector<ColorRange> colorRanges = {
            // Cyan tower -> Draws Red Dot
            { TowerType::Frog,    cv::Scalar(80, 100, 100), cv::Scalar(100, 255, 255), cv::Scalar(0, 0, 255) },

            // Gray tower -> Draws Green Dot
            { TowerType::Mortar,  cv::Scalar(0, 0, 80),     cv::Scalar(180, 60, 200),  cv::Scalar(0, 255, 0) },

            // Black tower -> Draws Yellow Dot
            { TowerType::Sprayer, cv::Scalar(0, 0, 0),      cv::Scalar(180, 255, 40),  cv::Scalar(0, 255, 255) }
        };

        static constexpr int MIN_TOWER_AREA = 2000;
        static constexpr int MAX_TOWER_AREA = 50000;
        static const cv::Size kernelSize{ 5, 5 };
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, kernelSize);

        // outputImage will start as the clean BGR image.
        // We will draw dots onto THIS image.
        cv::Mat outputImage = bgr.clone();

        // 3. Loop through all tower types
        for (const auto& range : colorRanges) {
            cv::Mat mask;
            cv::inRange(hsv, range.lower, range.upper, mask);

            // Clean up noise
            cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
            cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            for (const auto& contour : contours) {
                double area = cv::contourArea(contour);
                if (area < MIN_TOWER_AREA || area > MAX_TOWER_AREA) continue;

                cv::RotatedRect rotRect = cv::minAreaRect(contour);
                float w = rotRect.size.width;
                float h = rotRect.size.height;

                // Calculate Aspect Ratio (Longest Side / Shortest Side)
                // We use std::max/min so it works whether the strip is vertical or horizontal
                float aspectRatio = (h > 0 && w > 0) ? std::max(w, h) / std::min(w, h) : 0.0f;

                // If the object is 2x longer than it is wide, it's a strip/glitch. Skip it.
                if (aspectRatio > 2.0f) continue;

                cv::Moments m = cv::moments(contour);
                if (m.m00 == 0) continue;

                // Calculate Center
                int cx = static_cast<int>(m.m10 / m.m00);
                int cy = static_cast<int>(m.m01 / m.m00);

                // --- DRAW THE DOT ---
                // Draw on 'outputImage' (BGR)
                // Position: (cx, cy)
                // Radius: 10
                // Color: range.dotColor
                // Thickness: -1 (Filled circle)
                cv::circle(outputImage, cv::Point(cx, cy), 10, range.dotColor, -1);

                // Optional: Draw outline around the tower too for better visibility
                cv::drawContours(outputImage, std::vector<std::vector<cv::Point>>{contour}, -1, range.dotColor, 2);
            }
        }

        // 4. Return the image with dots (Converted back to RGBA for your renderer)
        cv::Mat finalOutput;
        cv::cvtColor(outputImage, finalOutput, cv::COLOR_BGR2RGBA);

        return finalOutput;
    }

  //  cv::Mat Manager::detectTowers() {
  //      //std::vector<DetectedTower> towers;

  //      /*CV_Assert(!colorImg.empty());
  //      CV_Assert(colorImg.type() == CV_8UC3);*/

  //      cv::Mat hsv;
  //      cv::cvtColor(warpedColor, hsv, cv::COLOR_RGBA2BGR);
		//cv::cvtColor(hsv, hsv, cv::COLOR_BGR2HSV);

  //      struct ColorRange {
  //          TowerType type;
  //          cv::Scalar lower;
  //          cv::Scalar upper;
  //      };

  //      static const std::vector<ColorRange> colorRanges = {
  //          // Cyan tower
  //          {
  //              TowerType::Frog,
  //              cv::Scalar(80, 100, 100),
  //              cv::Scalar(100, 255, 255)
  //          },

  //          // Gray tower 
  //          {
  //              TowerType::Mortar,
  //              cv::Scalar(0, 0, 100),
  //              cv::Scalar(180, 60, 200)
  //          },

  //          // Black tower
  //          {
  //              TowerType::Sprayer,
  //              cv::Scalar(0, 0, 0),
  //              cv::Scalar(180, 255, 50)
  //          }

  //      };

  //      static constexpr int MIN_TOWER_AREA = 300;
  //      static const cv::Size kernelSize{ 5, 5 };
  //      cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, kernelSize);
  //      cv::Mat mask;
  //      cv::inRange(hsv, colorRanges[2].lower, colorRanges[2].upper, mask);

  //      cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
  //      cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

  //      //for (const auto& range : colorRanges) {
  //      //    //const auto& range = colorRanges[0];

  //      //    cv::Mat mask;
  //      //    cv::inRange(hsv, range.lower, range.upper, mask);

  //      //    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
  //      //    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

  //      //    std::vector<std::vector<cv::Point>> contours;
  //      //    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  //      //    for (const auto& contour : contours) {
  //      //        double area = cv::contourArea(contour);
  //      //        if (area < MIN_TOWER_AREA) continue;

  //      //        cv::Moments m = cv::moments(contour);
  //      //        if (m.m00 == 0) continue;

  //      //        float cx = static_cast<float>(m.m10 / m.m00);
  //      //        float cy = static_cast<float>(m.m01 / m.m00);

  //      //        // Convert to window coordinates
  //      //        float screenX = cx * MainGame::WINDOW_WIDTH / colorImg.cols;
  //      //        float screenY = cy * MainGame::WINDOW_HEIGHT / colorImg.rows;

  //      //        /*towers.push_back({
  //      //            range.type,
  //      //            glm::vec2(screenX, screenY)
  //      //            });*/
  //      //    }
  //      //}

  //      return mask;
  //  }

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