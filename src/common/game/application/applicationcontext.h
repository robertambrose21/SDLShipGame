#pragma once

#include <entt/entt.hpp>

#include "game/net/messages.h"
#include "core/net/servermessagestransmitter.h"
#include "core/util/gameassert.h"
#include "game/ecs/logicsystemregistry.h"
// #include "game/ecs/drawsystemregistry.h"

class Grid;
class ActorPool;
class WeaponController;
class ProjectilePool;
class AreaOfEffectPool;
class GameController;
class ItemController;
class EffectController;
class SpawnController;
class VisiblityController;
class LogicSystemRegistry;
// class DrawSystemRegistry;

class ApplicationContext {
public:
    ApplicationContext(
        Grid* grid,
        ActorPool* actorPool,
        WeaponController* weaponController,
        ProjectilePool* projectilePool,
        AreaOfEffectPool* areaOfEffectPool,
        GameController* gameController,
        ItemController* itemController,
        EffectController* effectController,
        SpawnController* SpawnController,
        VisiblityController* visiblityController
    );

    Grid* getGrid(void);
    ActorPool* getActorPool(void);
    WeaponController* getWeaponController(void);
    ProjectilePool* getProjectilePool(void);
    AreaOfEffectPool* getAreaOfEffectPool(void);
    GameController* getGameController(void);
    ItemController* getItemController(void);
    EffectController* getEffectController(void);
    SpawnController* getSpawnController(void);
    VisiblityController* getVisibilityController(void);

    void setServerMessagesTransmitter(ServerMessagesTransmitter* transmitter);
    ServerMessagesTransmitter* getServerMessagesTransmitter(void);

    entt::registry& getEntityRegistry(void);
    LogicSystemRegistry* getLogicSystemRegistry(void);
    // DrawSystemRegistry* getDrawSystemRegistry(void);

private:
    Grid* grid;
    ActorPool* actorPool;
    WeaponController* weaponController;
    ProjectilePool* projectilePool;
    AreaOfEffectPool* areaOfEffectPool;
    GameController* gameController;
    ItemController* itemController;
    EffectController* effectController;
    SpawnController* spawnController;
    VisiblityController* visiblityController;
    ServerMessagesTransmitter* transmitter;

    entt::registry registry;
    std::unique_ptr<LogicSystemRegistry> logicSystemRegistry;
    // std::unique_ptr<DrawSystemRegistry> drawSystemRegistry;
};