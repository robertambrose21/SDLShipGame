cmake_minimum_required(VERSION 3.12.0)
project(ShipGameServerLib VERSION 0.1.0)

add_subdirectory(application)
add_subdirectory(core)

add_library(${PROJECT_NAME} INTERFACE)

set_target_properties(ShipGameServerLib PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(ShipGameServerLib INTERFACE ServerApplicationLib ServerCoreLib)