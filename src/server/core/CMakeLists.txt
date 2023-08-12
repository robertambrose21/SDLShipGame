cmake_minimum_required(VERSION 3.12.0)
project(CoreLib VERSION 0.1.0)

cmake_minimum_required(VERSION 3.12.0)
project(ServerCoreLib VERSION 0.1.0)

add_library(
    ${PROJECT_NAME}
    net/gameserver.cpp
    net/servermessagesreceiver.h
    net/servermessagestransmitter.h
    net/gameadapter.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(ServerCoreLib)