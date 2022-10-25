cmake_minimum_required(VERSION 3.0.0)
project(CoreLib VERSION 0.1.0)

include_directories(../../)
include_directories(../)

add_library(
    ${PROJECT_NAME}
    glmimport.cpp 
    grid/grid.cpp
    net/server/gameserver.cpp
    net/client/gameclient.cpp
    net/gameadapter.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(CoreLib ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})