#pragma once

#include "../core/gameclient.h"
#include "application.h"

class ClientApplication {
private:
    std::shared_ptr<GameClient> client;

public:
    ClientApplication();
    ~ClientApplication();

    void initialise(void);
    void run(void);
};