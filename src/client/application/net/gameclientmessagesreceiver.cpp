#include "gameclientmessagesreceiver.h"

// Handle with events?
GameClientMessagesReceiver::GameClientMessagesReceiver(
    ApplicationContext& context, 
    const std::map<unsigned, bool>& walkableTileIds
) :
    context(context),
    walkableTileIds(walkableTileIds)
{ }

void GameClientMessagesReceiver::setTransmitter(GameClientMessagesTransmitter* transmitter) {
    this->transmitter = transmitter;
}

void GameClientMessagesReceiver::setPlayerController(PlayerController* playerController) {
    this->playerController = playerController;
}

void GameClientMessagesReceiver::receiveMessage(yojimbo::Message* message) {
    switch(message->GetType()) {
        case (int) GameMessageType::GAME_STATE_UPDATE:          { receiveGameStateUpdate((GameStateUpdateMessage*) message); break; }
        case (int) GameMessageType::TEST_MESSAGE:               { receiveTestMessage((GameTestMessage*) message); break;}
        case (int) GameMessageType::SET_PARTICIPANT:            { receiveSetParticipant((SetParticipantMessage*) message); break; }
        case (int) GameMessageType::LOAD_MAP:                   { receiveLoadMap((LoadMapMessage*) message); break; }
        case (int) GameMessageType::FIND_PATH:                  { receiveFindPath((FindPathMessage*) message); break; }
        case (int) GameMessageType::ATTACK_ENTITY:              { receiveAttackEntity((AttackMessage*) message); break; }
        case (int) GameMessageType::NEXT_TURN:                  { receiveNextTurn((NextTurnMessage*) message); break; }
        case (int) GameMessageType::SET_TURN:                   { receiveSetTurn((SetTurnMessage*) message); break; }
        case (int) GameMessageType::SPAWN_ITEMS:                { receiveSpawnItems((SpawnItemsMessage*) message); break; }
        case (int) GameMessageType::TAKE_ITEMS:                 { receiveTakeItems((TakeItemsMessage*) message); break; }
        case (int) GameMessageType::ENGAGEMENT:                 { receiveEngagement((EngagementMessage*) message); break; }
        case (int) GameMessageType::APPLY_DAMAGE:               { receiveApplyDamageMessage((ApplyDamageMessage*) message); break; }
        case (int) GameMessageType::APPLY_ENTITY_EFFECT:        { receiveApplyEntityEffectMessage((ApplyEntityEffectMessage*) message); break; }
        case (int) GameMessageType::APPLY_GRID_EFFECT:          { receiveApplyGridEffectMessage((ApplyGridEffectMessage*) message); break; }
        case (int) GameMessageType::TILES_REVEALED:             { receiveTilesRevealedMessage((TilesRevealedMessage*) message); break; }
        case (int) GameMessageType::SET_ENTITY_POSITION:        { receiveSetEntityPositionMessage((SetEntityPositionMessage*) message); break; }
        case (int) GameMessageType::ADD_ENTITY_VISIBILITY:      { receiveAddEntityVisibilityMessage((AddEntityVisibilityMessage*) message); break; }
        case (int) GameMessageType::REMOVE_ENTITY_VISIBILITY:   { receiveRemoveEntityVisibilityMessage((RemoveEntityVisibilityMessage*) message); break; }

        default:
            std::cout << "Received unhandled message: " << message->GetType() << std::endl;
            break;
    }
}

void GameClientMessagesReceiver::receiveGameStateUpdate(GameStateUpdateMessage* message) {
    // std::cout << "Got game state update " << update.currentParticipantId << std::endl;
    context.getEntityPool()->addGameStateUpdate(message->gameStateUpdate);
}

void GameClientMessagesReceiver::receiveTestMessage(GameTestMessage* message) {
    std::cout << "Received test data " << message->data << std::endl;
}

void GameClientMessagesReceiver::receiveSetParticipant(SetParticipantMessage* message) {
    auto turnController = context.getTurnController();
    auto participant = turnController->addParticipant(message->participantId, message->isPlayer, { }, nullptr, false);

    if(message->isPlayer) {
        playerController->setParticipant(participant);
    }

    if(message->numParticipantsToSet == turnController->getParticipants().size()) {
        turnController->allParticipantsSet();
    }

    transmitter->sendSetParticipantAckMessage(message->participantId);
}

void GameClientMessagesReceiver::receiveLoadMap(LoadMapMessage* message) {
    // TODO: Sequencing
    auto block = message->mapBlock;
    auto grid = context.getGrid();
    auto offset = block.sequence * MaxMapBlockSize;

    for(int i = 0; i < block.blockSize; i++) {
        auto x = (i + offset) / block.width;
        auto y = (i + offset) % block.width;
        auto id = block.data[i];

        // grid->setTile(x, y, { id, walkableTileIds[id], false }, false);
    }

    std::cout 
        << "Received map block sequence [" 
        << (block.sequence + 1)
        << "] of [" 
        << block.numSequences
        << "]"
        << std::endl;
}

void GameClientMessagesReceiver::receiveFindPath(FindPathMessage* message) {
    if(!context.getEntityPool()->hasEntity(message->entityId)) {
        return;
    }

    auto const& entity = context.getEntityPool()->getEntity(message->entityId);
    
    context.getTurnController()->queueAction(std::make_unique<MoveAction>(
        message->turnNumber, 
        entity, 
        glm::ivec2(message->x, message->y), 
        message->shortStopSteps
    ));
}

void GameClientMessagesReceiver::receiveAttackEntity(AttackMessage* message) {
    auto entityPool = context.getEntityPool();

    if(!entityPool->hasEntity(message->entityId)) {
        return;
    }

    auto weaponId = UUID::fromBytes(message->weaponIdBytes);
    auto const& entity = entityPool->getEntity(message->entityId);

    for(auto weapon : entity->getWeapons()) {
        if(weapon->getId() == weaponId) {
            context.getTurnController()->queueAction(
                std::make_unique<AttackAction>(
                    message->turnNumber, 
                    entity, 
                    weapon, 
                    glm::ivec2(message->x, message->y), 
                    true
                )
            );
        }
    }
}

void GameClientMessagesReceiver::receiveNextTurn(NextTurnMessage* message) {
    dynamic_cast<ClientTurnController*>(context.getTurnController())
        ->receiveSetNextTurnFlag(message->participantId, message->turnNumber);
}

void GameClientMessagesReceiver::receiveSetTurn(SetTurnMessage* message) {
    context.getTurnController()->setCurrentParticipant(message->currentParticipantId);
    context.getTurnController()->setTurnNumber(message->turnNumber);
}

void GameClientMessagesReceiver::receiveSpawnItems(SpawnItemsMessage* message) {
    for(int i = 0; i < message->numItems; i++) {
        context.getItemController()->addItem(
            message->items[i].name, 
            { message->x, message->y }, 
            message->items[i].id, 
            message->droppedBy
        );
    }
}

void GameClientMessagesReceiver::receiveTakeItems(TakeItemsMessage* message) {
    auto entityPool = context.getEntityPool();

    if(!entityPool->hasEntity(message->entityId)) {
        return;
    }

    auto const& entity = entityPool->getEntity(message->entityId);

    std::vector<Item*> itemsToTake;

    for(int i = 0; i < message->numItems; i++) {
        auto item = context.getItemController()->getItem(message->items[i].id);

        if(item != nullptr) {
            itemsToTake.push_back(item);
        }
    }

    context.getTurnController()->queueAction(std::make_unique<TakeItemAction>(message->turnNumber, entity, itemsToTake));
}

void GameClientMessagesReceiver::receiveEngagement(EngagementMessage* message) {
    context.getTurnController()->queueEngagement(
        message->turnToEngageOn, 
        { 
            message->participantIdA, 
            message->participantIdB, 
            message->type == EngagementType::DISENGAGED 
        }
    );
}

void GameClientMessagesReceiver::receiveApplyDamageMessage(ApplyDamageMessage* message) {
    auto entityPool = context.getEntityPool();

    if(!entityPool->hasEntity(message->targetId)) {
        return;
    }

    auto const& entity = entityPool->getEntity(message->targetId);

    entity->takeDamage(message->damage);

    publish<ApplyDamageEventData>({ message->fromId, entity, (DamageType) message->source, message->damage });
}

void GameClientMessagesReceiver::receiveApplyEntityEffectMessage(ApplyEntityEffectMessage* message) {
    auto entityPool = context.getEntityPool();

    if(!entityPool->hasEntity(message->targetId)) {
        return;
    }

    auto const& target = entityPool->getEntity(message->targetId);

    std::vector<uint32_t> damageTicks;
    for(int i = 0; i < message->effectStats.numDamageTicks; i++) {
        damageTicks.push_back(message->effectStats.damageTicks[i]);
    }

    Stats::EffectStats stats;
    stats.duration = message->effectStats.duration;
    stats.type = (EffectType) message->effectStats.effectType;
    stats.damageTicks = damageTicks;

    switch((EffectType) message->type) {
        case FREEZE:
            context.getEffectController()->addEffect(std::make_unique<FreezeEffect>(target, stats));
            break;

        case POISON:
            context.getEffectController()->addEffect(std::make_unique<PoisonEffect>(target, stats));
            break;

        default:
            break;
    }
}

void GameClientMessagesReceiver::receiveApplyGridEffectMessage(ApplyGridEffectMessage* message) {
    switch((EffectType) message->type) {
        case FREEZE:
            context.getEffectController()->addGridEffect(std::make_unique<GridFreezeEffect>(
                context.getGrid(),
                message->x, 
                message->y, 
                message->duration
            ));
            break;

        default:
            break;
    }
}

void GameClientMessagesReceiver::receiveTilesRevealedMessage(TilesRevealedMessage* message) {
    std::vector<glm::ivec2> tiles;

    auto grid = context.getGrid();

    for(int i = 0; i < message->numRevealedTiles; i++) {
        auto tile = message->revealedTiles[i];

        tiles.push_back({ tile.x, tile.y });
        grid->setTile(tile.x, tile.y, { tile.id, walkableTileIds[tile.id], false, tile.orientation }, false);
    }

    context.getVisibilityController()->revealTiles(message->participantId, tiles);
}

void GameClientMessagesReceiver::receiveSetEntityPositionMessage(SetEntityPositionMessage* message) {
    if(!context.getEntityPool()->hasEntity(message->entityId)) {
        spdlog::debug("Cannot set position for unrecognized entity with id {}", message->entityId);
        return;
    }

    auto entity = context.getEntityPool()->getEntity(message->entityId);

    entity->setPosition(glm::ivec2(message->x, message->y));
    entity->setMovesLeft(message->movesLeft);
}


void GameClientMessagesReceiver::receiveRemoveEntityVisibilityMessage(RemoveEntityVisibilityMessage* message) {
    auto participant = playerController->getParticipant();
    
    if(message->participantId != participant->getId()) {
        std::cout 
            << "Cannot set visibility for entity with id "
            << message->entityId
            << " message is for participant "
            << message->participantId
            << " and this is participant "
            << participant->getId()
            << std::endl;
        return;
    }


    if(context.getEntityPool()->hasEntity(message->entityId)) {
        auto entityToRemove = context.getEntityPool()->getEntity(message->entityId);
        participant->removeVisibleEntity(entityToRemove);

        context.getEntityPool()->removeEntity(message->entityId);
    }
    else {
        std::cout << std::format("Warning: removing entity which doesn't exist {}", message->entityId) << std::endl;
    }
}

void GameClientMessagesReceiver::receiveAddEntityVisibilityMessage(AddEntityVisibilityMessage* message) {
    auto participant = playerController->getParticipant();

    if(message->participantId != participant->getId()) {
        std::cout 
            << "Cannot set visibility for entity with id "
            << message->entity.id
            << " message is for participant "
            << message->participantId
            << " and this is participant "
            << participant->getId()
            << std::endl;
        return;
    }

    auto entityStateUpdate = message->entity;

    if(context.getEntityPool()->hasEntity(entityStateUpdate.id)) {
        std::cout << "Entity with id " << entityStateUpdate.id << " already exists and is visible" << std::endl;
        return;
    }

    auto entity = context.getEntityPool()->addEntity(message->entity.name, message->entity.id);

    for(int j = 0; j < entityStateUpdate.numWeapons; j++) {
        auto const& weaponUpdate = entityStateUpdate.weaponUpdates[j];
        auto weaponId = UUID::fromBytes(weaponUpdate.idBytes);
        
        if(!entity->hasWeapon(weaponId)) {
            auto weapon = context.getWeaponController()->createWeapon(weaponId, weaponUpdate.name, entity);
            
            if(weapon->getItem() != nullptr && weaponUpdate.hasItem) {
                weapon->getItem()->setId(weaponUpdate.itemId);
            }

            entity->addWeapon(std::move(weapon));
        }
    }

    EntityStateUpdate::deserialize(message->entity, entity);

    if(participant->hasVisibleEntity(entity)) {
        std::cout << std::format("Warning: received already visible entity {}", entity->getId()) << std::endl;
    }
    
    participant->addVisibleEntity(entity);
}
