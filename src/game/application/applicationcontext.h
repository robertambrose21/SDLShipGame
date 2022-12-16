#pragma once

#include "game/net/messages.h"
#include "graphics/window.h"
#include "game/entities/entitypool.h"
#include "game/weapons/projectilepool.h"
#include "game/weapons/areaofeffectpool.h"
#include "game/weapons/weaponcontroller.h"
#include "turncontroller.h"
#include "core/net/server/servermessagestransmitter.h"
#include "core/util/gameassert.h"

class ApplicationContext {
private:
    std::shared_ptr<Window> window;
    std::shared_ptr<EntityPool> entityPool;
    std::shared_ptr<WeaponController> weaponController;
    std::shared_ptr<ProjectilePool> projectilePool;
    std::shared_ptr<AreaOfEffectPool> areaOfEffectPool;
    std::shared_ptr<TurnController> turnController;
    std::shared_ptr<ServerMessagesTransmitter> transmitter;

public:
    ApplicationContext(
        const std::shared_ptr<Window>& window,
        const std::shared_ptr<EntityPool>& entityPool,
        const std::shared_ptr<WeaponController>& weaponController,
        const std::shared_ptr<ProjectilePool>& projectilePool,
        const std::shared_ptr<AreaOfEffectPool>& areaOfEffectPool,
        const std::shared_ptr<TurnController>& turnController
    );

    std::shared_ptr<Grid> getGrid(void);
    std::shared_ptr<GraphicsContext> getGraphicsContext(void);
    std::shared_ptr<Window> getWindow(void);
    std::shared_ptr<EntityPool> getEntityPool(void);
    std::shared_ptr<WeaponController> getWeaponController(void);
    std::shared_ptr<ProjectilePool> getProjectilePool(void);
    std::shared_ptr<AreaOfEffectPool> getAreaOfEffectPool(void);
    std::shared_ptr<TurnController> getTurnController(void);

    void setServerMessagesTransmitter(const std::shared_ptr<ServerMessagesTransmitter>& transmitter);
    std::shared_ptr<ServerMessagesTransmitter> getServerMessagesTransmitter(void);

    GameStateUpdate getCurrentGameState(void);
};