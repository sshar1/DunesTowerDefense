cmake_minimum_required(VERSION 3.20)
project(DunesTowerDefense)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED COMPONENTS core imgcodecs imgproc videoio)
find_package(SDL2 REQUIRED)

add_executable(DunesTowerDefense src/main.cpp external/glad/src/glad.c)
add_executable(UnitTests tests/test.cpp external/glad/src/glad.c)

add_library(GameDependencies INTERFACE)

target_include_directories(GameDependencies INTERFACE
        /opt/homebrew/include
        external/KHR
        external/glad/include
        external/glm
        ${OpenCV_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(GameDependencies INTERFACE
        ${OpenCV_LIBS}
        ${SDL2_LIBRARIES}
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
)

target_link_libraries(DunesTowerDefense PRIVATE GameDependencies)
target_link_libraries(UnitTests PRIVATE GameDependencies)