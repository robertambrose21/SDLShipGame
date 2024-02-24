#include "gameclientmessagesreceiver.h"

// Handle with events?
GameClientMessagesReceiver::GameClientMessagesReceiver(ApplicationContext& context) :
    context(context)
{ }

void GameClientMessagesReceiver::setTransmitter(GameClientMessagesTransmitter* transmitter) {
    this->transmitter = transmitter;
}

void GameClientMessagesReceiver::setPlayerController(PlayerController* playerController) {
    this->playerController = playerController;
}

void GameClientMessagesReceiver::receiveMessage(yojimbo::Message* message) {
    switch(message->GetType()) {
        case (int) GameMessageType::GAME_STATE_UPDATE: {
            GameStateUpdateMessage* updateMessage = (GameStateUpdateMessage*) message;
            receiveGameStateUpdate(updateMessage->gameStateUpdate);
            break;
        }

        case (int) GameMessageType::TEST_MESSAGE: {
            GameTestMessage* testMessage = (GameTestMessage*) message;
            receiveTestMessage(testMessage->data);
            break;
        }

        case (int) GameMessageType::SET_PARTICIPANT: {
            SetParticipantMessage* setParticipantMessage = (SetParticipantMessage*) message;
            receiveSetParticipant(
                setParticipantMessage->participantId,
                setParticipantMessage->numParticipantsToSet,
                setParticipantMessage->isPlayer
            );
            break;
        }

        case (int) GameMessageType::LOAD_MAP: {
            LoadMapMessage* loadMapMessage = (LoadMapMessage*) message;
            receiveLoadMap(loadMapMessage->mapBlock);
            break;
        }

        case (int) GameMessageType::FIND_PATH: {
            FindPathMessage* findPathMessage = (FindPathMessage*) message;
            receiveFindPath(
                findPathMessage->entityId,
                { findPathMessage->x, findPathMessage->y },
                findPathMessage->shortStopSteps,
                findPathMessage->turnNumber
            );
            break;
        }

        case (int) GameMessageType::ATTACK_ENTITY: {
            AttackMessage* attackMessage = (AttackMessage*) message;
            receiveAttackEntity(
                attackMessage->entityId,
                attackMessage->x,
                attackMessage->y,
                attackMessage->weaponIdBytes,
                attackMessage->turnNumber
            );
            break;
        }

        case (int) GameMessageType::NEXT_TURN: {
            NextTurnMessage* nextTurnMessage = (NextTurnMessage*) message;
            receiveNextTurn(nextTurnMessage->participantId, nextTurnMessage->turnNumber);
            break;
        }

        case (int) GameMessageType::SPAWN_ITEMS: {
            SpawnItemsMessage* spawnItemsMessage = (SpawnItemsMessage*) message;
            receiveSpawnItems(
                glm::ivec2(spawnItemsMessage->x, spawnItemsMessage->y),
                spawnItemsMessage->ownerId,
                spawnItemsMessage->numItems,
                spawnItemsMessage->items
            );
            break;
        }

        case (int) GameMessageType::TAKE_ITEMS: {
            TakeItemsMessage* takeItemsMessage = (TakeItemsMessage*) message;
            receiveTakeItems(
                takeItemsMessage->entityId,
                takeItemsMessage->numItems,
                takeItemsMessage->items,
                takeItemsMessage->turnNumber
            );
            break;
        }

        case (int) GameMessageType::ENGAGEMENT: {
            EngagementMessage* engagementMessage = (EngagementMessage*) message;
            receiveEngagement(engagementMessage->participantIdA, engagementMessage->participantIdB, engagementMessage->type);
            break;
        }

        case (int) GameMessageType::APPLY_DAMAGE: {
            ApplyDamageMessage* applyDamageMessage = (ApplyDamageMessage*) message;
            receiveApplyDamageMessage(
                applyDamageMessage->fromId, 
                applyDamageMessage->targetId, 
                applyDamageMessage->source, 
                applyDamageMessage->damage
            );
            break;
        }

        default:
            std::cout << "Received unhandled message: " << message << std::endl;
            break;
    }
}

void GameClientMessagesReceiver::receiveGameStateUpdate(const GameStateUpdate& update) {
    // std::cout << "Got game state update " << update.currentParticipantId << std::endl;
    context.getEntityPool()->addGameStateUpdate(update);
}

void GameClientMessagesReceiver::receiveTestMessage(int data) {
    std::cout << "Received test data " << data << std::endl;
}

void GameClientMessagesReceiver::receiveSetParticipant(
    int participantId,
    int numParticipantsToSet,
    bool isPlayer
) {
    auto turnController = context.getTurnController();
    auto const& participant = turnController->addParticipant(participantId, isPlayer, { }, nullptr, false);

    if(isPlayer) {
        playerController->setParticipant(participant);
    }

    if(numParticipantsToSet == turnController->getParticipants().size()) {
        turnController->allParticipantsSet();
    }

    transmitter->sendSetParticipantAckMessage(participantId);
}

void GameClientMessagesReceiver::receiveLoadMap(const MapBlock& block) {
    // TODO: Sequencing
    auto grid = context.getGrid();
    auto offset = block.sequence * MaxMapBlockSize;

    for(int i = 0; i < block.blockSize; i++) {
        auto x = (i + offset) / block.width;
        auto y = (i + offset) % block.width;
        auto id = block.data[i];

        grid->setTile(x, y, { id, id == 1 });
    }

    std::cout 
        << "Received map block sequence [" 
        << (block.sequence + 1)
        << "] of [" 
        << block.numSequences
        << "]"
        << std::endl;
}

void GameClientMessagesReceiver::receiveFindPath(
    uint32_t entityId, 
    const glm::ivec2& position,
    int shortStopSteps,
    int turnNumber
) {
    if(!context.getEntityPool()->hasEntity(entityId)) {
        return;
    }

    auto const& entity = context.getEntityPool()->getEntity(entityId);
    
    context.getTurnController()->queueAction(std::make_unique<MoveAction>(turnNumber, entity, position, shortStopSteps));
}

void GameClientMessagesReceiver::receiveAttackEntity(
    uint32_t entityId, 
    int x,
    int y,
    uint8_t weaponIdBytes[16],
    int turnNumber
) {
    auto entityPool = context.getEntityPool();

    if(!entityPool->hasEntity(entityId)) {
        return;
    }

    auto weaponId = UUID::fromBytes(weaponIdBytes);
    auto const& entity = entityPool->getEntity(entityId);

    for(auto weapon : entity->getWeapons()) {
        if(weapon->getId() == weaponId) {
            context.getTurnController()->queueAction(
                std::make_unique<AttackAction>(
                    turnNumber, 
                    entity, 
                    weapon, 
                    glm::ivec2(x, y), 
                    true
                )
            );
        }
    }
}

void GameClientMessagesReceiver::receiveNextTurn(int participantId, int turnNumber) {
    dynamic_cast<ClientTurnController*>(context.getTurnController())->receiveSetNextTurnFlag(participantId, turnNumber);
}

void GameClientMessagesReceiver::receiveSpawnItems(
    const glm::ivec2& position, 
    uint32_t ownerId,
    int numItems, 
    ItemUpdate items[64]
) {
    auto entityPool = context.getEntityPool();

    Entity* owner = nullptr;

    if(entityPool->hasEntity(ownerId)) {
        owner = entityPool->getEntity(ownerId);
    }

    for(int i = 0; i < numItems; i++) {
        context.getItemController()->addItem(items[i].name, position, items[i].id, owner);
    }
}

void GameClientMessagesReceiver::receiveTakeItems(
    uint32_t entityId,
    int numItems,
    ItemUpdate items[64],
    int turnNumber
) {
    auto entityPool = context.getEntityPool();

    if(!entityPool->hasEntity(entityId)) {
        return;
    }

    auto const& entity = entityPool->getEntity(entityId);

    std::vector<Item*> itemsToTake;

    for(int i = 0; i < numItems; i++) {
        auto item = context.getItemController()->getItem(items[i].id);

        if(item != nullptr) {
            itemsToTake.push_back(item);
        }
    }

    context.getTurnController()->queueAction(std::make_unique<TakeItemAction>(turnNumber, entity, itemsToTake));
}

void GameClientMessagesReceiver::receiveEngagement(int participantIdA, int participantIdB, int type) {
    switch(type) {
        case EngagementEventData::ENGAGED:
            context.getTurnController()->engage(participantIdA, participantIdB);
            break;

        case EngagementEventData::DISENGAGED:
            context.getTurnController()->disengage(participantIdA, participantIdB);
            break;

        default:
            break;
    }
}

void GameClientMessagesReceiver::receiveApplyDamageMessage(int fromId, uint32_t targetId, uint8_t source, int damage) {
    auto entityPool = context.getEntityPool();

    if(!entityPool->hasEntity(targetId)) {
        return;
    }

    auto const& entity = entityPool->getEntity(targetId);

    std::cout << "[" << source << "]: Took " << damage << " damage from " << fromId << std::endl;

    entity->takeDamage(damage);
}
