cmake_minimum_required(VERSION 3.0.0)
project(Test VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(glm REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_DIRS})

add_subdirectory(src/graphics)
add_subdirectory(src/player)
add_subdirectory(src/game)
add_subdirectory(src/core)

add_executable(Test src/main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(Test CoreLib GraphicsLib GameLib PlayerLib)
