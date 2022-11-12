#pragma once

#include <iostream>
#include <sstream>
#include "game/net/messages.h"
#include "game/application/applicationcontext.h"
#include "core/net/client/clientmessagesreceiver.h"
#include "core/net/gameadapter.h"
#include "player/playercontroller.h"

class GameClientMessagesReceiver : public ClientMessagesReceiver {
private:
    std::shared_ptr<ApplicationContext> context;
    std::shared_ptr<PlayerController> playerController;

    void receiveTestMessage(int data);
    void receiveGameStateUpdate(GameStateUpdate update);
    void receiveSetParticipant(int participantId);
    void receiveLoadMap(MapBlock block);

public:
    GameClientMessagesReceiver(std::shared_ptr<ApplicationContext> context);

    void setPlayerController(std::shared_ptr<PlayerController> playerController);

    void receiveMessage(yojimbo::Message* message) override;
};