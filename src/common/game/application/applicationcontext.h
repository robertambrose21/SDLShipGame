#pragma once

#include "game/net/messages.h"
#include "core/net/servermessagestransmitter.h"
#include "core/util/gameassert.h"

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

class ApplicationContext {
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
};