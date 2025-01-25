cmake_minimum_required(VERSION 3.12.0)
project(ServerApplicationLib VERSION 0.1.0)

include_directories(../)

add_library(
    ${PROJECT_NAME}
    serverapplication.cpp
    serverturncontroller.cpp
    net/gameservermessagesreceiver.cpp
    net/gameservermessagestransmitter.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(ServerApplicationLib yojimbo)