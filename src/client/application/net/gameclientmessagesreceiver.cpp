#include "gameclientmessagesreceiver.h"

GameClientMessagesReceiver::GameClientMessagesReceiver(
    ApplicationContext& context,
    uint64_t clientId,
    const std::map<unsigned, bool>& walkableTileIds
) :
    context(context),
    clientId(clientId),
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
        case (int) GameMessageType::ATTACK_ENTITY:              { receiveAttackActor((AttackMessage*) message); break; }
        case (int) GameMessageType::NEXT_TURN:                  { receiveNextTurn((NextTurnMessage*) message); break; }
        case (int) GameMessageType::SPAWN_ITEMS:                { receiveSpawnItems((SpawnItemsMessage*) message); break; }
        case (int) GameMessageType::TAKE_ITEMS:                 { receiveTakeItems((TakeItemsMessage*) message); break; }
        case (int) GameMessageType::APPLY_DAMAGE:               { receiveApplyDamageMessage((ApplyDamageMessage*) message); break; }
        case (int) GameMessageType::APPLY_ENTITY_EFFECT:        { receiveApplyActorEffectMessage((ApplyActorEffectMessage*) message); break; }
        case (int) GameMessageType::APPLY_GRID_EFFECT:          { receiveApplyGridEffectMessage((ApplyGridEffectMessage*) message); break; }
        case (int) GameMessageType::TILES_REVEALED:             { receiveTilesRevealedMessage((TilesRevealedMessage*) message); break; }
        case (int) GameMessageType::SET_ENTITY_POSITION:        { receiveSetActorPositionMessage((SetActorPositionMessage*) message); break; }
        case (int) GameMessageType::ADD_ENTITY_VISIBILITY:      { receiveAddActorVisibilityMessage((AddActorVisibilityMessage*) message); break; }
        case (int) GameMessageType::REMOVE_ENTITY_VISIBILITY:   { receiveRemoveActorVisibilityMessage((RemoveActorVisibilityMessage*) message); break; }
        case (int) GameMessageType::CREATE_ENGAGEMENT:          { receiveCreateEngagementMessage((CreateEngagementMessage*) message); break; }
        case (int) GameMessageType::ADD_TO_ENGAGEMENT:          { receiveAddToEngagementMessage((AddToEngagementMessage*) message); break; }
        case (int) GameMessageType::DISENGAGE:                  { receiveDisenageMessage((DisengageMessage*) message); break; }
        case (int) GameMessageType::REMOVE_ENGAGEMENT:          { receiveRemoveEngagementMessage((RemoveEngagementMessage*) message); break; }
        case (int) GameMessageType::MERGE_ENGAGEMENTS:          { receiveMergeEngagementsMessage((MergeEngagementsMessage*) message); break; }
        case (int) GameMessageType::CREATE_FACTION:             { receiveCreateFactionMessage((CreateFactionMessage*) message); break; }
        case (int) GameMessageType::UPDATE_FACTIONS:            { receiveUpdateFactionsMessage((UpdateFactionsMessage*) message); break; }
        case (int) GameMessageType::SET_FACTION:                { receiveSetFactionMessage((SetFactionMessage*) message); break; }
        case (int) GameMessageType::ADD_FACTION:                { receiveAddFactionMessage((AddFactionMessage*) message); break; }
        case (int) GameMessageType::CHANGE_FACTION_ALIGNMENT:   { receiveChangeFactionAlignmentMessage((ChangeFactionAlignmentMessage*) message); break; }

        default:
            std::cout << "Received unhandled message: " << message->GetType() << std::endl;
            break;
    }
}

void GameClientMessagesReceiver::receiveGameStateUpdate(GameStateUpdateMessage* message) {
    spdlog::trace("Got game state update, num actors: [{}]", message->gameStateUpdate.numActors);
    context.getActorPool()->addGameStateUpdate(message->gameStateUpdate);
}

void GameClientMessagesReceiver::receiveTestMessage(GameTestMessage* message) {
    std::cout << "Received test data " << message->data << std::endl;
}

void GameClientMessagesReceiver::receiveSetParticipant(SetParticipantMessage* message) {
    auto gameController = context.getGameController();

    if(gameController->hasParticipant(message->participantId)) {
        spdlog::info("Participant {} already attached to a client, nothing to do.", message->participantId);
        return;
    }

    auto participant = gameController->addParticipant(message->participantId, message->clientId != 0, { }, nullptr, false);

    // Are we this participant
    if(message->clientId == clientId) {
        playerController->setParticipant(participant);
    }

    if(message->numParticipantsToSet == gameController->getParticipants().size()) {
        gameController->allParticipantsSet();
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
    if(!context.getActorPool()->hasActor(message->actorId)) {
        return;
    }

    auto const& actor = context.getActorPool()->getActor(message->actorId);
    auto participant = context.getGameController()->getParticipant(actor->getParticipantId());
    
    context.getGameController()->queueAction(std::make_unique<MoveAction>(
        participant,
        actor, 
        message->turnNumber, 
        glm::ivec2(message->x, message->y), 
        message->shortStopSteps
    ));
}

void GameClientMessagesReceiver::receiveAttackActor(AttackMessage* message) {
    spdlog::trace("Received attack actor message {} -> ({}, {})", message->actorId, message->x, message->y);
    auto actorPool = context.getActorPool();

    if(!actorPool->hasActor(message->actorId)) {
        return;
    }

    auto weaponId = UUID::fromBytes(message->weaponIdBytes);
    auto const& actor = actorPool->getActor(message->actorId);
    auto participant = context.getGameController()->getParticipant(actor->getParticipantId());

    for(auto weapon : actor->getWeapons()) {
        if(weapon->getId() == weaponId) {
            auto isQueued = context.getGameController()->queueAction(
                std::make_unique<AttackAction>(
                    participant,
                    actor, 
                    message->turnNumber, 
                    weapon, 
                    glm::ivec2(message->x, message->y), 
                    true
                )
            );

            spdlog::trace("Attack action queued: {}", isQueued);
        }
    }
}

void GameClientMessagesReceiver::receiveNextTurn(NextTurnMessage* message) {
    dynamic_cast<ClientGameController*>(context.getGameController())
        ->receiveSetNextTurnFlag(message->participantId, message->engagementId, message->turnNumber);
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
    auto actorPool = context.getActorPool();

    if(!actorPool->hasActor(message->actorId)) {
        return;
    }

    auto const& actor = actorPool->getActor(message->actorId);
    auto participant = context.getGameController()->getParticipant(actor->getParticipantId());

    std::vector<Item*> itemsToTake;

    for(int i = 0; i < message->numItems; i++) {
        auto item = context.getItemController()->getItem(message->items[i].id);

        if(item != nullptr) {
            itemsToTake.push_back(item);
        }
    }

    if(message->turnNumber == -1) {
        context.getGameController()->executeActionImmediately(
            std::make_unique<TakeItemAction>(
                participant,
                actor,
                itemsToTake
            )
        );
    }
    else {
        context.getGameController()->queueAction(
            std::make_unique<TakeItemAction>(
                participant,
                actor,
                message->turnNumber,
                itemsToTake
            )
        );
    }
}

void GameClientMessagesReceiver::receiveApplyDamageMessage(ApplyDamageMessage* message) {
    auto actorPool = context.getActorPool();

    if(!actorPool->hasActor(message->targetId)) {
        return;
    }

    auto const& actor = actorPool->getActor(message->targetId);

    actor->takeDamage(message->damage);

    publish<ApplyDamageEventData>({ message->fromId, actor, (DamageType) message->source, message->damage });
}

void GameClientMessagesReceiver::receiveApplyActorEffectMessage(ApplyActorEffectMessage* message) {
    auto actorPool = context.getActorPool();

    if(!actorPool->hasActor(message->targetId)) {
        return;
    }

    auto const& target = actorPool->getActor(message->targetId);

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
            context.getEffectController()->addEffect(
                std::make_unique<FreezeEffect>(target, message->participantId, stats));
            break;

        case POISON:
            context.getEffectController()->addEffect(
                std::make_unique<PoisonEffect>(target, message->participantId, stats));
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
                message->participantId,
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

void GameClientMessagesReceiver::receiveSetActorPositionMessage(SetActorPositionMessage* message) {
    if(!context.getActorPool()->hasActor(message->actorId)) {
        spdlog::debug("Cannot set position for unrecognized actor with id {}", message->actorId);
        return;
    }

    auto actor = context.getActorPool()->getActor(message->actorId);

    actor->setPosition(glm::ivec2(message->x, message->y));
    actor->setMovesLeft(message->movesLeft);
}


void GameClientMessagesReceiver::receiveRemoveActorVisibilityMessage(RemoveActorVisibilityMessage* message) {
    auto clientParticipant = playerController->getParticipant();

    spdlog::trace(
        "Received RemoveActorVisibilityMessage for actor id {} and participant {}", 
        message->actorId,
        clientParticipant->getId()
    );
    
    if(message->visibleToParticipantId != clientParticipant->getId()) {
        std::cout 
            << "Cannot set visibility for actor with id "
            << message->actorId
            << " message is for participant "
            << message->visibleToParticipantId
            << " and this is participant "
            << clientParticipant->getId()
            << std::endl;
        return;
    }


    if(context.getActorPool()->hasActor(message->actorId)) {
        auto actorToRemove = context.getActorPool()->getActor(message->actorId);
        clientParticipant->removeVisibleActor(actorToRemove);

        context.getActorPool()->removeActor(message->actorId);
    }
    else {
        std::cout << std::format("Warning: removing actor which doesn't exist {}", message->actorId) << std::endl;
    }
}

void GameClientMessagesReceiver::receiveAddActorVisibilityMessage(AddActorVisibilityMessage* message) {
    auto clientParticipant = playerController->getParticipant();

    spdlog::trace(
        "Received AddActorVisibilityMessage for actor id {} and participant {}", 
        message->actor.id,
        clientParticipant->getId()
    );

    if(message->visibleToParticipantId != clientParticipant->getId()) {
        std::cout 
            << "Cannot set visibility for actor with id "
            << message->actor.id
            << " message is for participant "
            << message->visibleToParticipantId
            << " and this is participant "
            << clientParticipant->getId()
            << std::endl;
        return;
    }

    auto actorStateUpdate = message->actor;

    if(context.getActorPool()->hasActor(actorStateUpdate.id)) {
        std::cout << "Actor with id " << actorStateUpdate.id << " already exists and is visible" << std::endl;
        return;
    }

    auto actor = context.getActorPool()->addActor(message->actor.name, message->actor.id);

    if(!context.getGameController()->hasParticipant(message->actor.participantId)) {
        spdlog::warn(
            "Cannot add actor {} which has a non-existant participant {}",
            actor->toString(),
            message->actor.participantId
        );
        return;
    }

    context.getGameController()->getParticipant(message->actor.participantId)->addActor(actor);

    for(int j = 0; j < actorStateUpdate.numWeapons; j++) {
        auto const& weaponUpdate = actorStateUpdate.weaponUpdates[j];
        auto weaponId = UUID::fromBytes(weaponUpdate.idBytes);
        
        if(!actor->hasWeapon(weaponId)) {
            auto weapon = context.getWeaponController()->createWeapon(weaponId, weaponUpdate.name, actor);
            
            if(weapon->getItem() != nullptr && weaponUpdate.hasItem) {
                weapon->getItem()->setId(weaponUpdate.itemId);
            }

            actor->addWeapon(std::move(weapon));
        }
    }

    ActorStateUpdate::deserialize(message->actor, actor);

    if(clientParticipant->hasVisibleActor(actor)) {
        std::cout << std::format("Warning: received already visible actor {}", actor->getId()) << std::endl;
    }
    
    clientParticipant->addVisibleActor(actor);
}

void GameClientMessagesReceiver::receiveCreateEngagementMessage(CreateEngagementMessage* message) {
    auto gameController = context.getGameController();
    auto engagementController = gameController->getEngagementController();

    std::vector<Participant*> participants;

    for(auto i = 0; i < message->numParticipants; i++) {
        auto participant = gameController->getParticipant(message->participants[i]);
        
        if(participant == nullptr) {
            spdlog::error(
                "CreateEngagementMessage invalid: participant {} not found, engagementId: {}",
                message->participants[i],
                message->engagementId
            );
            return;
        }

        participants.push_back(participant);
    }

    engagementController->createEngagement(participants, message->engagementId);
}

void GameClientMessagesReceiver::receiveAddToEngagementMessage(AddToEngagementMessage* message) {
    auto gameController = context.getGameController();
    auto engagementController = gameController->getEngagementController();

    auto participant = gameController->getParticipant(message->participantId);
        
    if(participant == nullptr) {
        spdlog::error(
            "AddToEngagementMessage invalid: participant {} not found, engagementId: {}",
            message->participantId,
            message->engagementId
        );
        return;
    }

    engagementController->addToEngagement(message->engagementId, participant);
}

void GameClientMessagesReceiver::receiveDisenageMessage(DisengageMessage* message) {
    auto gameController = context.getGameController();
    auto engagementController = gameController->getEngagementController();

    auto participant = gameController->getParticipant(message->participantId);
        
    if(participant == nullptr) {
        spdlog::error(
            "DisengageMessage invalid: participant {} not found, engagementId: {}",
            message->participantId,
            message->engagementId
        );
        return;
    }

    engagementController->disengage(message->engagementId, participant);
}

void GameClientMessagesReceiver::receiveRemoveEngagementMessage(RemoveEngagementMessage* message) {
    auto gameController = context.getGameController();
    auto engagementController = gameController->getEngagementController();

    engagementController->removeEngagement(message->engagementId);
}

void GameClientMessagesReceiver::receiveMergeEngagementsMessage(MergeEngagementsMessage* message) {
    auto gameController = context.getGameController();
    auto engagementController = gameController->getEngagementController();

    std::vector<Participant*> participants;

    for(auto i = 0; i < message->numParticipants; i++) {
        auto participant = gameController->getParticipant(message->participants[i]);
        
        if(participant == nullptr) {
            spdlog::error(
                "MergeEngagementsMessage invalid: participant {} not found, engagementId: {}",
                message->participants[i],
                message->newEngagementId
            );
            return;
        }

        participants.push_back(participant);
    }

    engagementController->merge(message->engagementIdA, message->engagementIdB, message->newEngagementId);
}

void GameClientMessagesReceiver::receiveCreateFactionMessage(CreateFactionMessage* message) {
    auto gameController = context.getGameController();
    auto factionController = gameController->getFactionController();

    factionController->createFaction(message->id, message->name);
}

void GameClientMessagesReceiver::receiveUpdateFactionsMessage(UpdateFactionsMessage* message) {
    auto gameController = context.getGameController();
    auto factionController = gameController->getFactionController();

    for(int i = 0; i < message->numFactions; i++) {
        factionController->createFaction(message->factions[i].id, message->factions[i].name);
    }
}

void GameClientMessagesReceiver::receiveSetFactionMessage(SetFactionMessage* message) {
    auto gameController = context.getGameController();
    auto factionController = gameController->getFactionController();

    auto participant = gameController->getParticipant(message->participantId);

    if(participant == nullptr) {
        spdlog::error("SetFactionMessage invalid: participant {} not found", message->participantId);
        return;
    }

    factionController->setParticipantFaction(participant, message->factionId);
}

void GameClientMessagesReceiver::receiveAddFactionMessage(AddFactionMessage* message) {
    auto gameController = context.getGameController();
    auto factionController = gameController->getFactionController();

    auto participant = gameController->getParticipant(message->participantId);

    if(participant == nullptr) {
        spdlog::error("AddFactionMessage invalid: participant {} not found", message->participantId);
        return;
    }

    factionController->addFactionAlignment(participant, message->factionId, (Factioned::Faction::Alignment) message->alignment);
}

void GameClientMessagesReceiver::receiveChangeFactionAlignmentMessage(ChangeFactionAlignmentMessage* message) {
     auto gameController = context.getGameController();
    auto factionController = gameController->getFactionController();

    auto participant = gameController->getParticipant(message->participantId);

    if(participant == nullptr) {
        spdlog::error("ChangeFactionAlignmentMessage invalid: participant {} not found", message->participantId);
        return;
    }

    factionController->changeAlignmentToFaction(
        participant, 
        message->factionId, 
        (Factioned::Faction::Alignment) message->existingAlignemnt,
        (Factioned::Faction::Alignment) message->newAlignemnt
    );
}


