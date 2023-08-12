cmake_minimum_required(VERSION 3.12.0)
project(CommonLib VERSION 0.1.0)

add_subdirectory(core)
add_subdirectory(game)
add_subdirectory(graphics)

add_library(${PROJECT_NAME} INTERFACE)

set_target_properties(CommonLib PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(CommonLib INTERFACE CommonGameLib CommonCoreLib CommonGraphicsLib)