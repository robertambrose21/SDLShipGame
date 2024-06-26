#pragma once

#include <iostream>
#include <sstream>
#include "game/net/messages.h"
#include "game/application/applicationcontext.h"
#include "core/net/clientmessagesreceiver.h"
#include "core/net/gameadapter.h"
#include "player/playercontroller.h"
#include "application/clientturncontroller.h"
#include "game/actions/attackaction.h"

class GameClientMessagesReceiver :
    public ClientMessagesReceiver,
    public EventPublisher<ApplyDamageEventData>
{
private:
    ApplicationContext& context;
    GameClientMessagesTransmitter* transmitter;
    PlayerController* playerController;
    std::map<int, bool> walkableTileIds;

    void receiveTestMessage(int data);
    void receiveGameStateUpdate(const GameStateUpdate& update);
    void receiveSetParticipant(int participantId, int numParticipantsToSet, bool isPlayer);
    void receiveLoadMap(const MapBlock& block);
    void receiveFindPath(
        uint32_t entityId,
        const glm::ivec2& position,
        int shortStopSteps,
        int turnNumber
    );
    void receiveAttackEntity(
        uint32_t entityId, 
        int x,
        int y,
        uint8_t weaponIdBytes[16],
        int turnNumber
    );
    void receiveNextTurn(int participantId, int turnNumber);
    void receiveSpawnItems(
        const glm::ivec2& position,
        uint32_t ownerId,
        int numItems,
        ItemUpdate items[64]
    );
    void receiveTakeItems(
        uint32_t entityId,
        int numItems,
        ItemUpdate items[64],
        int turnNumber
    );
    void receiveEngagement(int participantIdA, int participantIdB, int type);
    void receiveApplyDamageMessage(int fromId, uint32_t targetId, uint8_t source, int damage);
    void receiveApplyEntityEffectMessage(uint8_t type, uint32_t targetId, const EffectStatsUpdate& effectStats);
    void receiveApplyGridEffectMessage(uint8_t type, int x, int y, uint8_t duration);

public:
    GameClientMessagesReceiver(ApplicationContext& context, const std::map<int, bool>& walkableTileIds);

    void setPlayerController(PlayerController* playerController);
    void setTransmitter(GameClientMessagesTransmitter* transmitter);

    void receiveMessage(yojimbo::Message* message) override;
};