#pragma once

#include "core/net/server/gameserver.h"
#include "game/net/server/gameservermessagesreceiver.h"
#include "game/net/server/gameservermessagestransmitter.h"
#include "game/entities/behaviour/chaseandattackstrategy.h"
#include "game/application/application.h"

class ServerApplication {
private:
    std::shared_ptr<GameServer> server;
    std::shared_ptr<GameServerMessagesTransmitter> transmitter;
    std::shared_ptr<GameServerMessagesReceiver> receiver;

    std::shared_ptr<EntityPool> entityPool;
    std::shared_ptr<ProjectilePool> projectilePool;
    std::shared_ptr<AreaOfEffectPool> areaOfEffectPool;
    std::shared_ptr<TurnController> turnController;
    
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
    std::shared_ptr<Entity> addPlayer(glm::ivec2 position);
};