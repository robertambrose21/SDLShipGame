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
    std::shared_ptr<GameClientMessagesTransmitter> transmitter;
    std::shared_ptr<PlayerController> playerController;

    void receiveTestMessage(const int& data);
    void receiveGameStateUpdate(const GameStateUpdate& update);
    void receiveSetParticipant(const int& participantId, const int& numParticipantsToSet, const bool& isPlayer);
    void receiveLoadMap(const MapBlock& block);
    void receiveFindPath(
        const uint32_t& entityId,
        const glm::ivec2& position,
        const int& shortStopSteps
    );
    void receiveAttackEntity(
        const uint32_t& entityId, 
        const uint32_t& targetId, 
        const uint32_t& weaponId
    );

public:
    GameClientMessagesReceiver(const std::shared_ptr<ApplicationContext>& context);

    void setPlayerController(const std::shared_ptr<PlayerController>& playerController);
    void setTransmitter(const std::shared_ptr<GameClientMessagesTransmitter>& transmitter);

    void receiveMessage(yojimbo::Message* message) override;
};