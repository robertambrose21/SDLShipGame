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
    void sendSelectEntityMessage(const uint32_t& entityId);
    void sendAttackEntityMessage(
        const uint32_t& entityId, 
        const uint32_t& targetId, 
        const uint32_t& weaponId
    );
};