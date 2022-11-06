#pragma once

#include "game/net/client/gameclientmessagesreceiver.h"
#include "game/net/client/gameclientmessagestransmitter.h"
#include "core/net/client/gameclient.h"
#include "application.h"

class ClientApplication {
private:
    std::shared_ptr<PlayerController> playerController;
    std::shared_ptr<GameClientMessagesTransmitter> clientMessagesTransmitter;
    std::shared_ptr<GameClientMessagesReceiver> clientMessagesReceiver;
    std::shared_ptr<GameClient> client;

public:
    ClientApplication();
    ~ClientApplication();

    void initialise(void);
    void run(void);
};