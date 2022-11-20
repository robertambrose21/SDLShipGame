cmake_minimum_required(VERSION 3.0.0)
project(GameLib VERSION 0.1.0)

include_directories(../)

add_library(
    ${PROJECT_NAME}
    entities/entity.cpp 
    entities/entitypool.cpp 
    entities/behaviour/behaviourstrategy.h
    entities/behaviour/chaseandattackstrategy.cpp
    weapons/weapon.cpp 
    weapons/projectile.cpp 
    weapons/projectileweapon.cpp 
    weapons/meleeweapon.cpp
    weapons/areaofeffect.cpp
    application/serverapplication.cpp
    application/clientapplication.cpp
    application/application.cpp
    application/applicationcontext.cpp
    net/messages.h
    weapons/projectilepool.cpp
    weapons/areaofeffectpool.cpp
    weapons/weaponcontroller.cpp
    application/turncontroller.cpp
    net/client/gameclientmessagesreceiver.cpp
    net/client/gameclientmessagestransmitter.cpp
    net/server/gameservermessagesreceiver.cpp
    net/server/gameservermessagestransmitter.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 20)
target_link_libraries(GameLib ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})