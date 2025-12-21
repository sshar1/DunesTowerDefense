cmake_minimum_required(VERSION 3.20)
project(DunesTowerDefense)

set(CMAKE_CXX_STANDARD 20)

# 1. Find Packages
find_package(glfw3 REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgcodecs imgproc videoio)
find_package(SDL2 REQUIRED)

# 2. Add Executable
add_executable(DunesTowerDefense src/main.cpp external/glad/src/glad.c)

# 3. Include Directories
target_include_directories(DunesTowerDefense PRIVATE
        /opt/homebrew/include
        external/KHR
        external/glad/include
        includes/glm
        ${OpenCV_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
)

# 4. Link Libraries
target_link_libraries(DunesTowerDefense
        glfw
        ${OpenCV_LIBS}
        ${SDL2_LIBRARIES}
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
)