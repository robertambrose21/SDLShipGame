cmake_minimum_required(VERSION 3.12.0)
project(ShipGame VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(THREADS_PREFER_PTHREAD_FLAG ON)

if(APPLE)
     set(CMAKE_PREFIX_PATH "/opt/homebrew")
     list(TRANSFORM CMAKE_PREFIX_PATH APPEND "/include"
          OUTPUT_VARIABLE CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES)
     set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES "${CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES}")
endif()

# https://github.com/crashoz/uuid_v4/issues/14
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2TTF REQUIRED)
find_package(glm REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_DIRS} ${SDL2TTF_DIRS}. ./src/common ./src/server ./src/client ./imgui)

add_subdirectory(src/server)
add_subdirectory(src/client)
add_subdirectory(src/common)
add_subdirectory(yojimbo)
add_subdirectory(imgui)
add_subdirectory(stduuid)

add_executable(ShipGameServer src/server/main.cpp)
set_target_properties(ShipGameServer PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(ShipGameServer yojimbo ShipGameServerLib CommonLib Threads::Threads stduuid)

add_executable(ShipGameClient src/client/main.cpp)
set_target_properties(ShipGameClient PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(ShipGameClient yojimbo ShipGameClientLib CommonLib Threads::Threads stduuid)
