cmake_minimum_required(VERSION 3.12.0)
project(ShipGameClientLib VERSION 0.1.0)

add_subdirectory(application)
add_subdirectory(core)
add_subdirectory(player)
add_subdirectory(graphics)

add_library(${PROJECT_NAME} INTERFACE)

set_target_properties(ShipGameClientLib PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(ShipGameClientLib INTERFACE ClientApplicationLib ClientPlayerLib ClientCoreLib GraphicsLib)