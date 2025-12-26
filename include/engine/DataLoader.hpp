//
// Created by Sami Sharif  on 12/23/25.
//

#pragma once
#include <vector>

/*
 * This class is responsible for taking information from the kinect and
 * converting it to a friendly format for rendering
 * For example, it converts the raw kinect data to vertices, filtering
 * out the outside world
 */
using TopographyVertex = std::uint16_t;
using TopographyVertices = std::vector<TopographyVertex>;

class DataLoader {
public:
    static DataLoader& getInstance() {
        static DataLoader instance;
        return instance;
    }

    DataLoader(DataLoader const&) = delete;
    DataLoader& operator=(DataLoader const&) = delete;
    DataLoader(DataLoader&&) = delete;
    DataLoader& operator=(DataLoader&&) = delete;

    // TODO for now, this will read straight from a csv and output the vertices
    const TopographyVertices& processTopographyVertices(std::string_view filepath);

    // TODO these are here temporarily but we should move them to a kinect manager of some sort
    static constexpr int RESOLUTION = 1;
    static constexpr int DEPTH_WIDTH = 512 / RESOLUTION;
    static constexpr int DEPTH_HEIGHT = 424 / RESOLUTION;

private:
    DataLoader();
    ~DataLoader() = default;

    std::unique_ptr<TopographyVertices> topographyVertices;
};

