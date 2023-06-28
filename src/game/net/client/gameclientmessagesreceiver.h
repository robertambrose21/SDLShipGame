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
    std::shared_ptr<TurnController> turnController;

    void receiveTestMessage(int data);
    void receiveGameStateUpdate(const GameStateUpdate& update);
    void receiveSetParticipant(int participantId, int numParticipantsToSet, bool isPlayer);
    void receiveLoadMap(const MapBlock& block);
    void receiveFindPath(
        uint32_t entityId,
        const glm::ivec2& position,
        int shortStopSteps
    );
    void receiveAttackEntity(
        uint32_t entityId, 
        int x,
        int y,
        uint32_t weaponId
    );
    void receiveActionsRollResponse(int participantId, int numDice, DiceActionResult dice[64]);

public:
    GameClientMessagesReceiver(const std::shared_ptr<ApplicationContext>& context);

    void setPlayerController(const std::shared_ptr<PlayerController>& playerController);
    void setTransmitter(const std::shared_ptr<GameClientMessagesTransmitter>& transmitter);

    void receiveMessage(yojimbo::Message* message) override;
};