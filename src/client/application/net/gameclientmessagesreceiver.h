#pragma once

#include <iostream>
#include <sstream>
#include "game/net/messages.h"
#include "game/application/applicationcontext.h"
#include "core/net/clientmessagesreceiver.h"
#include "core/net/gameadapter.h"
#include "player/playercontroller.h"
#include "application/clientgamecontroller.h"
#include "game/actions/attackaction.h"
#include "game/net/gamemessagelogger.h"

class GameClientMessagesReceiver :
    public ClientMessagesReceiver,
    public EventPublisher<ApplyDamageEventData>
{
public:
    GameClientMessagesReceiver(
        ApplicationContext& context,
        uint64_t clientId,
        const std::map<unsigned, bool>& walkableTileIds
    );

    void setPlayerController(PlayerController* playerController);
    void setTransmitter(GameClientMessagesTransmitter* transmitter);

    void receiveMessage(yojimbo::Message* message) override;

private:
    ApplicationContext& context;
    GameClientMessagesTransmitter* transmitter;
    PlayerController* playerController;

    uint64_t clientId;
    std::map<unsigned, bool> walkableTileIds;

    void receiveTestMessage(GameTestMessage* message);
    void receiveGameStateUpdate(GameStateUpdateMessage* message);
    void receiveSetParticipant(SetParticipantMessage* message);
    void receiveLoadMap(LoadMapMessage* message);
    void receiveFindPath(FindPathMessage* message);
    void receiveAttackEntity(AttackMessage* message);
    void receiveNextTurn(NextTurnMessage* message);
    void receiveSetTurn(SetTurnMessage* message);
    void receiveSpawnItems(SpawnItemsMessage* message);
    void receiveTakeItems(TakeItemsMessage* message);
    void receiveEngagement(EngagementMessage* message);
    void receiveApplyDamageMessage(ApplyDamageMessage* message);
    void receiveApplyEntityEffectMessage(ApplyEntityEffectMessage* message);
    void receiveApplyGridEffectMessage(ApplyGridEffectMessage* message);
    void receiveTilesRevealedMessage(TilesRevealedMessage* message);
    void receiveSetEntityPositionMessage(SetEntityPositionMessage* message);
    void receiveAddEntityVisibilityMessage(AddEntityVisibilityMessage* message);
    void receiveRemoveEntityVisibilityMessage(RemoveEntityVisibilityMessage* message);

    // Engagemnets
    void receiveCreateEngagementMessage(CreateEngagementMessage* message);
    void receiveAddToEngagementMessage(AddToEngagementMessage* message);
    void receiveDisenageMessage(DisengageMessage* message);
    void receiveRemoveEngagementMessage(RemoveEngagementMessage* message);
    void receiveMergeEngagementsMessage(MergeEngagementsMessage* message);
};