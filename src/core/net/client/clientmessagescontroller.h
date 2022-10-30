#pragma once

#include "core/glmimport.h"
#include "core/net/message/messages.h"
#include "gameclient.h"

class ClientMessagesController {
private:
    std::shared_ptr<GameClient> client;

public:
    ClientMessagesController(std::shared_ptr<GameClient> client);

    void sendFindPathMessage(const glm::ivec2& position);
};