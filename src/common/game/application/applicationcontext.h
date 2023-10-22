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

class ApplicationContext {
private:
    Grid* grid;
    EntityPool* entityPool;
    WeaponController* weaponController;
    ProjectilePool* projectilePool;
    AreaOfEffectPool* areaOfEffectPool;
    TurnController* turnController;
    ServerMessagesTransmitter* transmitter;

public:
    ApplicationContext(
        Grid* grid,
        EntityPool* entityPool,
        WeaponController* weaponController,
        ProjectilePool* projectilePool,
        AreaOfEffectPool* areaOfEffectPool,
        TurnController* turnController
    );

    Grid* getGrid(void);
    EntityPool* getEntityPool(void);
    WeaponController* getWeaponController(void);
    ProjectilePool* getProjectilePool(void);
    AreaOfEffectPool* getAreaOfEffectPool(void);
    TurnController* getTurnController(void);

    void setServerMessagesTransmitter(ServerMessagesTransmitter* transmitter);
    ServerMessagesTransmitter* getServerMessagesTransmitter(void);
};