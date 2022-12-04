cmake_minimum_required(VERSION 3.1.0)
project(ShipGame VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(glm REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_DIRS} . ./src)

add_subdirectory(src/graphics)
add_subdirectory(src/player)
add_subdirectory(src/game)
add_subdirectory(src/core)
add_subdirectory(yojimbo)

add_executable(ShipGameServer src/main.cpp)
set_target_properties(ShipGameServer PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(ShipGameServer yojimbo GameLib CoreLib GraphicsLib PlayerLib Threads::Threads)

add_executable(ShipGameClient src/main_client.cpp)
set_target_properties(ShipGameClient PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(ShipGameClient yojimbo GameLib CoreLib GraphicsLib PlayerLib Threads::Threads)
