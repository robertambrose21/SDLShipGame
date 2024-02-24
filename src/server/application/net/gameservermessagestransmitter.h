#pragma once

#include <map>

#include "game/net/messages.h"
#include "core/net/gameserver.h"
#include "core/net/servermessagestransmitter.h"
#include "game/application/turncontroller.h"
#include "game/application/application.h"

class ServerTurnController;

class GameServerMessagesTransmitter : 
    public ServerMessagesTransmitter, 
    public EventSubscriber<ItemEventData>,
    public EventSubscriber<MoveActionEventData>,
    public EventSubscriber<AttackActionEventData>,
    public EventSubscriber<TakeItemActionEventData>,
    public EventSubscriber<EngagementEventData>,
    public EventSubscriber<AreaOfEffectEventData>,
    public EventSubscriber<ProjectileEventData>,
    public EventSubscriber<MeleeWeaponEventData>
{
private:
    GameServer& server;
    ServerTurnController* turnController;

    std::function<void(int)> onClientConnectFunc;

    void onClientConnected(int clientIndex) override;

public:
    GameServerMessagesTransmitter(
        GameServer& server,
        ServerTurnController* turnController,
        std::function<void(int)> onClientConnectFunc = [](int) { }
    );

    void onPublish(const Event<ItemEventData>& event);
    void onPublish(const Event<MoveActionEventData>& event);
    void onPublish(const Event<AttackActionEventData>& event);
    void onPublish(const Event<TakeItemActionEventData>& event);
    void onPublish(const Event<EngagementEventData>& event);
    void onPublish(const Event<AreaOfEffectEventData>& event);
    void onPublish(const Event<ProjectileEventData>& event);
    void onPublish(const Event<MeleeWeaponEventData>& event);

    void sendSetParticipant(int clientIndex, TurnController::Participant* participant);
    void sendGameStateUpdate(int clientIndex, const GameStateUpdate& update);
    void sendLoadMap(int clientIndex, const MapBlock& block);
    void sendActionsRollResponse(int clientIndex, int participantId, const std::vector<DiceActionResult>& dice);
    void sendNextTurn(int clientIndex, int participantId, int turnNumber);
};
