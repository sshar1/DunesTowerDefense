cmake_minimum_required(VERSION 3.20)
project(DunesTowerDefense)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED COMPONENTS core imgcodecs imgproc videoio)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
#add_executable(my_game ${MY_SOURCES})
#target_link_libraries(my_game PRIVATE SDL2_image::SDL2_image)

add_executable(DunesTowerDefense src/main.cpp external/glad/src/glad.c
        src/MainGame.cpp
        include/MainGame.hpp
        src/InputManager.cpp
        include/InputManager.hpp
        src/engine/Renderer.cpp
        include/engine/Renderer.hpp
        src/engine/DataLoader.cpp
        include/engine/DataLoader.hpp
        src/entities/DungBeetle.cpp
        include/entities/DungBeetle.hpp
        src/entities/Sprite.cpp
        include/entities/Sprite.hpp
        src/entities/Enemy.cpp
        include/entities/Enemy.hpp
        src/WaveManager.cpp
        include/WaveManager.hpp)
add_executable(UnitTests tests/test.cpp external/glad/src/glad.c)

add_library(GameDependencies INTERFACE)

target_include_directories(GameDependencies INTERFACE
        include
        /opt/homebrew/include
        external/KHR
        external/rapidcsv
        external/glad/include
        external/glm
        ${OpenCV_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(GameDependencies INTERFACE
        ${OpenCV_LIBS}
        ${SDL2_LIBRARIES}
#        SDL2::SDL2
        SDL2_image::SDL2_image
        ${SDL2_LIBRARIES}
#        ${SDL2_IMAGE_LIBRARIES}
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
)

target_link_libraries(DunesTowerDefense PRIVATE GameDependencies)
target_link_libraries(UnitTests PRIVATE GameDependencies)