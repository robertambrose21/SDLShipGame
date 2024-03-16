#pragma once

#include "core/net/gameserver.h"
#include "application/net/gameservermessagesreceiver.h"
#include "application/net/gameservermessagestransmitter.h"
#include "game/entities/behaviour/chaseandattackstrategy.h"
#include "core/grid/generation/emptystrategy.h"
#include "core/grid/generation/hallstrategy.h"
#include "core/grid/generation/wavefunctioncollapsestrategy.h"
#include "game/application/application.h"
#include "game/application/stdoutsubscriber.h"
#include "serverturncontroller.h"

class ServerApplication {
private:
    std::unique_ptr<Application> application;

    std::unique_ptr<GameServerMessagesTransmitter> transmitter;
    std::unique_ptr<GameServer> server;
    std::unique_ptr<GameServerMessagesReceiver> receiver;
    StdOutSubscriber stdoutSubscriber;

    void sendLoadMapToClient(int clientIndex);
    void sendGameStateUpdatesToClients(void);

    void onClientConnect(int clientIndex);

public:
    ServerApplication();
    ~ServerApplication();

    void initialise(void);
    void run(void);

    // Temp
    void loadMap(void);
    // Temp
    void loadGame(void);
    // Temp
    Entity* addPlayer(bool hasFreezeGun);
};