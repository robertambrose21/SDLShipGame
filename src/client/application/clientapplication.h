#pragma once

#include "net/gameclientmessagesreceiver.h"
#include "net/gameclientmessagestransmitter.h"
#include "core/net/gameclient.h"
#include "game/application/application.h"
#include "clientstatemachine.h"
#include "clientstates.h"
#include "graphics/window.h"
#include "graphics/drawstrategies/entitydrawstrategy.h"
#include "graphics/drawstrategies/weapondrawstrategy.h"
#include "graphics/drawstrategies/projectiledrawstrategy.h"
#include "graphics/drawstrategies/areaofeffectdrawstrategy.h"
#include "graphics/drawstrategies/itemdrawstrategy.h"
#include "clientturncontroller.h"

class ClientApplication {
private:
    std::unique_ptr<Application> application;

    std::unique_ptr<PlayerController> playerController;
    std::unique_ptr<GameClientMessagesTransmitter> clientMessagesTransmitter;
    std::unique_ptr<GameClientMessagesReceiver> clientMessagesReceiver;
    std::unique_ptr<GameClient> client;
    std::unique_ptr<Window> window;

    std::unique_ptr<ClientStateMachine> clientStateMachine;

    std::unique_ptr<EntityDrawStrategy> entityDrawStrategy;
    std::unique_ptr<WeaponDrawStrategy> weaponDrawStrategy;
    std::unique_ptr<ProjectileDrawStrategy> projectileDrawStrategy;
    std::unique_ptr<AreaOfEffectDrawStrategy> areaOfEffectDrawStrategy;
    std::unique_ptr<ItemDrawStrategy> itemDrawStrategy;

    void drawGameLoop(GraphicsContext& graphicsContext);

public:
    ClientApplication();
    ~ClientApplication();

    void draw(GraphicsContext& graphicsContext, bool& quit);
    void update(int64_t timeSinceLastFrame, bool& quit);
    void initialise(void);
    void run(void);
};