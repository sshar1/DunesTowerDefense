//
// Created by Sami Sharif  on 12/23/25.
//

#include "engine/DataLoader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

DataLoader::DataLoader()
    : topographyVertices(std::make_unique<TopographyVertices>(DEPTH_HEIGHT * DEPTH_WIDTH))
{
}

const TopographyVertices& DataLoader::processTopographyVertices(std::string_view filepath) {
    // TODO for now, this is going to be a csv file. It won't be later.

    // std::vector<TopologyVertex> topologyVertices;
    // topologyVertices.reserve(DEPTH_WIDTH * DEPTH_HEIGHT);
    std::ifstream file(filepath.data());
    std::string line;

    if (!file.is_open()) {
        std::cout << "Error opening file" << std::endl;
    }

    int row = 0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        int col = 0;

        while (std::getline(ss, value, ',')) {
            // topologyVertices->push_back(std::stoi(value));
            (*topographyVertices)[DEPTH_WIDTH * row + col] = std::stoi(value);
            col++;
        }

        row++;
    }

    return *topographyVertices;
}
