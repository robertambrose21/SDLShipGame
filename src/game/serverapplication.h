#pragma once

#include "../core/gameserver.h"
#include "application.h"

class ServerApplication {
private:
    std::shared_ptr<GameServer> server;

public:
    ServerApplication();
    ~ServerApplication();

    void initialise(void);
    void run(void);
};