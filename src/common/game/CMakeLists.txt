cmake_minimum_required(VERSION 3.12.0)
project(CommonGameLib VERSION 0.1.0)

include_directories(../../)
include_directories(../)

add_library(
    ${PROJECT_NAME}
    effects/statuseffect.cpp
    effects/effect.h
    entities/entity.cpp 
    entities/entitystats.h
    entities/entitypool.cpp 
    entities/behaviour/behaviourstrategy.h
    entities/behaviour/chaseandattackstrategy.cpp
    items/item.cpp
    items/loottable.cpp
    items/itemcontroller.cpp
    weapons/weapon.cpp 
    weapons/projectile.cpp 
    weapons/projectileweapon.cpp 
    weapons/meleeweapon.cpp
    weapons/areaofeffect.cpp
    application/application.cpp
    application/applicationcontext.cpp
    net/messages.cpp
    weapons/projectilepool.cpp
    weapons/areaofeffectpool.cpp
    weapons/weaponcontroller.cpp
    application/turncontroller.cpp
    application/stdoutsubscriber.cpp
    actions/action.cpp
    actions/moveaction.cpp
    actions/attackaction.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(CommonGameLib)