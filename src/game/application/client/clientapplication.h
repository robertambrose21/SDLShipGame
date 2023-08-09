#pragma once

#include "game/net/client/gameclientmessagesreceiver.h"
#include "game/net/client/gameclientmessagestransmitter.h"
#include "core/net/client/gameclient.h"
#include "game/application/application.h"
#include "clientstatemachine.h"
#include "clientstates.h"

class ClientApplication {
private:
    std::unique_ptr<PlayerController> playerController;
    std::unique_ptr<GameClientMessagesTransmitter> clientMessagesTransmitter;
    std::unique_ptr<GameClientMessagesReceiver> clientMessagesReceiver;
    std::unique_ptr<GameClient> client;

    std::unique_ptr<ClientStateMachine> clientStateMachine;

public:
    ClientApplication();
    ~ClientApplication();

    void draw(GraphicsContext& graphicsContext, bool& quit);
    void update(uint32_t timeSinceLastFrame, bool& quit);
    void initialise(void);
    void run(void);
};