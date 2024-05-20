#pragma once

#include "game/net/messages.h"
#include "core/net/servermessagestransmitter.h"
#include "core/util/gameassert.h"

class Grid;
class EntityPool;
class WeaponController;
class ProjectilePool;
class AreaOfEffectPool;
class TurnController;
class ItemController;
class EffectController;
class SpawnController;

class ApplicationContext {
private:
    Grid* grid;
    EntityPool* entityPool;
    WeaponController* weaponController;
    ProjectilePool* projectilePool;
    AreaOfEffectPool* areaOfEffectPool;
    TurnController* turnController;
    ItemController* itemController;
    EffectController* effectController;
    SpawnController* spawnController;
    ServerMessagesTransmitter* transmitter;

public:
    ApplicationContext(
        Grid* grid,
        EntityPool* entityPool,
        WeaponController* weaponController,
        ProjectilePool* projectilePool,
        AreaOfEffectPool* areaOfEffectPool,
        TurnController* turnController,
        ItemController* itemController,
        EffectController* effectController,
        SpawnController* SpawnController
    );

    Grid* getGrid(void);
    EntityPool* getEntityPool(void);
    WeaponController* getWeaponController(void);
    ProjectilePool* getProjectilePool(void);
    AreaOfEffectPool* getAreaOfEffectPool(void);
    TurnController* getTurnController(void);
    ItemController* getItemController(void);
    EffectController* getEffectController(void);
    SpawnController* getSpawnController(void);

    void setServerMessagesTransmitter(ServerMessagesTransmitter* transmitter);
    ServerMessagesTransmitter* getServerMessagesTransmitter(void);
};