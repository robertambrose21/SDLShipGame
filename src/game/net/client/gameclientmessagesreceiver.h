#pragma once

#include <iostream>
#include "game/net/gamestateupdate.h"
#include "core/net/client/clientmessagesreceiver.h"
#include "core/net/gameadapter.h"

class GameClientMessagesReceiver : public ClientMessagesReceiver {
private:
    void receiveTestMessage(int data);
    void receiveGameStateUpdate(GameStateUpdate update);

public:
    GameClientMessagesReceiver();

    void receiveMessage(yojimbo::Message* message) override;
};