cmake_minimum_required(VERSION 3.12.0)
project(CommonGameLib VERSION 0.1.0)

include_directories(../../)
include_directories(../)

add_library(
    ${PROJECT_NAME}
    effects/effecttypes.h
    effects/effect.cpp
    effects/grideffect.cpp
    effects/effectcontroller.cpp
    entities/entity.cpp 
    entities/entitypool.cpp 
    entities/behaviour/behaviourstrategy.cpp
    entities/behaviour/chaseandattackstrategy.cpp
    items/item.cpp
    items/loottable.cpp
    items/itemcontroller.cpp
    items/equippable.h
    items/gear.cpp
    weapons/weapon.cpp 
    weapons/projectile.cpp 
    weapons/projectileweapon.cpp 
    weapons/meleeweapon.cpp
    weapons/areaofeffect.cpp
    application/application.cpp
    application/applicationcontext.cpp
    net/messages.cpp
    message/messages.h
    weapons/projectilepool.cpp
    weapons/areaofeffectpool.cpp
    weapons/weaponcontroller.cpp
    weapons/damagesource.cpp
    application/turncontroller.cpp
    application/stdoutsubscriber.cpp
    application/visibilitycontroller.cpp
    actions/action.cpp
    actions/moveaction.cpp
    actions/attackaction.cpp
    actions/takeitemaction.cpp
    actions/equipgearaction.cpp
    actions/equipweaponaction.cpp
    event/events.h
    stats/stats.h
    spawn/spawncontroller.cpp
    net/gamemessagelogger.cpp
    participant/participant.cpp
    data/tiles.h
    data/engagements.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(CommonGameLib)