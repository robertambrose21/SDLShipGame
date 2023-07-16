#pragma once

#include "core/net/server/gameserver.h"
#include "game/net/server/gameservermessagesreceiver.h"
#include "game/net/server/gameservermessagestransmitter.h"
#include "game/entities/behaviour/chaseandattackstrategy.h"
#include "game/application/application.h"

class ServerApplication {
private:
    std::unique_ptr<GameServer> server;
    std::unique_ptr<GameServerMessagesTransmitter> transmitter;
    std::unique_ptr<GameServerMessagesReceiver> receiver;
    
    std::map<int, int> participantToClientIndex;

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
    Entity* addPlayer(glm::ivec2 position, bool hasFreezeGun);
};