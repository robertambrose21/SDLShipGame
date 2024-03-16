cmake_minimum_required(VERSION 3.12.0)
project(CommonCoreLib VERSION 0.1.0)

include_directories(../../)
include_directories(../)

add_library(
    ${PROJECT_NAME}
    glmimport.cpp 
    grid/grid.cpp
    grid/generation/generationstrategy.cpp
    grid/generation/emptystrategy.cpp
    grid/generation/hallstrategy.cpp
    net/message/messages.h
    net/yojimboimport.h
    util/idgenerator.h
    util/gameassert.h
    util/randomrolls.h
    util/timing.h
    state/state.h
    state/statemachine.cpp
    event/event.h
    event/eventpublisher.h
    event/eventsubscriber.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(CommonCoreLib)