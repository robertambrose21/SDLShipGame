#pragma once

#include <map>

#include "game/net/messages.h"
#include "core/net/gameserver.h"
#include "core/net/servermessagestransmitter.h"
#include "game/application/turncontroller.h"
#include "game/application/application.h"
#include "game/application/visibilitycontroller.h"

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
    public EventSubscriber<MeleeWeaponEventData>,
    public EventSubscriber<EntityEffectEvent>,
    public EventSubscriber<GridEffectEvent>,
    public EventSubscriber<TilesRevealedEventData>,
    public EventSubscriber<EntitySetPositionEventData>,
    public EventSubscriber<EntityVisibilityToParticipantData>
{
public:
    GameServerMessagesTransmitter(
        GameServer& server,
        ServerTurnController* turnController,
        VisiblityController* visibilityController,
        ItemController* itemController,
        std::function<void(int)> onClientConnectFunc = [](int) { },
        std::function<void(int)> onClientDisconnectFunc = [](int) { }
    );

    void onPublish(const Event<ItemEventData>& event);
    void onPublish(const Event<MoveActionEventData>& event);
    void onPublish(const Event<AttackActionEventData>& event);
    void onPublish(const Event<TakeItemActionEventData>& event);
    void onPublish(const Event<EngagementEventData>& event);
    void onPublish(const Event<AreaOfEffectEventData>& event);
    void onPublish(const Event<ProjectileEventData>& event);
    void onPublish(const Event<MeleeWeaponEventData>& event);
    void onPublish(const Event<EntityEffectEvent>& event);
    void onPublish(const Event<GridEffectEvent>& event);
    void onPublish(const Event<TilesRevealedEventData>& event);
    void onPublish(const Event<EntitySetPositionEventData>& event);
    void onPublish(const Event<EntityVisibilityToParticipantData>& event);

    void sendSetParticipant(int clientIndex, Participant* participant);
    void sendSetParticipantToAllClients(Participant* participant);
    void sendGameStateUpdate(int clientIndex, const GameStateUpdate& update);
    void sendLoadMap(int clientIndex, const MapBlock& block);
    void sendActionsRollResponse(int clientIndex, int participantId, const std::vector<DiceActionResult>& dice);
    void sendNextTurn(int clientIndex, int participantId, int turnNumber);
    void sendNextTurnToAllClients(int participantId, int turnNumber);
    void sendSetTurn(int clientIndex, uint8_t currentParticipantId, uint32_t turnNumber);
    void sendItems(int clientIndex, const std::vector<Item*>& items);

    void sendLoadGameToClient(int clientIndex);

private:
    GameServer& server;
    ServerTurnController* turnController;
    VisiblityController* visibilityController;
    ItemController* itemController;

    std::function<void(int)> onClientConnectFunc;
    std::function<void(int)> onClientDisconnectFunc;

    void onClientConnected(int clientIndex) override;
    void onClientDisconnected(int clientIndex) override;

    void sendRevealedTiles(const std::vector<RevealedTile>& tiles, int participantId);
    void sendEngagement(
        int participantIdA, 
        int participantIdB, 
        int turnToEngageOn,
        EngagementType type,
        int clientIndex
    );
};
