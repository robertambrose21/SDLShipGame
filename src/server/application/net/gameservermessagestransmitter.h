#pragma once

#include <map>

#include "game/net/messages.h"
#include "core/net/gameserver.h"
#include "core/net/servermessagestransmitter.h"
#include "game/application/gamecontroller.h"
#include "game/application/application.h"
#include "game/application/visibilitycontroller.h"

class ServerGameController;

// TODO: Break this up into multiple transmitters
class GameServerMessagesTransmitter : 
    public ServerMessagesTransmitter, 
    public EventSubscriber<ItemEventData>,
    public EventSubscriber<MoveActionEventData>,
    public EventSubscriber<AttackActionEventData>,
    public EventSubscriber<TakeItemActionEventData>,
    public EventSubscriber<AreaOfEffectEventData>,
    public EventSubscriber<ProjectileEventData>,
    public EventSubscriber<MeleeWeaponEventData>,
    public EventSubscriber<ActorEffectEvent>,
    public EventSubscriber<GridEffectEvent>,
    public EventSubscriber<TilesRevealedEventData>,
    public EventSubscriber<ActorSetPositionEventData>,
    public EventSubscriber<ActorVisibilityToParticipantData>,
    public EventSubscriber<CreateEngagementEventData>,
    public EventSubscriber<AddToEngagementEventData>,
    public EventSubscriber<DisengageEventData>,
    public EventSubscriber<RemoveEngagementEventData>,
    public EventSubscriber<MergeEngagementEventData>,
    public EventSubscriber<CreateFactionEventData>,
    public EventSubscriber<SetFactionEventData>,
    public EventSubscriber<AddFactionEventData>,
    public EventSubscriber<ChangeFactionAlignmentEventData>
{
public:
    GameServerMessagesTransmitter(
        GameServer& server,
        ServerGameController* gameController,
        VisiblityController* visibilityController,
        ItemController* itemController,
        std::function<void(int)> onClientConnectFunc = [](int) { },
        std::function<void(int)> onClientDisconnectFunc = [](int) { }
    );

    void onPublish(const Event<ItemEventData>& event);
    void onPublish(const Event<MoveActionEventData>& event);
    void onPublish(const Event<AttackActionEventData>& event);
    void onPublish(const Event<TakeItemActionEventData>& event);
    void onPublish(const Event<AreaOfEffectEventData>& event);
    void onPublish(const Event<ProjectileEventData>& event);
    void onPublish(const Event<MeleeWeaponEventData>& event);
    void onPublish(const Event<ActorEffectEvent>& event);
    void onPublish(const Event<GridEffectEvent>& event);
    void onPublish(const Event<TilesRevealedEventData>& event);
    void onPublish(const Event<ActorSetPositionEventData>& event);
    void onPublish(const Event<ActorVisibilityToParticipantData>& event);
    void onPublish(const Event<CreateEngagementEventData>& event);
    void onPublish(const Event<AddToEngagementEventData>& event);
    void onPublish(const Event<DisengageEventData>& event);
    void onPublish(const Event<RemoveEngagementEventData>& event);
    void onPublish(const Event<MergeEngagementEventData>& event);
    void onPublish(const Event<CreateFactionEventData>& event);
    void onPublish(const Event<SetFactionEventData>& event);
    void onPublish(const Event<AddFactionEventData>& event);
    void onPublish(const Event<ChangeFactionAlignmentEventData>& event);
    
    void sendSetParticipant(int clientIndex, Participant* participant);
    void sendSetParticipantToAllClients(Participant* participant);
    void sendGameStateUpdate(int clientIndex, const GameStateUpdate& update);
    void sendLoadMap(int clientIndex, const MapBlock& block);
    void sendActionsRollResponse(int clientIndex, int participantId, const std::vector<DiceActionResult>& dice);
    void sendNextTurn(int clientIndex, int participantId, uint32_t engagementId, int turnNumber);
    void sendNextTurnToAllClients(int participantId, uint32_t engagementId, int turnNumber);
    void sendItems(int clientIndex, const std::vector<Item*>& items);
    void sendFactionUpdates(int clientIndex, const std::vector<Factioned::Faction>& factions);

    void sendLoadGameToClient(int clientIndex);

private:
    GameServer& server;
    ServerGameController* gameController;
    VisiblityController* visibilityController;
    ItemController* itemController;

    std::function<void(int)> onClientConnectFunc;
    std::function<void(int)> onClientDisconnectFunc;

    void onClientConnected(int clientIndex) override;
    void onClientDisconnected(int clientIndex) override;

    void sendRevealedTiles(const std::vector<RevealedTile>& tiles, int participantId);
};
