#include "gameclientmessagesreceiver.h"

GameClientMessagesReceiver::GameClientMessagesReceiver(const std::shared_ptr<ApplicationContext>& context) :
    context(context)
{
    turnController = context->getTurnController();
}

void GameClientMessagesReceiver::setTransmitter(const std::shared_ptr<GameClientMessagesTransmitter>& transmitter) {
    this->transmitter = transmitter;
}

void GameClientMessagesReceiver::setPlayerController(const std::shared_ptr<PlayerController>& playerController) {
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
            AttackEntityMessage* attackEntityMessage = (AttackEntityMessage*) message;
            receiveAttackEntity(
                attackEntityMessage->entityId,
                attackEntityMessage->targetId,
                attackEntityMessage->weaponId
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

        default:
            break;
    }
}

void GameClientMessagesReceiver::receiveGameStateUpdate(const GameStateUpdate& update) {
    // std::cout << "Got game state update " << update.currentParticipant << std::endl;

    context->getTurnController()->setCurrentParticipant(update.currentParticipant);
    context->getEntityPool()->addGameStateUpdate(update);
}

void GameClientMessagesReceiver::receiveTestMessage(int data) {
    std::cout << "Received test data " << data << std::endl;
}

void GameClientMessagesReceiver::receiveSetParticipant(
    int participantId,
    int numParticipantsToSet,
    bool isPlayer
) {
    auto const& turnController = context->getTurnController();
    auto const& participant = turnController->addParticipant(participantId, isPlayer, { });

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
    auto const& grid = context->getGrid();

    for(int i = 0; i < block.blockSize; i++) {
        auto x = i / block.width;
        auto y = i % block.width;
        auto id = block.data[i];

        grid->setTile(x, y, { id, id == 1 });
    }
}

void GameClientMessagesReceiver::receiveFindPath(
    uint32_t entityId, 
    const glm::ivec2& position,
    int shortStopSteps
) {
    if(!context->getEntityPool()->hasEntity(entityId)) {
        return;
    }

    auto const& entity = context->getEntityPool()->getEntity(entityId);

    turnController->performMoveAction(entity, position, shortStopSteps);
}

void GameClientMessagesReceiver::receiveAttackEntity(
    uint32_t entityId, 
    uint32_t targetId, 
    uint32_t weaponId
) {
    auto const& entityPool = context->getEntityPool();

    if(!entityPool->hasEntity(entityId) || !entityPool->hasEntity(targetId)) {
        return;
    }

    auto const& entity = entityPool->getEntity(entityId);
    auto const& target = entityPool->getEntity(targetId);

    for(auto [_, weapon] : entity->getWeapons()) {
        if(weapon->getId() == weaponId) {
            turnController->performAttackAction(entity, weapon, target);
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

        playerController->getDice()->setActionsFromServer(vActions);
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

        turnController->setActions(participantId, vActions);
    }
}
