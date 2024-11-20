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

// TODO: Remove?
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

        // server.sendMessage(clientIndex, message);
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
        message->turnToEngageOn = turnController->getTurnNumber();

        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::onPublish(const Event<AreaOfEffectEventData>& event) {
    for(auto [participantId, clientIndex] : turnController->getAllAttachedClients()) {
        ApplyDamageMessage* message = (ApplyDamageMessage*) server.createMessage(clientIndex, GameMessageType::APPLY_DAMAGE);

        message->fromId = event.data.aoe->getOwnerId();
        message->targetId = event.data.target->getId();
        message->source = (uint8_t) DamageType::AOE;
        message->damage = event.data.damage;

        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::onPublish(const Event<ProjectileEventData>& event) {
    // TODO: Handle status effects
    if(event.data.target == nullptr) {
        return;
    }

    for(auto [participantId, clientIndex] : turnController->getAllAttachedClients()) {
        ApplyDamageMessage* message = (ApplyDamageMessage*) server.createMessage(clientIndex, GameMessageType::APPLY_DAMAGE);

        message->fromId = event.data.projectile->getOwnerId();
        message->targetId = event.data.target->getId();
        message->source = (uint8_t) DamageType::PROJECTILE;
        message->damage = event.data.damage;

        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::onPublish(const Event<MeleeWeaponEventData>& event) {
    for(auto [participantId, clientIndex] : turnController->getAllAttachedClients()) {
        ApplyDamageMessage* message = (ApplyDamageMessage*) server.createMessage(clientIndex, GameMessageType::APPLY_DAMAGE);

        message->fromId = event.data.owner->getParticipantId();
        message->targetId = event.data.target->getId();
        message->source = (uint8_t) DamageType::MELEE;
        message->damage = event.data.damage;

        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::onPublish(const Event<EntityEffectEvent>& event) {
    for(auto [participantId, clientIndex] : turnController->getAllAttachedClients()) {
        ApplyEntityEffectMessage* message = 
            (ApplyEntityEffectMessage*) server.createMessage(clientIndex, GameMessageType::APPLY_ENTITY_EFFECT);
        
        message->type = event.data.type;
        message->targetId = event.data.target->getId();
        message->effectStats.duration = event.data.stats.duration;
        message->effectStats.effectType = event.data.stats.type;
        message->effectStats.numDamageTicks = event.data.stats.damageTicks.size();

        for(int i = 0; i < event.data.stats.damageTicks.size(); i++) {
            message->effectStats.damageTicks[i] = event.data.stats.damageTicks[i];
        }

        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::onPublish(const Event<GridEffectEvent>& event) {
    for(auto [participantId, clientIndex] : turnController->getAllAttachedClients()) {
        ApplyGridEffectMessage* message = 
            (ApplyGridEffectMessage*) server.createMessage(clientIndex, GameMessageType::APPLY_GRID_EFFECT);

        message->type = event.data.type;
        message->x = event.data.x;
        message->y = event.data.y;
        message->duration = event.data.duration;

        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::onPublish(const Event<TilesRevealedEventData>& event) {
    auto clientIndex = turnController->getAttachedClient(event.data.participantId);

    if(clientIndex == -1) {
        return;
    }

    // TODO: Generic block message structure
    auto totalTiles = event.data.tiles.size();
    auto numMessagesToSend = totalTiles / 64;
    auto numTilesLastMessage = totalTiles % 64;

    if(numTilesLastMessage > 0) {
        numMessagesToSend++;
    }
    else {
        numTilesLastMessage = 64;
    }

    for(int i = 0; i < numMessagesToSend; i++) {
        TilesRevealedMessage* message = (TilesRevealedMessage*) server.createMessage(clientIndex, GameMessageType::TILES_REVEALED);
        message->participantId = event.data.participantId;
        message->numRevealedTiles = (i == numMessagesToSend - 1) ? numTilesLastMessage : 64;

        for(int j = 0; j < message->numRevealedTiles; j++) {
            auto tilesIndex = (i * 64) + j;

            message->revealedTiles[j].id = event.data.tiles[tilesIndex].id;
            message->revealedTiles[j].x = event.data.tiles[tilesIndex].x;
            message->revealedTiles[j].y = event.data.tiles[tilesIndex].y;
        }

        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::onPublish(const Event<EntitySetPositionEventData>& event) {
    for(auto [participantId, clientIndex] : turnController->getAllAttachedClients()) {
        SetEntityPositionMessage* message =
            (SetEntityPositionMessage*) server.createMessage(clientIndex, GameMessageType::SET_ENTITY_POSITION);

        message->entityId = event.data.entity->getId();
        message->movesLeft = event.data.entity->getMovesLeft();
        message->x = event.data.position.x;
        message->y = event.data.position.y;

        // TODO: Only send if entity is visible to the participant, same with attack?
        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::onPublish(const Event<EntityVisibilityToParticipantData>& event) {
    int clientIndex = turnController->getAttachedClient(event.data.participantId);

    if(clientIndex == -1) {
        return;
    }

    if(event.data.isVisible) {
        AddEntityVisibilityMessage* message =
            (AddEntityVisibilityMessage*) server.createMessage(clientIndex, GameMessageType::ADD_ENTITY_VISIBILITY);

        message->entity = EntityStateUpdate::serialize(event.data.entity);
        message->participantId = event.data.participantId;
        server.sendMessage(clientIndex, message);
    } 
    else {
        RemoveEntityVisibilityMessage* message = 
            (RemoveEntityVisibilityMessage*) server.createMessage(clientIndex, GameMessageType::REMOVE_ENTITY_VISIBILITY);

        message->entityId = event.data.entity->getId();
        message->participantId = event.data.participantId;

        server.sendMessage(clientIndex, message);
    }
}

void GameServerMessagesTransmitter::sendSetParticipant(
    int clientIndex, 
    Participant* participant
) {
    SetParticipantMessage* message = (SetParticipantMessage*) server.createMessage(clientIndex, GameMessageType::SET_PARTICIPANT);

    message->participantId = participant->getId();
    message->numParticipantsToSet = turnController->getParticipants().size();
    message->isPlayer = participant->getIsPlayer();

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