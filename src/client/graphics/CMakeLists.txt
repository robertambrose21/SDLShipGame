cmake_minimum_required(VERSION 3.12.0)
project(GraphicsLib VERSION 0.1.0)

include_directories(../)

add_library(
    ${PROJECT_NAME}
    window.cpp 
    textureloader.cpp 
    gridrenderer.cpp
    graphicscontext.cpp
    sdlimport.h
    ui/dice.cpp
    ui/button.cpp
    ui/panel.cpp
    ui/playerpanel.cpp
    ui/text.cpp
    ui/actionbutton.cpp
    ui/inventorypanel.cpp
    ui/entitypanel.cpp
    ui/itemraritycolours.h
    ui/examineitempanel.cpp
    ui/diagnosticspanel.cpp
    camera.cpp
    drawstrategies/drawstrategy.h
    drawstrategies/entitydrawstrategy.cpp
    drawstrategies/weapondrawstrategy.cpp
    drawstrategies/projectiledrawstrategy.cpp
    drawstrategies/areaofeffectdrawstrategy.cpp
    drawstrategies/itemdrawstrategy.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(GraphicsLib Imgui ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2TTF_LIBRARY})