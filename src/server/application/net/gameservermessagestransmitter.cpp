#include "gameservermessagestransmitter.h"
#include "application/serverturncontroller.h"

GameServerMessagesTransmitter::GameServerMessagesTransmitter(
    GameServer& server,
    ServerTurnController* turnController,
    std::function<void(int)> onClientConnectFunc
) :
    server(server),
    onClientConnectFunc(onClientConnectFunc),
    turnController(turnController)
{ }

void GameServerMessagesTransmitter::onClientConnected(int clientIndex) {
    onClientConnectFunc(clientIndex);
}

void GameServerMessagesTransmitter::onPublish(const Event<ItemEventData>& event) {
    if(event.data.type != ItemEventData::SPAWN) {
        return;
    }

    std::map<glm::ivec2, std::vector<Item*>> itemGroups;

    for(auto item : event.data.items) {
        itemGroups[item->getPosition()].push_back(item);
    }
    
    for(auto [_, clientIndex] : turnController->getAllAttachedClients()) {
        // TODO: handle items > 64
        for(auto [position, items] : itemGroups) {
            SpawnItemsMessage* message = (SpawnItemsMessage*) server.createMessage(0, GameMessageType::SPAWN_ITEMS);

            message->x = position.x;
            message->y = position.y;
            message->ownerId = event.data.owner->getId();
            message->numItems = items.size();

            for(int i = 0; i < items.size(); i++) {
                message->items[i].id = items[i]->getId();
                strcpy(message->items[i].name, items[i]->getName().data());
            }

            server.sendMessage(clientIndex, message);
        }
    }
}

void GameServerMessagesTransmitter::onPublish(const Event<MoveActionEventData>& event) {
    for(auto [participantId, clientIndex] : turnController->getAllAttachedClients()) {
        if(turnController->getAttachedClient(event.data.entity->getParticipantId()) == clientIndex) {
            return;
        }

        FindPathMessage* message = (FindPathMessage*) server.createMessage(clientIndex, GameMessageType::FIND_PATH);

        message->entityId = event.data.entity->getId();
        message->x = event.data.position.x;
        message->y = event.data.position.y;
        message->shortStopSteps = event.data.shortStopSteps;
        message->turnNumber = event.data.turnNumber;

        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::onPublish(const Event<AttackActionEventData>& event) {
    for(auto [participantId, clientIndex] : turnController->getAllAttachedClients()) {
        if(turnController->getAttachedClient(event.data.owner->getParticipantId()) == clientIndex) {
            return;
        }

        AttackMessage* message = (AttackMessage*) server.createMessage(clientIndex, GameMessageType::ATTACK_ENTITY);

        message->entityId = event.data.owner->getId();
        message->x = event.data.target.x;
        message->y = event.data.target.y;
        memcpy(message->weaponIdBytes, &event.data.weapon->getId().getBytes()[0], 16);
        message->turnNumber = event.data.turnNumber;

        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::onPublish(const Event<TakeItemActionEventData>& event) {
    for(auto [_, clientIndex] : turnController->getAllAttachedClients()) {
        TakeItemsMessage* message = (TakeItemsMessage*) server.createMessage(clientIndex, GameMessageType::TAKE_ITEMS);

        message->turnNumber = event.data.turnNumber;
        message->entityId = event.data.entity->getId();
        message->numItems = event.data.items.size();

        for(int i = 0; i < event.data.items.size(); i++) {
            message->items[i].id = event.data.items[i]->getId();
            strcpy(message->items[i].name, event.data.items[i]->getName().data());
        }

        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::onPublish(const Event<EngagementEventData>& event) {
    for(auto [participantId, clientIndex] : turnController->getAllAttachedClients()) {
        EngagementMessage* message = (EngagementMessage*) server.createMessage(clientIndex, GameMessageType::ENGAGEMENT);

        message->participantIdA = event.data.participantIdA;
        message->participantIdB = event.data.participantIdB;
        message->type = event.data.type;

        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::sendSetParticipant(
    int clientIndex, 
    TurnController::Participant* participant
) {
    SetParticipantMessage* message = (SetParticipantMessage*) server.createMessage(clientIndex, GameMessageType::SET_PARTICIPANT);

    message->participantId = participant->id;
    message->numParticipantsToSet = turnController->getParticipants().size();
    message->isPlayer = participant->isPlayer;

    server.sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendGameStateUpdate(int clientIndex, const GameStateUpdate& update) {
    GameStateUpdateMessage* message = (GameStateUpdateMessage*) server.createMessage(clientIndex, GameMessageType::GAME_STATE_UPDATE);

    message->gameStateUpdate = update;

    server.sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendLoadMap(int clientIndex, const MapBlock& block) {
    LoadMapMessage* message = (LoadMapMessage*) server.createMessage(clientIndex, GameMessageType::LOAD_MAP);

    message->mapBlock = block;

    server.sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendActionsRollResponse(
    int clientIndex, 
    int participantId, 
    const std::vector<DiceActionResult>& dice
) {  
    ActionsRollResponseMessage* message = 
        (ActionsRollResponseMessage*) server.createMessage(clientIndex, GameMessageType::ACTIONS_ROLL_RESPONSE);

    message->participantId = participantId;
    message->numDice = dice.size();
    for(int i = 0; i < dice.size(); i++) {
        message->dice[i] = dice[i];
    }

    server.sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendNextTurn(int clientIndex, int participantId, int turnNumber) {
    NextTurnMessage* message = (NextTurnMessage*) server.createMessage(clientIndex, GameMessageType::NEXT_TURN);

    message->participantId = participantId;
    message->turnNumber = turnNumber;

    server.sendMessage(clientIndex, message);
}