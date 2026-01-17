cmake_minimum_required(VERSION 3.20)
project(DunesTowerDefense)

set(CMAKE_CXX_STANDARD 20)

# --- Library Setup ---
set(OpenCV_DIR "C:/opencv/build/x64/vc16/lib")
find_package(OpenCV REQUIRED COMPONENTS core imgcodecs imgproc videoio)

set(SDL2_DIR "C:/SDL2-2.32.10/cmake")
find_package(SDL2 REQUIRED)

set(SDL2_image_DIR "C:/SDL2_image-2.8.8/cmake")
find_package(SDL2_image REQUIRED)

# --- Define Sources ---
# (I kept your list exactly as is)
set(SOURCE_FILES 
    src/main.cpp 
    external/glad/src/glad.c
    src/MainGame.cpp include/MainGame.hpp
    src/InputManager.cpp include/InputManager.hpp
    src/engine/Renderer.cpp include/engine/Renderer.hpp
    src/engine/DataLoader.cpp include/engine/DataLoader.hpp
    src/entities/DungBeetle.cpp include/entities/DungBeetle.hpp
    src/entities/Sprite.cpp include/entities/Sprite.hpp
    src/entities/Enemy.cpp include/entities/Enemy.hpp
    src/WaveManager.cpp include/WaveManager.hpp
    src/engine/ResourceManager.cpp include/engine/ResourceManager.hpp
    src/entities/DuneWorm.cpp include/entities/DuneWorm.hpp
    src/entities/Base.cpp include/entities/Base.hpp
    include/engine/VisionManager.hpp src/engine/VisionManager.cpp
    include/engine/KinectSensor.hpp src/engine/KinectSensor.cpp
    src/entities/Bee.cpp include/entities/Bee.hpp
    src/entities/projectiles/Projectile.cpp include/entities/projectiles/Projectile.hpp
    src/entities/projectiles/Rock.cpp include/entities/projectiles/Rock.hpp
    src/entities/projectiles/Stinger.cpp include/entities/projectiles/Stinger.hpp
    src/entities/towers/Tower.cpp include/entities/towers/Tower.hpp
    src/entities/towers/Sprayer.cpp include/entities/towers/Sprayer.hpp
    src/entities/towers/Frog.cpp include/entities/towers/Frog.hpp
    src/entities/towers/Mortar.cpp include/entities/towers/Mortar.hpp
    include/entities/util/ITargetable.hpp
)

# --- Kinect SDK Setup ---
# The SDK usually creates an environment variable KINECTSDK20_DIR
if(NOT DEFINED ENV{KINECTSDK20_DIR})
    message(FATAL_ERROR "Kinect SDK 2.0 not found. Please install it.")
endif()

# Set paths based on the environment variable
set(KINECT_INC_DIR "$ENV{KINECTSDK20_DIR}/inc")
set(KINECT_LIB_DIR "$ENV{KINECTSDK20_DIR}/lib/x64")

include_directories(${KINECT_INC_DIR})
link_directories(${KINECT_LIB_DIR})

add_executable(DunesTowerDefense ${SOURCE_FILES})
add_executable(UnitTests tests/test.cpp external/glad/src/glad.c)

# --- Interface Library for Dependencies ---
add_library(GameDependencies INTERFACE)

target_include_directories(GameDependencies INTERFACE
    include
    external/KHR
    external/rapidcsv
    external/glad/include
    external/glm
    # REMOVED: /opt/homebrew/include (This is Mac only)
    # REMOVED: Manual ${SDL_INCLUDE_DIRS} (The targets below handle this automatically)
)

target_link_libraries(GameDependencies INTERFACE
    ${OpenCV_LIBS}
    SDL2::SDL2
    SDL2::SDL2main   # Required on Windows for entry point
    SDL2_image::SDL2_image
)

# --- Handle Platform Specifics ---
if(APPLE)
    target_link_libraries(GameDependencies INTERFACE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
elseif(WIN32)
    # Windows usually needs opengl32 explicitly if you use raw OpenGL calls
    target_link_libraries(GameDependencies INTERFACE opengl32)
endif()

# --- Link Everything ---
target_link_libraries(DunesTowerDefense PRIVATE GameDependencies kinect20)
target_link_libraries(UnitTests PRIVATE GameDependencies)

set(SDL2_DLL_PATH "C:/SDL2-2.32.10/lib/x64/SDL2.dll")
set(SDL2_IMAGE_DLL_PATH "C:/SDL2_image-2.8.8/lib/x64/SDL2_image.dll")

# For OpenCV, we search for the DLL because the name changes with versions
file(GLOB OPENCV_DLL_PATH "C:/opencv/build/x64/vc16/bin/opencv_world*.dll")

# 2. Command to copy them to the executable folder after building
add_custom_command(TARGET DunesTowerDefense POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DLL_PATH}
        $<TARGET_FILE_DIR:DunesTowerDefense>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_IMAGE_DLL_PATH}
        $<TARGET_FILE_DIR:DunesTowerDefense>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${OPENCV_DLL_PATH}
        $<TARGET_FILE_DIR:DunesTowerDefense>
    COMMENT "Copying runtime DLLs to build directory..."
)

#file(COPY ${CMAKE_SOURCE_DIR}/tests DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(TARGET DunesTowerDefense POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    ${CMAKE_CURRENT_BINARY_DIR}/data
    COMMENT "Syncing data folder (shaders, textures, etc.) to build directory..."
)