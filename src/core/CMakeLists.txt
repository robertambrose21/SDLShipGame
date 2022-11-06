cmake_minimum_required(VERSION 3.0.0)
project(CoreLib VERSION 0.1.0)

include_directories(../../)
include_directories(../)

add_library(
    ${PROJECT_NAME}
    glmimport.cpp 
    grid/grid.cpp
    net/server/gameserver.cpp
    net/server/servermessagesreceiver.h
    net/client/gameclient.cpp
    net/client/clientmessagesreceiver.h
    net/gameadapter.cpp
    net/message/messages.h
    util/idgenerator.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(CoreLib ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})