cmake_minimum_required(VERSION 3.0.0)
project(PlayerLib VERSION 0.1.0)

add_library(${PROJECT_NAME} playercontroller.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(PlayerLib ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})