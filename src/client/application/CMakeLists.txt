cmake_minimum_required(VERSION 3.12.0)
project(ClientApplicationLib VERSION 0.1.0)

include_directories(../)

add_library(
    ${PROJECT_NAME}
    clientapplication.cpp
    clientstatemachine.cpp
    clientstates.cpp
    clientturncontroller.cpp
    net/gameclientmessagesreceiver.cpp
    net/gameclientmessagestransmitter.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(ClientApplicationLib ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})