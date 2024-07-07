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
#include "game/net/gamemessagelogger.h"

class GameClientMessagesReceiver :
    public ClientMessagesReceiver,
    public EventPublisher<ApplyDamageEventData>
{
private:
    ApplicationContext& context;
    GameClientMessagesTransmitter* transmitter;
    PlayerController* playerController;
    std::map<int, bool> walkableTileIds;

    void receiveTestMessage(GameTestMessage* message);
    void receiveGameStateUpdate(GameStateUpdateMessage* message);
    void receiveSetParticipant(SetParticipantMessage* message);
    void receiveLoadMap(LoadMapMessage* message);
    void receiveFindPath(FindPathMessage* message);
    void receiveAttackEntity(AttackMessage* message);
    void receiveNextTurn(NextTurnMessage* message);
    void receiveSpawnItems(SpawnItemsMessage* message);
    void receiveTakeItems(TakeItemsMessage* message);
    void receiveEngagement(EngagementMessage* message);
    void receiveApplyDamageMessage(ApplyDamageMessage* message);
    void receiveApplyEntityEffectMessage(ApplyEntityEffectMessage* message);
    void receiveApplyGridEffectMessage(ApplyGridEffectMessage* message);
    void receiveTilesRevealedMessage(TilesRevealedMessage* message);

public:
    GameClientMessagesReceiver(
        ApplicationContext& context, 
        const std::map<int, bool>& walkableTileIds
    );

    void setPlayerController(PlayerController* playerController);
    void setTransmitter(GameClientMessagesTransmitter* transmitter);

    void receiveMessage(yojimbo::Message* message) override;
};