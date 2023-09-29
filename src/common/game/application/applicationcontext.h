#pragma once

#include "game/net/messages.h"
#include "graphics/window.h"
#include "game/entities/entitypool.h"
#include "game/weapons/projectilepool.h"
#include "game/weapons/areaofeffectpool.h"
#include "game/weapons/weaponcontroller.h"
#include "turncontroller.h"
#include "core/net/servermessagestransmitter.h"
#include "core/util/gameassert.h"

class ApplicationContext {
private:
    // Window& window;
    Grid& grid;
    EntityPool& entityPool;
    WeaponController& weaponController;
    ProjectilePool& projectilePool;
    AreaOfEffectPool& areaOfEffectPool;
    TurnController& turnController;
    ServerMessagesTransmitter* transmitter;

public:
    ApplicationContext(
        // Window& window,
        Grid& grid,
        EntityPool& entityPool,
        WeaponController& weaponController,
        ProjectilePool& projectilePool,
        AreaOfEffectPool& areaOfEffectPool,
        TurnController& turnController
    );

    Grid& getGrid(void);
    EntityPool& getEntityPool(void);
    WeaponController& getWeaponController(void);
    ProjectilePool& getProjectilePool(void);
    AreaOfEffectPool& getAreaOfEffectPool(void);
    TurnController& getTurnController(void);

    void setServerMessagesTransmitter(ServerMessagesTransmitter* transmitter);
    ServerMessagesTransmitter* getServerMessagesTransmitter(void);

    GameStateUpdate getCurrentGameState(void);
};