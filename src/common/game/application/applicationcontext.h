#pragma once

#include "game/net/messages.h"
#include "core/net/servermessagestransmitter.h"
#include "core/util/gameassert.h"

class Grid;
class EntityPool;
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
    EntityPool* entityPool;
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
        EntityPool* entityPool,
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
    EntityPool* getEntityPool(void);
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