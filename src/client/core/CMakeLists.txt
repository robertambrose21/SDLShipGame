cmake_minimum_required(VERSION 3.12.0)
project(ClientCoreLib VERSION 0.1.0)

include_directories(../../)
include_directories(../)

add_library(
    ${PROJECT_NAME}
    net/gameclient.cpp
    net/clientmessagesreceiver.h
    net/clientgameadapter.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(ClientCoreLib ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})