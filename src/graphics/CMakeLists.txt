cmake_minimum_required(VERSION 3.1.0)
project(GraphicsLib VERSION 0.1.0)

include_directories(../)

add_library(
    ${PROJECT_NAME}
    window.cpp 
    textureloader.cpp 
    gridrenderer.cpp
    graphicscontext.cpp
    sdlimport.h
    healthbar.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(GraphicsLib ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})