#pragma once

#include <iostream>
#include <sstream>
#include "game/net/messages.h"
#include "game/application/applicationcontext.h"
#include "core/net/clientmessagesreceiver.h"
#include "core/net/gameadapter.h"
#include "player/playercontroller.h"
#include "application/clientturncontroller.h"

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
    void receiveNextTurn(int participantId, int turnNumber);

public:
    GameClientMessagesReceiver(ApplicationContext& context);

    void setPlayerController(PlayerController* playerController);
    void setTransmitter(GameClientMessagesTransmitter* transmitter);

    void receiveMessage(yojimbo::Message* message) override;
};