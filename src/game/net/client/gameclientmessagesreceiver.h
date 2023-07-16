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
    ApplicationContext& context;
    GameClientMessagesTransmitter* transmitter;
    PlayerController* playerController;

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
    GameClientMessagesReceiver(ApplicationContext& context);

    void setPlayerController(PlayerController* playerController);
    void setTransmitter(GameClientMessagesTransmitter* transmitter);

    void receiveMessage(yojimbo::Message* message) override;
};