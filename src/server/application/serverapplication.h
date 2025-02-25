#pragma once

#include "core/net/gameserver.h"
#include "application/net/gameservermessagesreceiver.h"
#include "application/net/gameservermessagestransmitter.h"
#include "game/entities/behaviour/chaseandattackstrategy.h"
#include "core/grid/generation/emptystrategy.h"
#include "core/grid/generation/hallstrategy.h"
#include "core/grid/generation/wfcstrategy.h"
#include "game/application/application.h"
#include "game/application/stdoutsubscriber.h"
#include "game/spawn/spawncontroller.h"
#include "serverturncontroller.h"
#include "game/net/gamemessagelogger.h"
#include "game/application/visibilitycontroller.h"
#include "game/items/equippable.h"

class ServerApplication {
private:
    std::unique_ptr<Application> application;

    std::unique_ptr<GameServerMessagesTransmitter> transmitter;
    std::unique_ptr<GameServer> server;
    std::unique_ptr<GameServerMessagesReceiver> receiver;
    StdOutSubscriber stdoutSubscriber;

    void sendLoadMapToClient(int clientIndex);
    void sendGameStateUpdatesToParticipant(int clientIndex);

    void onClientConnect(int clientIndex);
    void onClientDisconnect(int clientIndex);

    std::vector<GenerationStrategy::Room> unfilledRooms;

public:
    ServerApplication();
    ~ServerApplication();

    void initialise(void);
    void run(void);

    // Temp
    std::vector<GenerationStrategy::Room> loadMap(void);
    // Temp
    void loadGame(const std::vector<GenerationStrategy::Room>& rooms);
    // Temp
    Entity* addPlayer(bool hasFreezeGun);
};