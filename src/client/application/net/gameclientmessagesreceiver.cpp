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
                findPathMessage->shortStopSteps
            );
            break;
        }

        case (int) GameMessageType::ATTACK_ENTITY: {
            AttackMessage* attackMessage = (AttackMessage*) message;
            receiveAttackEntity(
                attackMessage->entityId,
                attackMessage->x,
                attackMessage->y,
                attackMessage->weaponId
            );
            break;
        }

        case (int) GameMessageType::ACTIONS_ROLL_RESPONSE: {
            ActionsRollResponseMessage* actionsRollResponseMessage = (ActionsRollResponseMessage*) message;
            receiveActionsRollResponse(
                actionsRollResponseMessage->participantId,
                actionsRollResponseMessage->numDice,
                actionsRollResponseMessage->dice
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
            receiveSpawnItems(glm::ivec2(spawnItemsMessage->x, spawnItemsMessage->y), spawnItemsMessage->numItems, spawnItemsMessage->items);
            break;
        }

        default:
            std::cout << "Received unhandled message: " << message << std::endl;
            break;
    }
}

void GameClientMessagesReceiver::receiveGameStateUpdate(const GameStateUpdate& update) {
    // std::cout << "Got game state update " << update.currentParticipant << std::endl;
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
    int shortStopSteps
) {
    if(!context.getEntityPool()->hasEntity(entityId)) {
        return;
    }

    auto const& entity = context.getEntityPool()->getEntity(entityId);

    context.getTurnController()->performMoveAction(entity, position, shortStopSteps);
}

void GameClientMessagesReceiver::receiveAttackEntity(
    uint32_t entityId, 
    int x,
    int y,
    uint32_t weaponId
) {
    auto entityPool = context.getEntityPool();

    if(!entityPool->hasEntity(entityId)) {
        return;
    }

    auto const& entity = entityPool->getEntity(entityId);

    for(auto weapon : entity->getWeapons()) {
        if(weapon->getId() == weaponId) {
            context.getTurnController()->performAttackAction(entity, weapon, glm::ivec2(x, y));
        }
    }
}

void GameClientMessagesReceiver::receiveActionsRollResponse(int participantId, int numDice, DiceActionResult dice[64]) {
    if(participantId == 0) {
        std::vector<int> vActions;
        for(int i = 0; i < numDice; i++) {
            for(int j = 0; j < dice[i].rollNumber; j++) {
                vActions.push_back(dice[i].actions[j]);
            }
        }

        playerController->getDice().setActionsFromServer(vActions);
    }
    else {
        std::map<TurnController::Action, int> vActions = {
            { TurnController::Action::Move, 0 },
            { TurnController::Action::Attack, 0 }
        };

        for(int i = 0; i < numDice; i++) {
            for(int j = 0; j < dice[i].rollNumber; j++) {
                vActions[(TurnController::Action) dice[i].actions[j]]++;
            }
        }

        context.getTurnController()->setActions(participantId, vActions);
    }
}

void GameClientMessagesReceiver::receiveNextTurn(int participantId, int turnNumber) {
    dynamic_cast<ClientTurnController*>(context.getTurnController())->receiveSetNextTurnFlag(participantId, turnNumber);
}

void GameClientMessagesReceiver::receiveSpawnItems(const glm::ivec2& position, int numItems, ItemUpdate items[64]) {
    for(int i = 0; i < numItems; i++) {
        context.getItemController()->addItem(items[i].name, position, items[i].id, false);
    }
}
