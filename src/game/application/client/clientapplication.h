#pragma once

#include "game/net/client/gameclientmessagesreceiver.h"
#include "game/net/client/gameclientmessagestransmitter.h"
#include "core/net/client/gameclient.h"
#include "game/application/application.h"
#include "clientstatemachine.h"
#include "clientstates.h"

class ClientApplication {
private:
    std::shared_ptr<PlayerController> playerController;
    std::shared_ptr<GameClientMessagesTransmitter> clientMessagesTransmitter;
    std::shared_ptr<GameClientMessagesReceiver> clientMessagesReceiver;
    std::shared_ptr<GameClient> client;

    std::shared_ptr<ClientStateMachine> clientStateMachine;

    std::shared_ptr<EntityPool> entityPool;
    std::shared_ptr<ProjectilePool> projectilePool;
    std::shared_ptr<AreaOfEffectPool> areaOfEffectPool;
    std::shared_ptr<TurnController> turnController;

public:
    ClientApplication();
    ~ClientApplication();

    void draw(std::shared_ptr<GraphicsContext> graphicsContext, bool& quit);
    void update(uint32_t timeSinceLastFrame, bool& quit);
    void initialise(void);
    void run(void);
};