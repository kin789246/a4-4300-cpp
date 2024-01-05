cmake_minimum_required(VERSION 3.15)

# for LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add MacOS frameworks path for LLVM 17
set(CMAKE_CXX_COMPILER 
    /opt/homebrew/opt/llvm/bin/clang++ -F$ENV{SDKROOT}/System/Library/Frameworks
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-result -pedantic-errors")

project(
    a4
    LANGUAGES CXX
    VERSION 0.1.0
)

add_subdirectory(dependencies)

# make executable app
set(SRC_FILES
    src/main.cpp
    src/Action.cpp
    src/Animation.cpp
    src/Assets.cpp
    src/Physics.cpp
    src/Scene.cpp
    src/Scene_Menu.cpp
    src/Scene_Zelda.cpp
    src/Vec2.cpp
    src/GameEngine.cpp
    src/Entity.cpp
    src/EntityManager.cpp
)
set(exec_name a4)
add_executable(${exec_name} ${SRC_FILES})
target_link_libraries(${exec_name} PRIVATE ImGui-SFML::ImGui-SFML sfml-audio)
include_directories(include)

# copy assets to binary directory
file(
    COPY ${CMAKE_SOURCE_DIR}/assets/fonts/mario.ttf 
    DESTINATION ${CMAKE_BINARY_DIR}/assets/fonts
)

file(
    COPY ${CMAKE_SOURCE_DIR}/config/assets.txt 
    DESTINATION ${CMAKE_BINARY_DIR}/config
)

file(
    COPY ${CMAKE_SOURCE_DIR}/config/level1.txt 
    DESTINATION ${CMAKE_BINARY_DIR}/config
)

file(
    COPY ${CMAKE_SOURCE_DIR}/config/level2.txt 
    DESTINATION ${CMAKE_BINARY_DIR}/config
)

file(
    COPY ${CMAKE_SOURCE_DIR}/config/level3.txt 
    DESTINATION ${CMAKE_BINARY_DIR}/config
)

file(GLOB game_png ${CMAKE_SOURCE_DIR}/assets/images/*.png)
file(GLOB game_wav ${CMAKE_SOURCE_DIR}/assets/sounds/*.wav)
file(GLOB game_ogg ${CMAKE_SOURCE_DIR}/assets/sounds/*.ogg)

file(COPY ${game_png} DESTINATION ${CMAKE_BINARY_DIR}/assets/images)
file(COPY ${game_wav} DESTINATION ${CMAKE_BINARY_DIR}/assets/sounds)
file(COPY ${game_ogg} DESTINATION ${CMAKE_BINARY_DIR}/assets/sounds)
