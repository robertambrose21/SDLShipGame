#pragma once

#include "core/net/client/clientmessagescontroller.h"
#include "core/net/client/gameclient.h"
#include "application.h"

class ClientApplication {
private:
    std::shared_ptr<PlayerController> playerController;
    std::shared_ptr<ClientMessagesController> clientMessagesController;
    std::shared_ptr<GameClient> client;

public:
    ClientApplication();
    ~ClientApplication();

    void initialise(void);
    void run(void);
};